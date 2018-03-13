//
// **************************************************************
// * This is the modelling of the Birmingham Cyclotron Beamline *
// * including                                                  *
// * - the beampipe                                             *
// * - the mylar window                                         *
// * - the current chamber                                      *
// *                                                            *
// * Last updated Thu 21 Mar 2013 14:47:17 GMT (tprice)         *
// **************************************************************
//


#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"

#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UnitsTable.hh"
#include "G4ProductionCuts.hh"
#include "G4UserLimits.hh"
#include "G4SystemOfUnits.hh"

#include "G4SolidStore.hh"
#include "globals.hh"
#include "G4RegionStore.hh"

#include "G4RunManager.hh"

#include "TMath.h"

DetectorConstruction::DetectorConstruction()
: expHall_log(0), 
  beampipe_log(0), 
  alcollimator_log(0), 
  havarwindow_log(0), 
  beampipe_vac_log(0),
  absorber_log(0),
  
  expHall_phys(0),
  beampipe_phys(0),
  alcollimator_phys(0),
  havarwindow_phys(0),
  beampipe_vac_phys(0),
  absorber_phys(0)
{ 
	DefineMaterials();
	SetDefaultColours();
 
  // create commands for interactive definition of the detector  
  detectorConstructionMessenger = new DetectorConstructionMessenger(this);  
	
	//
	// some variables which will be used to calculate positions etc of the placements
	// ------------------------------------------------------------------------------
	
	// the beampipe
	beampipe_outerradius = 4.5*cm;
	beampipe_thickness   = 1.0*cm;
	beampipe_innerradius = beampipe_outerradius - beampipe_thickness;
	beampipe_hz          = 200*cm;
	beampipe_angle       = 0.*deg;
  beampipe_spanning    = 360.*deg;
  
  // the Al collimator within the end of the beampipe
	alcollimator_outerradius = beampipe_innerradius; // sits snuggly inside the beampipe
	alcollimator_innerradius = 2.2*cm; // this parameter is now set to a default value of 2*cm but changed in macros
	alcollimator_hz          = 3.347*cm;
	alcollimator_angle       = 0.0*deg;
	alcollimator_spanning    = 360.0*deg;
	
	// the Havar window at end of beampipe
	havarwindow_thickness   = 25*um;
  havarwindow_outerradius = alcollimator_innerradius+havarwindow_thickness;
  havarwindow_innerradius = alcollimator_innerradius;
  havarwindow_startingPhi = 0.0*deg;
  havarwindow_widthPhi    = 360.0*deg;
  havarwindow_startingEta = 90.0*deg;
  havarwindow_widthEta    = 90.0*deg;
  
  // Proton Monitor Chamber
  chamberposition_z     = 10.5*cm;
  chamber_startingangle = 0.0*deg;
  chamber_spanningangle = 360.0*deg;
  chamber_alring_outerradius   = 50.0*mm;
  chamber_alring_innerradius   = 96.5*mm/2.0;
  chamber_alring_hz            = 1.4*mm;
	chamber_window_outerradius = chamber_alring_innerradius;
  chamber_window_innerradius = 0.0*mm;
  ///chamber_window_hz          = 0.05*mm/2;
	chamber_window_hz          = 0.05*mm; /// NOTE for now keep one window but twice the thicknes 
	chamber_airgap_outerradius = chamber_alring_innerradius;
  chamber_airgap_innerradius = 0.0*mm;
  chamber_airgap_hz          = 1.2*mm;
	/// NOTE need to add some graphite in there
  
    
  // PMMA absorber
  absorber_hx = 4.25*cm;
  absorber_hy = 4.75*cm;
	absorber_hz = 1*mm;
  absorber_posz = chamberposition_z + chamber_alring_hz*2 + absorber_hz + 5*cm; // always so downstream face touches monitor
  absorber_material = Perspex;
	
			// Jig
  // This will require 3 boxes to model correct shapes
  jig_outer_hx       = 5.25*cm;
  jig_outer_hy       = 8.5*cm;
  jig_outer_hz       = 5.75*cm;
  jig_hwallthickness = 0.5*cm;
  jig_airgap_hx      = jig_outer_hx - 2*jig_hwallthickness;
  jig_airgap_hy      = 5.5*cm;
  jig_airgap_hz      = jig_outer_hz;
  jig_posz           = chamberposition_z + chamber_alring_hz*2 + jig_outer_hz;

		validation_hx = 6*cm;
	validation_hy = 6*cm;
	validation_hz = 10*um;
	validation_material = Air;
	validation_pos[0] = G4ThreeVector(0.0,0.0,1.2*cm);
	validation_pos[1] = G4ThreeVector(0.0,0.0,12.0*cm);
	validation_pos[2] = G4ThreeVector(0.0,0.0,59.0*cm);
	validation_pos[3] = G4ThreeVector(0.0,0.0,110.0*cm);
	
	scattererCollimator_radius = 5*mm;
	scattering_material = Air;
	firstScattererPosition = G4ThreeVector(0.0*cm, 0.0*cm, -3200*mm);
	firstScattererSize = G4ThreeVector(0.0*mm, 10.0*mm, 100*um);
	firstScattererThickness = 80*um;
	gafFilmPosition = G4ThreeVector(0.0*cm, 0.0*cm, 45*cm);
	gafFilmSize = G4ThreeVector(5*cm, 5*cm, 0.0*cm); // do not allow Z to change so hard code later
	scattererInVacuum = true;
	reverseFilm = false;
	
	silicon_pos = 11.2*mm;
	
	constructWaterTank = false;
	constructScatteringSystem = false;
	
	filmType = "None";

  }

DetectorConstruction::~DetectorConstruction()
{
	delete detectorConstructionMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	// DEFINE THE ACTUAL GEOMETRY
	// ==========================
	
	// Clean old geometry, if any
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
	
	// world volume which holds everything
	// -----------------------------------
	G4double expHall_x = 2*m;
  G4double expHall_y = 1.5*m;
  G4double expHall_z = 5.00*m;
	
  expHall_box = new G4Box("expHall_box", expHall_x,expHall_y,expHall_z);
  expHall_log = new G4LogicalVolume( expHall_box, Air, "experimentalHall_log" );
	expHall_log->SetVisAttributes (expHall_vis);
  expHall_phys = new G4PVPlacement( 0, G4ThreeVector(), expHall_log, "experimentalHall", 0, false, 0 );
	
	
	ConstructBeamline(); 	
	//ConstructValidation();
	
	ConstructPRaVDATable();
	if(constructWaterTank)			 ConstructWaterTank();
	if(constructScatteringSystem)	ConstructScatteringSystem();
	
	if(filmType == "EBT3" || filmType == "HD-810")
		ConstructGaFChromicFilm();
 	return expHall_phys;
}



void DetectorConstruction::ConstructBeamline()
{  
	G4double beampipe_window_hZ = 12.5*um;
	G4Tubs* beampipe_window = new G4Tubs("beampipe_window", 0.0*cm, 2.5*cm, beampipe_window_hZ, 0*deg, 360*deg);
	G4LogicalVolume* beampipe_window_log = new G4LogicalVolume(beampipe_window, Ti, "beampipe_window");
	new G4PVPlacement(0, G4ThreeVector(0.0,0.0,0*cm), beampipe_window_log, "beampipe_window", expHall_log, false, 0);
	//NOTE: all the geometry is now set from the window at 0,0,0
	
	// nthe design of the new nozzle
	G4double nozzle_thin_hZ = 3*cm;
	G4double nozzle_thick_hZ = 0.5*cm;
	G4double nozzle_mid_hZ = .5*cm;
 	G4Tubs* nozzle_thin = new G4Tubs("nozzle_thin", 25*mm, beampipe_outerradius, nozzle_thin_hZ, 0*deg, 360*deg);
  G4Tubs* nozzle_thick = new G4Tubs("nozzle_thick", 25*mm, 6*cm, nozzle_thick_hZ, 0*deg, 360*deg);
  G4UnionSolid* nozzle_back = new G4UnionSolid("nozzle_back", nozzle_thin, nozzle_thick, 0, G4ThreeVector(0.0*cm, 0.0*cm, nozzle_thin_hZ+nozzle_thick_hZ));
	G4Tubs* nozzle_mid = new G4Tubs("nozzle_mid", 5.1*cm, 6*cm, nozzle_mid_hZ, 0*deg, 360*deg);
	G4UnionSolid* nozzle_front = new G4UnionSolid("nozzle_front", nozzle_mid, nozzle_thick, 0, G4ThreeVector(0,0,nozzle_mid_hZ+nozzle_thick_hZ));
	G4UnionSolid* nozzle = new G4UnionSolid("nozzle", nozzle_back, nozzle_front, 0, G4ThreeVector(0,0,nozzle_mid_hZ+nozzle_thick_hZ+nozzle_thin_hZ+nozzle_thick_hZ));
 	G4LogicalVolume* nozzle_log = new G4LogicalVolume(nozzle, Al, "nozzle");
 	new G4PVPlacement(0, G4ThreeVector(0,0,-nozzle_thin_hZ), nozzle_log, "nozzle", expHall_log, false, 0);
	
	// new collimator held onto the nozzle (for now just circular holes
	G4Box* beampipe_col_whole = new G4Box("beampipe_col_whole", 5.5*cm/2, 5.5*cm/2, 0.5*cm);
	G4Tubs* beampipe_col_hole = new G4Tubs("beampipe_col_hole", 0.0*cm, alcollimator_innerradius, 1.01*cm, 0*deg,360*deg);
	G4SubtractionSolid* beampipe_col = new G4SubtractionSolid("beampipe_col", beampipe_col_whole, beampipe_col_hole);
	G4LogicalVolume* beampipe_col_log = new G4LogicalVolume(beampipe_col, Al, "beampipe_col");
	G4ThreeVector col_pos = G4ThreeVector(0,0,nozzle_mid_hZ*2 + nozzle_thick_hZ*4+0.5*cm);
	new G4PVPlacement(0, col_pos, beampipe_col_log, "beampipe_col", expHall_log, false, 0);

	G4Tubs* beampipe = new G4Tubs("beampipe", beampipe_innerradius, beampipe_outerradius, beampipe_hz, beampipe_angle, beampipe_spanning);
	
	// beam pipe supports
	G4Box* support_box = new G4Box("support_box", 3*cm, 60*cm, 3*cm);
	G4LogicalVolume* support_log = new G4LogicalVolume(support_box, Al, "support");
	new G4PVPlacement(0, G4ThreeVector(0,-60*cm,-300*cm), support_log, "support", expHall_log, false, 0);
	new G4PVPlacement(0, G4ThreeVector(0,-60*cm,-400*cm), support_log, "support", expHall_log, false, 1);
	
	G4RotationMatrix* rot_magnet = new G4RotationMatrix();
	rot_magnet->rotateX(90*deg);
	
	// surround to the magent
	G4double magnet_r = 250*mm;
	G4double magnet_height = 100*mm;
	G4double magnet_pos = -750	*mm;
	
	G4Box* magnet_box_whole = new G4Box("magnet_box_whole", magnet_r+25*mm, magnet_height+25*mm, magnet_r+25*mm);
	G4Box* magnet_box_hole = new G4Box("magnet_box_hole", magnet_r, magnet_height, magnet_r+25*mm);
	G4SubtractionSolid* magnet_box = new G4SubtractionSolid("magnet_box", magnet_box_whole, magnet_box_hole,0,G4ThreeVector(0,0,0));
	G4LogicalVolume* magnet_box_log = new G4LogicalVolume(magnet_box, Al, "magnet_box");
		magnet_box_log->SetVisAttributes(new G4VisAttributes(G4Colour::Yellow()));
	new G4PVPlacement(0,G4ThreeVector(0,0,magnet_pos-(2*nozzle_thin_hZ)), magnet_box_log, "magnet_box", expHall_log, false, 0);
	
	// support structure beneath the magnet
	G4Box* magnet_support = new G4Box("magnet_support", 4*cm, 50*cm, 4*cm);
	G4LogicalVolume* magnet_support_log = new G4LogicalVolume(magnet_support, Al, "magnet_support");
		magnet_support_log->SetVisAttributes(new G4VisAttributes(G4Colour::Gray()));
	new G4PVPlacement(0,G4ThreeVector(magnet_r+25*mm-4*cm,-(50*cm+magnet_height+25*mm), magnet_pos-(2*nozzle_thin_hZ)-magnet_r+4*cm-25*mm), magnet_support_log,"magnet_support",expHall_log,false,0);
	new G4PVPlacement(0,G4ThreeVector(magnet_r+25*mm-4*cm,-(50*cm+magnet_height+25*mm), magnet_pos-(2*nozzle_thin_hZ)+magnet_r-4*cm+25*mm), magnet_support_log,"magnet_support",expHall_log,false,1);
	new G4PVPlacement(0,G4ThreeVector(-(magnet_r+25*mm-4*cm),-(50*cm+magnet_height+25*mm), magnet_pos-(2*nozzle_thin_hZ)-magnet_r+4*cm-25*mm), magnet_support_log,"magnet_support",expHall_log,false,2);
	new G4PVPlacement(0,G4ThreeVector(-(magnet_r+25*mm-4*cm),-(50*cm+magnet_height+25*mm), magnet_pos-(2*nozzle_thin_hZ)+magnet_r-4*cm+25*mm), magnet_support_log,"magnet_support",expHall_log,false,3);	
	 
	//a very thin area around the magnet to allow colour change....
	G4Tubs* MagnetColor = new G4Tubs("magentcolor", magnet_r, magnet_r+1*mm, magnet_height, 0, CLHEP::twopi);
	G4LogicalVolume* MagnetColorLog = new G4LogicalVolume(MagnetColor, Al, "MagnetColor");
	MagnetColorLog->SetVisAttributes(new G4VisAttributes(new G4Colour(255,160,0)));
	G4ThreeVector mc_pos = G4ThreeVector(0,0,magnet_pos-(2*nozzle_thin_hZ));
		new G4PVPlacement(rot_magnet,mc_pos, MagnetColorLog, "MagnetColor", expHall_log, false, 0);
		
	// add the nozzle at -30 and +30 degrees to make it look fancy
	G4ThreeVector noz_pos = G4ThreeVector(0,0,magnet_r+1*mm);noz_pos.rotateY(30*deg);noz_pos.setZ(noz_pos.getZ()+magnet_pos-(1*nozzle_thin_hZ)+(1*nozzle_thick_hZ));
	G4RotationMatrix* noz_rot = new G4RotationMatrix();noz_rot->rotateY(-30*deg);	
	 new G4PVPlacement(noz_rot,noz_pos,nozzle_log,"MagnetBP0", expHall_log, false, 0);
	G4ThreeVector noz_pos2 = G4ThreeVector(0,0,magnet_r+1*mm);noz_pos2.rotateY(-30*deg);noz_pos2.setZ(noz_pos2.getZ()+magnet_pos-(1*nozzle_thin_hZ)+(1*nozzle_thick_hZ));
	G4RotationMatrix* noz_rot2 = new G4RotationMatrix();noz_rot2->rotateY(30*deg);
	 new G4PVPlacement(noz_rot2,noz_pos2,nozzle_log,"MagnetBP2", expHall_log, false, 0);	
	 
	
	// magnet volume to add to the beam pipe
	G4Tubs* Magnet1 = new G4Tubs("magnet1", 0*mm, magnet_r, magnet_height, beampipe_angle, beampipe_spanning); //outer part of magnet
	G4Tubs* Magnet_Vac = new G4Tubs("magnet_Vac", 0*mm, magnet_r-30*mm, magnet_height-10*mm, beampipe_angle, beampipe_spanning);  //inner vaccum part of magnet
	G4SubtractionSolid* MagnetTemp = new G4SubtractionSolid("magnetTemp", Magnet1, Magnet_Vac,0,G4ThreeVector(0,0,0)); // take the vacuu2m part out
	G4Tubs* Magnet_pipe = new G4Tubs("temp",0*mm,beampipe_outerradius,magnet_r,beampipe_angle, beampipe_spanning);
	G4SubtractionSolid* Magnet = new G4SubtractionSolid("magnet", MagnetTemp, Magnet_pipe,rot_magnet,G4ThreeVector(0,0,0));
	G4SubtractionSolid* beampipe_minusMag = new G4SubtractionSolid("beampipe", beampipe, Magnet1, rot_magnet, G4ThreeVector(0,0,beampipe_hz+magnet_pos));
  
	
	G4UnionSolid* temp_a = new G4UnionSolid("temp_a", beampipe, Magnet1,rot_magnet, G4ThreeVector(0,0,beampipe_hz+magnet_pos)	);
	G4SubtractionSolid* temp_b = new G4SubtractionSolid("temp_b", temp_a, Magnet_Vac,rot_magnet, G4ThreeVector(0,0,beampipe_hz+magnet_pos)	);
	G4SubtractionSolid* temp_c = new G4SubtractionSolid("temp_c", temp_b, Magnet_pipe,0,G4ThreeVector(0,0,beampipe_hz+magnet_pos)	);
	
	beampipe_log = new G4LogicalVolume(temp_c, Steel, "beampipe_log");
  //beampipe_log->SetVisAttributes(beampipe_vis);
	G4ThreeVector beampipe_pos = G4ThreeVector(0,0,-(beampipe_hz+nozzle_thin_hZ*2));
  new G4PVPlacement(0, beampipe_pos, beampipe_log, "beamPipe", expHall_log, false, 0);
		
	// add together all the beamline aspects and make vacuum structure inside it
	G4Tubs* vac_thin = new G4Tubs("vac_thin", 0.0*cm, 25*mm, nozzle_thin_hZ, 0*deg, 360*deg);
	G4Tubs* vac_pipe = new G4Tubs("vac_pip", 0.0*cm, beampipe_innerradius, beampipe_hz, beampipe_angle, beampipe_spanning);
	G4UnionSolid* vac = new G4UnionSolid("vac", vac_pipe, vac_thin, 0, G4ThreeVector(0.0,0.0,nozzle_thin_hZ+beampipe_hz));
	vac = new G4UnionSolid("vac", vac, Magnet_Vac,rot_magnet, G4ThreeVector(0,0,beampipe_hz+magnet_pos));
	vac_log = new G4LogicalVolume(vac, Vacuum, "vacuum");
	//vac_log->SetVisAttributes(beampipe_vac_vis);
	G4ThreeVector vacuum_pos = G4ThreeVector(0,0,-(beampipe_hz+(nozzle_thin_hZ*2)+beampipe_window_hZ));
	new G4PVPlacement(0, vacuum_pos, vac_log, "vacuum", expHall_log, false, 0);
	
	// large wall between vault and magnet room
	G4double wall_zpos = -2000*mm;
	G4Box* wall = new G4Box("wall",2*m,1.5*m,0.25*m);
	G4Tubs* hole = new G4Tubs("hole",0*cm,7.5*cm,1*m,0,CLHEP::twopi);
	G4SubtractionSolid* Wall = new G4SubtractionSolid("Wall",wall,hole);
 	G4LogicalVolume* Wall_log = new G4LogicalVolume(Wall,G4NistManager::Instance()->FindOrBuildMaterial("G4_CONCRETE"),"Wall");
	new G4PVPlacement(0,G4ThreeVector(0,0,wall_zpos), Wall_log, "Wall", expHall_log,false,0);

	// Beam monitor
	chamberposition_z = 0*mm + beampipe_window_hZ + chamber_alring_hz + (nozzle_thick_hZ*2) ;
	ConstructBeamMonitor(chamberposition_z);  

  // PMMA absorberg
	if(absorber_hz>0*mm)
	{
		absorber = new G4Box("absorber", absorber_hx, absorber_hy, absorber_hz);
		absorber_log = new G4LogicalVolume(absorber, absorber_material, "absorber_log");
		absorber_log->SetVisAttributes(absorber_vis);
		G4double uStepMin = 10*um;
		G4double uTrakMin = 10*um;
		G4UserLimits* stepLimit = new G4UserLimits(uStepMin,uTrakMin);
		absorber_log->SetUserLimits(stepLimit);
		
		if(absorberRegion) delete absorberRegion;
			absorberRegion = new G4Region("absorberRegion");
			absorberRegion->AddRootLogicalVolume(absorber_log);
			absorber_log->SetRegion(absorberRegion);

		absorber_phys = new G4PVPlacement(0, G4ThreeVector(0.0,0.0, +absorber_posz), absorber_log, "absorber", expHall_log, false,0);	
		G4cout << "\nAbsorber:" << G4endl;
		G4cout << "  Material = " << absorber_material  << G4endl;
		G4cout << "  Depth    = " << G4BestUnit(absorber_hz*2, "Length") <<  G4endl << G4endl;
		
		G4cout << "#### Absorber posz" << G4BestUnit(absorber_posz,"Length") << G4endl;
	}
				
	// PMMA Jig
	//ConstructJig();
}	

void DetectorConstruction::ConstructPRaVDATable()
{
	G4double beam_hthick = 4*cm;
	G4double table_hZ = 50*cm;
	G4double table_hX = 25*cm;
	G4double table_hY = 50*cm;
	
	G4Box* hor_z = new G4Box("hor_z", beam_hthick,beam_hthick,table_hZ);
	G4Box* hor_x = new G4Box("hor_x", table_hX, beam_hthick, beam_hthick);
	G4Box* vert_y = new G4Box("vert_y", beam_hthick, table_hY, beam_hthick);
	
	G4UnionSolid* table_upright1 = new G4UnionSolid("table_upright1", vert_y, vert_y, 0, G4ThreeVector(0,0,beam_hthick*2.0+table_hZ*2.0));
	G4UnionSolid* table_uprights = new G4UnionSolid("table_uprights", table_upright1, table_upright1, 0, G4ThreeVector(beam_hthick*2.0+table_hX*2.0,0,0));
	//
	G4UnionSolid* table_horZ = new G4UnionSolid("table_horZ", hor_z, hor_z, 0, G4ThreeVector(0,beam_hthick+table_hY,0));
	G4UnionSolid* table_horZs = new G4UnionSolid("table_horZs", table_horZ, table_horZ, 0, G4ThreeVector((beam_hthick+table_hX)*2,0,0));

	G4UnionSolid* table_horX = new G4UnionSolid("table_horX", hor_x, hor_x, 0, G4ThreeVector(0,beam_hthick+table_hY,0));
	G4UnionSolid* table_horXs = new G4UnionSolid("table_horXs", table_horX, table_horX, 0, G4ThreeVector(0,0,(beam_hthick+table_hZ)*2));
	
 	G4UnionSolid* table_temp = new G4UnionSolid("table_temp", table_uprights, table_horZs, 0, G4ThreeVector(0*cm,-20*cm-beam_hthick*2,+table_hZ+beam_hthick));
 	G4UnionSolid* table = new G4UnionSolid("table", table_temp, table_horXs, 0, G4ThreeVector(table_hX+beam_hthick,-20*cm-beam_hthick*2,0));	
		
	G4LogicalVolume* table_log = new G4LogicalVolume(table,Al,"table");
	new G4PVPlacement(0, G4ThreeVector(-beam_hthick-table_hX,-table_hY,0), table_log, "table", expHall_log, false, 0);
}

void DetectorConstruction::ConstructBeamMonitor(G4double z)
{
	 // Beam monitor
  // needs a ring, airgap and two windows
	chamber_alring = new G4Tubs("chamber_alring", chamber_alring_innerradius, chamber_alring_outerradius, chamber_alring_hz, chamber_startingangle, chamber_spanningangle);
	chamber_alring_log = new G4LogicalVolume(chamber_alring,  Al, "chamber_alring_log");
 	chamber_alring_log->SetVisAttributes (chamber_alring_vis);
	chamber_alring_phys = new G4PVPlacement(0, G4ThreeVector(0.0,0.0, +chamberposition_z), chamber_alring_log, "chamber_AlRing", expHall_log, false,0);  
	
	chamber_airgap = new G4Tubs("chamber_airgap", chamber_airgap_innerradius, chamber_airgap_outerradius, chamber_airgap_hz, chamber_startingangle, chamber_spanningangle);
	chamber_airgap_log = new G4LogicalVolume(chamber_airgap, Air, "chamber_airgap_log");
 	chamber_airgap_log->SetVisAttributes (chamber_airgap_vis);
	chamber_airgap_phys = new G4PVPlacement(0, G4ThreeVector(0.0,0.0, +chamberposition_z), chamber_airgap_log, "chamberAirGap", expHall_log, false,0);
		 
	chamber_window = new G4Tubs("chamber_window", chamber_window_innerradius, chamber_window_outerradius, chamber_window_hz, chamber_startingangle, chamber_spanningangle);                                   
  chamber_window_log = new G4LogicalVolume(chamber_window, Kapton, "chamber_window");
  chamber_window_log->SetVisAttributes (chamber_window_vis);
  chamber_window_phys1 = new G4PVPlacement(0, G4ThreeVector(0.0,0.0, +chamberposition_z + chamber_airgap_hz + chamber_window_hz), chamber_window_log, "chamberWindow1", expHall_log,true, 0);
	chamber_window_phys2 = new G4PVPlacement(0, G4ThreeVector(0.0,0.0, +chamberposition_z - chamber_airgap_hz - chamber_window_hz), chamber_window_log, "chamberWindow2", expHall_log, true, 1);
}

void DetectorConstruction::SetDefaultColours()
{
	expHall_vis         = new G4VisAttributes();                       expHall_vis->SetVisibility(false);
	beampipe_vac_vis    = new G4VisAttributes(G4Colour::Blue());                       beampipe_vac_vis->SetVisibility(true);
	beampipe_vis        = new G4VisAttributes(G4Colour::Grey());       beampipe_vis->SetVisibility(true);
	alcollimator_vis    = new G4VisAttributes(G4Colour::Red());        alcollimator_vis->SetVisibility(true);
	havarwindow_vis     = new G4VisAttributes(G4Colour::Yellow());     havarwindow_vis->SetVisibility(true);
	chamber_alring_vis  = new G4VisAttributes(G4Colour::Gray());       chamber_alring_vis->SetVisibility(true);
	chamber_airgap_vis  = new G4VisAttributes();                       chamber_airgap_vis->SetVisibility(false);
	chamber_window_vis  = new G4VisAttributes(G4Colour(1.,0.5,0.));    chamber_window_vis->SetVisibility(true);
	absorber_vis        = new G4VisAttributes(G4Colour::Blue());       absorber_vis->SetVisibility(true);
	jig_vis             = new G4VisAttributes(G4Colour::Green());      jig_vis->SetVisibility(true);
	dynamite_Si3N4_vis  = new G4VisAttributes();                       dynamite_Si3N4_vis->SetVisibility(true);
	dynamite_SiO2_vis   = new G4VisAttributes();                       dynamite_SiO2_vis->SetVisibility(true);
	dynamite_epi_vis    = new G4VisAttributes(G4Colour::Yellow());     dynamite_epi_vis->SetVisibility(true);
	dynamite_sub_vis    = new G4VisAttributes(G4Colour(1.,0.5,0.));    dynamite_sub_vis->SetVisibility(true);
	dynamite_back_vis   = new G4VisAttributes(G4Colour::Grey());       dynamite_back_vis->SetVisibility(true);
	tracker_vis         = new G4VisAttributes(G4Colour(1.,0.5,0.));    tracker_vis->SetVisibility(true);
	tracker_pcb_vis     = new G4VisAttributes();                       tracker_pcb_vis->SetVisibility(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCollimatorInnerRadius(G4double r)
{
	assert(r<=beampipe_innerradius);
	alcollimator_innerradius = r;
}

G4double DetectorConstruction::GetCollimatorInnerRadius()
{
	return alcollimator_innerradius;
}

G4double DetectorConstruction::GetBeamPipeInnerRadius()
{
	return beampipe_innerradius;
}

void DetectorConstruction::ConstructJig()
{
// This will require 3 solids and some subtractions and additions
  G4VSolid* jig_volume = new G4Box("jig_volume",jig_outer_hx,jig_outer_hy,jig_outer_hz);                                   
  G4VSolid* jig_air = new G4Box("jig_cutout",jig_airgap_hx,  jig_airgap_hy, jig_airgap_hz);  
  G4VSolid* jig_brace = new G4Box("jig_brace",jig_outer_hx - 2*jig_hwallthickness, jig_hwallthickness,jig_hwallthickness);  
  // remove the brace from air volume                               
  G4VSolid* jig_minus_air = new G4SubtractionSolid("jig_minus_air",jig_volume,jig_air, 0,G4ThreeVector(0,3.0*cm,0.0));                                 
  G4VSolid* jf = new G4UnionSolid("jig_frame", jig_minus_air,jig_brace,0,G4ThreeVector(0,jig_outer_hy-jig_hwallthickness,-jig_outer_hz+jig_hwallthickness));
	
	jig_log = new G4LogicalVolume(jf, Perspex, "jig_log");
  jig_log->SetVisAttributes(jig_vis);
 	jig_phys = new G4PVPlacement(0, G4ThreeVector(0.0, -2.25*cm, +jig_posz), jig_log, "jig", expHall_log, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsDepth(G4double value)
{
	absorber_hz = value/2;
	absorber_posz = chamberposition_z + chamber_alring_hz*2 + absorber_hz + 10*cm;
}

G4double DetectorConstruction::GetAbsorberDepth()
{
	return absorber_hz*2; 
}

G4double DetectorConstruction::GetAbsorberPosition()
{
	return absorber_posz;
}

void DetectorConstruction::SetAbsMaterial(G4String material)
{
  G4Material* pttoMaterial = G4Material::GetMaterial(material);  
  if (pttoMaterial)
     {
     		absorber_material = pttoMaterial;
     }  
  else {
  	G4cout << "### Cannot find material checking database" << material << G4endl;
		
		G4NistManager* man = G4NistManager::Instance();
		G4Material* pttoMaterial = man->FindOrBuildMaterial(material);
		if (pttoMaterial)
     {
     		absorber_material = pttoMaterial;
     } 
     else
			 G4cout << "### Still cannot find material" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{	
	// DEFINE ALL THE NECCESSARY MATERIALS
	// ===================================
	
	G4double density(0);     // material density
	G4int    ncomponents(0); // number of components in a material
	G4double fractionmass(0);// fractional mass of a component in a material
	
	G4NistManager* man = G4NistManager::Instance(); // allows materials to be accesses in the material database
	
	// Elements (Define as pure materials as easier to work with)
	// --------
	
	// silicon
	Si = man->FindOrBuildMaterial("G4_Si");	
	// aluminium
	Al = man->FindOrBuildMaterial("G4_Al");	
	// tantalum
	Ta = man->FindOrBuildMaterial("G4_Ta");
	// chrominium
	Cr = man->FindOrBuildMaterial("G4_Cr");
	// iron
	Fe = man->FindOrBuildMaterial("G4_Fe");
	// nickel
	Ni = man->FindOrBuildMaterial("G4_Ni");
	// oxygen
	O = man->FindOrBuildMaterial("G4_O");
	// carbon
	C = man->FindOrBuildMaterial("G4_C");
	// hydrogen
	H = man->FindOrBuildMaterial("G4_H");
	// nitrogen
	N = man->FindOrBuildMaterial("G4_N");
	// Manganese
	Mn = man->FindOrBuildMaterial("G4_Mn");
	// Molybdenum
	Mo = man->FindOrBuildMaterial("G4_Mo");
	// Tungsten
	W = man->FindOrBuildMaterial("G4_W");
	// Cobalt
	Co = man->FindOrBuildMaterial("G4_Mo");
	
	Be = man->FindOrBuildMaterial("G4_Be");
	
	Cu = man->FindOrBuildMaterial("G4_Cu");
	
	Mg = man->FindOrBuildMaterial("G4_Mg");
	
	Ti = man->FindOrBuildMaterial("G4_Ti");
	
	G4Material* Li = man->FindOrBuildMaterial("G4_Li");
	
	G4Material* Cl = man->FindOrBuildMaterial("G4_Cl");
	
	
	// Compounds
	// ---------
	
	// water
	Water = man->FindOrBuildMaterial("G4_WATER");	
	// air
	Air = man->FindOrBuildMaterial("G4_AIR");	
	// vacuum (air but with a tiny density)
	Vacuum = man->FindOrBuildMaterial("G4_Galactic");	
	// stainless steel (to match fluka)
	Steel = new G4Material("Steel", density=8*g/cm3, ncomponents=3); 
	Steel->AddMaterial(Cr, fractionmass=0.18);
	Steel->AddMaterial(Fe, fractionmass=0.74);
	Steel->AddMaterial(Ni, fractionmass=0.08);
	// Mylar (to match fluka)
	Mylar = man->FindOrBuildMaterial("G4_MYLAR");
	//Mylar = new G4Material("Mylar", density=1.397*g/cm3, ncomponents=3);
	//Mylar->AddMaterial(H, fractionmass=0.3636);
	//Mylar->AddMaterial(C, fractionmass=0.45451);
	//Mylar->AddMaterial(O, fractionmass=0.1818);
	// Kapton (to match fluka)
	//Kapton = new G4Material("Kapton", density=1.42*g/cm3, ncomponents=4);
	//Kapton->AddMaterial(C, fractionmass=0.6911);
	//Kapton->AddMaterial(H, fractionmass=0.0262);
	//Kapton->AddMaterial(N, fractionmass=0.0733);
	//Kapton->AddMaterial(O, fractionmass=0.2094);
	Kapton = man->FindOrBuildMaterial("G4_KAPTON");
	// Perspex
	Perspex = man->FindOrBuildMaterial("G4_PLEXIGLASS");
	/*Perspex = new G4Material("Perspex", density=1.19*g/cm3, ncomponents=3);
	Perspex->AddMaterial(C, fractionmass=0.59984);//0.600);
	Perspex->AddMaterial(O, fractionmass=0.31962);//0.320);
	Perspex->AddMaterial(H, fractionmass=0.08054);//0.080);
	Perspex->GetIonisation()->SetMeanExcitationEnergy(74.0*eV);*/
	G4cout << "Perspex->GetIonisation()->GetMeanExcitationEnergy() = " <<  Perspex->GetIonisation()->GetMeanExcitationEnergy()/eV << G4endl;
	// Havar (to match fluka)
	Havar = new G4Material("Havar", density=8.3*g/cm3, ncomponents=8);
	Havar->AddMaterial(C,  0.9648*perCent);
	Havar->AddMaterial(Cr, 22.2858*perCent);
	Havar->AddMaterial(Mn, 1.6874*perCent);
	Havar->AddMaterial(Fe, 18.1139*perCent);
	Havar->AddMaterial(Co, 41.7828*perCent);
	Havar->AddMaterial(Ni, 12.8336*perCent);
	Havar->AddMaterial(Mo, 1.4494*perCent);
	Havar->AddMaterial(W,  0.8823*perCent);
	
	//Dural for dynamite back
	Dural = new G4Material("Dural", density=2.800*g/cm3, ncomponents=3);
	Dural->AddMaterial(Al, 90.19987*perCent);
	Dural->AddMaterial(Cu, 8.9448*perCent);
	Dural->AddMaterial(Mg, 0.855*perCent);
	
	// Silicon Nitride
	Si3N4 = new G4Material("SiliconNitride", density=3.2*g/cm3, ncomponents=2);
	Si3N4->AddMaterial(Si, fractionmass=0.6);
	Si3N4->AddMaterial(N, fractionmass=0.4);
	
	SiO2 = man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
	
	// GafChroimic HD-810 sensitive layer
	gafFilm_material = new G4Material("GaFChromicSensitive", density=1.08*g/cm3, ncomponents=4);
	gafFilm_material->AddMaterial(C, fractionmass=0.2914);
	gafFilm_material->AddMaterial(H, fractionmass=0.5680);
	gafFilm_material->AddMaterial(O, fractionmass=0.0712);
	gafFilm_material->AddMaterial(N, fractionmass=0.0694);
	
	// sensitive EBT3 material
	G4int natoms = 0;
	LiPCD = new G4Material("LiPCD", density=1.2*g/cm3, ncomponents=4);
	LiPCD->AddElement(man->FindOrBuildElement("C"), natoms=25);
	LiPCD->AddElement(man->FindOrBuildElement("H"), natoms=41);
	LiPCD->AddElement(man->FindOrBuildElement("O"), natoms=2);
	LiPCD->AddElement(man->FindOrBuildElement("Li"), natoms=1);
	
	// EBT3 plastic material
	PET = new G4Material("PET",density=1.38*g/cm3,ncomponents=3);
	PET->AddElement(man->FindOrBuildElement("C"), natoms=10);
	PET->AddElement(man->FindOrBuildElement("H"), natoms=8);
	PET->AddElement(man->FindOrBuildElement("O"), natoms=4);
	//http://www.paryleneengineering.com/physical_properties_of_parylene.html
	
	Parylene = new G4Material("Parylene", density = 1.289*g/cm3, ncomponents=3);
	Parylene->AddElement(man->FindOrBuildElement("C"), natoms = 16);
	Parylene->AddElement(man->FindOrBuildElement("H"), natoms = 12);
	Parylene->AddElement(man->FindOrBuildElement("Cl"), natoms = 2);
	
	absorber_material = Perspex;

	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetWaterTankConstruction(bool construct)
{
	constructWaterTank = construct;
}

bool DetectorConstruction::GetWaterTankConstruction()
{
	return constructWaterTank;
}

void DetectorConstruction::SetWaterTankWindowThickness(G4double thickness)
{
	wt_window_hz = thickness / 2;
	//if(constructWaterTank) absorber_hz = wt_window_hz;
	//absorber_posz = chamberposition_z + chamber_alring_hz*2 + absorber_hz;
}

void DetectorConstruction::ConstructWaterTank()
{
	wt_window_hz = 1.219*mm/2;
	wt_depth_hz = 1.0*cm;
	wt_posz = 23.5*cm; // this is the distance to the front of the tank
	
	// collimator
 	G4Box* col1 = new G4Box("col1", 2*cm, 2*cm, 1*mm);
 	G4Tubs* col2 = new G4Tubs("col2", 0.0*mm, 0.5*mm, 1*mm, beampipe_angle, beampipe_spanning);
 	G4SubtractionSolid* col = new G4SubtractionSolid("col", col1, col2);
 	G4LogicalVolume* colLV = new G4LogicalVolume(col, Air, "colLV");
 	G4RotationMatrix* colrot = new G4RotationMatrix(0,0,0);
// // 	//colrot->rotateY(5*deg);
 	 new G4PVPlacement(colrot, G4ThreeVector(0,0,13.0*cm), colLV, "colPV", expHall_log, false, 0);
	
	// gafchromic film infront on the water tank
	G4ThreeVector gafPosition = G4ThreeVector(0,0,wt_posz+(7.0*um/2)-1*cm);	
	gafFilm[0] = new G4Box("gafFilmSensitive", gafFilmSize[0]/2, gafFilmSize[1]/2, 7.0*um/2);
	gafFilm_log[0] = new G4LogicalVolume(gafFilm[0], gafFilm_material, "gafFilmSensitive_log");
	gafFilm_phys[0] = new G4PVPlacement(0, gafPosition, gafFilm_log[0], "gafFilmSensitive_phys", expHall_log, false, 0);
	
	gafPosition = G4ThreeVector(0,0,wt_posz+7.0*um+(100*um/2)-1*cm);	// add required z distance to place the film correctly
	gafFilm[1] = new G4Box("gafFilmBacking", gafFilmSize[0]/2, gafFilmSize[1]/2, 100*um/2);
	gafFilm_log[1] = new G4LogicalVolume(gafFilm[1], Perspex, "gafFilmBacking_log");
	gafFilm_phys[1] = new G4PVPlacement(0, gafPosition, gafFilm_log[1], "gafFilmBacking_phys", expHall_log, false, 0);
	
	// add the 1.3mm perspex front to the water tank
	G4ThreeVector wtw_pos = G4ThreeVector(0,0,wt_posz-wt_window_hz);
	G4Box* watertank_window = new G4Box("watertank_window", absorber_hx, absorber_hy, wt_window_hz);
	G4LogicalVolume* wtw_log = new G4LogicalVolume(watertank_window, Perspex, "wtw_log");
	G4PVPlacement* wtw_phys = new G4PVPlacement(0, wtw_pos, wtw_log, "wtw_phys", expHall_log, false, 0);
		
	// water region of the water tank
	watertank = new G4Box("watertank", absorber_hx, absorber_hy, wt_depth_hz);
	watertank_log = new G4LogicalVolume(watertank, Water, "watertank_log");
	
	G4double uStepMin = 10*um;
  G4double uTrakMin = 10*um;
  G4UserLimits* stepLimit = new G4UserLimits(uStepMin,uTrakMin);
  watertank_log->SetUserLimits(stepLimit);
	
	if(watertankRegion) delete watertankRegion;
	watertankRegion = new G4Region("watertankRegion");
	watertankRegion->AddRootLogicalVolume(watertank_log);
	watertankRegion->SetProductionCuts(new G4ProductionCuts());
 	watertankRegion->GetProductionCuts()->SetProductionCut(0.001*mm, "gamma");
 	watertankRegion->GetProductionCuts()->SetProductionCut(0.001*mm, "e+");
 	watertankRegion->GetProductionCuts()->SetProductionCut(0.001*mm, "e-");
	watertank_log->SetRegion(watertankRegion);
	//G4cout << "### det watertankRegion gamma production cut = " << G4BestUnit(watertankRegion->GetProductionCuts()->GetProductionCut("gamma"), "Length") << G4endl;
	G4ThreeVector wt_pos = G4ThreeVector(0,0,wt_posz+wt_depth_hz);
	watertank_phys = new G4PVPlacement(0, wt_pos, watertank_log, "watertank_phys", expHall_log, false, 0);
	
	G4Box* sensor = new G4Box("sensor", 2*cm, 2*cm, 100*um/2);
	G4LogicalVolume* sensorLV = new G4LogicalVolume(sensor, Si, "sensorLV");
	G4double sensorz = silicon_pos-(wt_window_hz*2)+(100*um/2)-wt_depth_hz; // position inside the mother volume
	 new G4PVPlacement(0, G4ThreeVector(0,0,sensorz), sensorLV, "sensorPV", watertank_log, false, 0);
	
	G4Box* parylene = new G4Box("parylene", 2*cm, 2*cm, 25*um);
	G4LogicalVolume* paryleneLV = new G4LogicalVolume(parylene, Parylene, "paryleneLV");
	 new G4PVPlacement(0, G4ThreeVector(0,0,sensorz-50*um-25*um), paryleneLV, "parylenePV", watertank_log, false, 0);
	
	G4cout << "Silicon Position = " << G4BestUnit(silicon_pos, "Length") << G4endl;
	G4RunManager::GetRunManager() -> GeometryHasBeenModified();
}

G4double DetectorConstruction::GetWaterTankPosition()
{
	return wt_posz+(wt_window_hz*2)+wt_depth_hz;
}

G4double DetectorConstruction::GetWaterTankDepth()
{
	return wt_depth_hz*2;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructScatteringSystem()
{

	G4cout << "DetectorConstruction::ConstructScatteringSystem()" << G4endl;

	// this will give the brass collimator
	G4double scattCol_hz = 5*mm;
	scattererCollimator = new G4Tubs("scattererCollimator_whole", 0.5*cm, 3*cm, scattCol_hz, 0, CLHEP::twopi);	
	
	// need four logical volumes for the fours brass collimators
	// naming convention is simply _XY X=unit of system Y=col on unit
	G4LogicalVolume* scattCol_11 = new G4LogicalVolume(scattererCollimator, Cu, "scattCol_11");
	G4LogicalVolume* scattCol_12 = new G4LogicalVolume(scattererCollimator, Cu, "scattCol_12");
	G4LogicalVolume* scattCol_21 = new G4LogicalVolume(scattererCollimator, Cu, "scattCol_21");
	G4LogicalVolume* scattCol_22 = new G4LogicalVolume(scattererCollimator, Cu, "scattCol_22");
	
	// the scatterer volumes
	firstScatterer = new G4Tubs("firstScatterer", 0.0*mm, 20*mm, firstScattererThickness/2, 0, CLHEP::twopi);
	firstScatterer_log = new G4LogicalVolume(firstScatterer, scattering_material, "firstScatterer_log");
	
	// get the center of the vacuum and translate the required world position into vacuum coordinates
	G4ThreeVector vacuumCenter = G4PhysicalVolumeStore::GetInstance()->GetVolume("vacuum")->GetTranslation();
	G4ThreeVector vacuumPos = firstScattererPosition - vacuumCenter;
	G4cout << "vacuumPos = " << vacuumCenter << G4endl;	
	
	//translate the position of the collimator infront of the scatterer
	G4ThreeVector scattCol_22_pos = G4ThreeVector(vacuumPos.x(), 
																								vacuumPos.y(), 
																								vacuumPos.z()-scattCol_hz-firstScattererThickness/2);
	
	G4ThreeVector scattCol_21_pos = G4ThreeVector(vacuumPos.x(), 
																								vacuumPos.y(), 
																								vacuumPos.z()-scattCol_hz-(firstScattererThickness/2)-2*cm);
	
	
	G4ThreeVector scattCol_12_pos = G4ThreeVector(vacuumPos.x(), 
																								vacuumPos.y(), 
																								vacuumPos.z()-scattCol_hz-(firstScattererThickness/2)-2*cm-15*cm);
	
	G4ThreeVector scattCol_11_pos = G4ThreeVector(vacuumPos.x(), 
																								vacuumPos.y(), 
																								vacuumPos.z()-scattCol_hz-(firstScattererThickness/2)-2*cm-15*cm-2*cm);
	
	// finally place the volumes
	new G4PVPlacement(0, scattCol_11_pos, scattCol_11, "scattCol_11", vac_log, false, 0);
	new G4PVPlacement(0, scattCol_12_pos, scattCol_12, "scattCol_12", vac_log, false, 0);
	new G4PVPlacement(0, scattCol_21_pos, scattCol_21, "scattCol_21", vac_log, false, 0);
	new G4PVPlacement(0, scattCol_22_pos, scattCol_22, "scattCol_22", vac_log, false, 0);
	new G4PVPlacement(0, vacuumPos, firstScatterer_log, "firstScatterer_phys", vac_log, false, 0);
	
	
}

void DetectorConstruction::ConstructGaFChromicFilm()
{
		// enable the film to be reversed jsut incase this happens by accident taking measurements
	if(filmType == "HD-810")
	{
		if(!reverseFilm)
		{
			gafFilm[0] = new G4Box("gafFilmSensitive", gafFilmSize[0]/2, gafFilmSize[1]/2, 7.0*um/2);
			gafFilm_log[0] = new G4LogicalVolume(gafFilm[0], gafFilm_material, "gafFilmSensitive_log");
			gafFilm_phys[0] = new G4PVPlacement(0, gafFilmPosition, gafFilm_log[0], "gafFilmSensitive_phys", expHall_log, false, 0);
			
			gafFilm[1] = new G4Box("gafFilmBacking", gafFilmSize[0]/2, gafFilmSize[1]/2, 100*um/2);
			gafFilm_log[1] = new G4LogicalVolume(gafFilm[1], PET, "gafFilmBacking_log");
			gafFilmPosition = G4ThreeVector(gafFilmPosition[0], gafFilmPosition[1], gafFilmPosition[2]+53.5*um);
			gafFilm_phys[1] = new G4PVPlacement(0, gafFilmPosition, gafFilm_log[1], "gafFilmBacking_phys", expHall_log, false, 0);
		}
		else
		{
			gafFilm[1] = new G4Box("gafFilmBacking", gafFilmSize[0]/2, gafFilmSize[1]/2, 100*um/2);
			gafFilm_log[1] = new G4LogicalVolume(gafFilm[1], PET, "gafFilmBacking_log");
			gafFilm_phys[1] = new G4PVPlacement(0, gafFilmPosition, gafFilm_log[1], "gafFilmBacking_phys", expHall_log, false, 0);
			
			gafFilm[0] = new G4Box("gafFilmSensitive", gafFilmSize[0]/2, gafFilmSize[1]/2, 7.0*um/2);
			gafFilm_log[0] = new G4LogicalVolume(gafFilm[0], gafFilm_material, "gafFilmSensitive_log");
			gafFilmPosition = G4ThreeVector(gafFilmPosition[0], gafFilmPosition[1], gafFilmPosition[2]+53.5*um);
			gafFilm_phys[0] = new G4PVPlacement(0, gafFilmPosition, gafFilm_log[0], "gafFilmSensitive_phys", expHall_log, false, 0);
		}
	}
	else if(filmType == "EBT3")
	{
		/// Definition of EBT3
		gafFilm[0] = new G4Box("gafFilmSensitive", gafFilmSize[0]/2, gafFilmSize[1]/2, 27*um/2);
		gafFilm_log[0] = new G4LogicalVolume(gafFilm[0], LiPCD, "gafFilmSensitive_log");
		new G4PVPlacement(0, gafFilmPosition, gafFilm_log[0], "gafFilmSensitive_phys", expHall_log, false, 0);
			
		gafFilm[1] = new G4Box("gafFilmBacking", gafFilmSize[0]/2, gafFilmSize[1]/2, 130*um/2);
		gafFilm_log[1] = new G4LogicalVolume(gafFilm[1], PET, "gafFilmBacking_log");
		G4ThreeVector gafFilmPosition1 = G4ThreeVector(gafFilmPosition[0], gafFilmPosition[1], gafFilmPosition[2]-78.5*um);
		new G4PVPlacement(0, gafFilmPosition1, gafFilm_log[1], "gafFilmFront_phys", expHall_log, false, 0);
		G4ThreeVector gafFilmPosition2 = G4ThreeVector(gafFilmPosition[0], gafFilmPosition[1], gafFilmPosition[2]+78.5*um);
		new G4PVPlacement(0, gafFilmPosition2, gafFilm_log[1], "gafFilmBacking_phys", expHall_log, false, 0);
	}
	
	/// This is to stop all the protons after the film due to not wanting to propogate proton to
	/// the end of the world volume through the air in an attempt to speed up the simulation
	G4VSolid* scatterer_mop = gafFilm[0];
	G4LogicalVolume* scatterer_mop_log = new G4LogicalVolume(scatterer_mop, Perspex, "scatterer_mop");
	G4ThreeVector mop_pos = gafFilmPosition;
	mop_pos.setZ(mop_pos.getZ()+5*cm);
	new G4PVPlacement(0,mop_pos, scatterer_mop_log, "scatterer_mop", expHall_log, false, 0);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::UpdateGeometry()
{
  //G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  // Inform the kernel about the new geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
	//absorber->SetZHalfLength(absorber_hz);
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
     G4RunManager::GetRunManager() -> GeometryHasBeenModified();
   G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
}

void DetectorConstruction::ConstructValidation()
{
	validation_box = new G4Box("validation_box", validation_hx, validation_hy, validation_hz);
	validation_log[0] = new G4LogicalVolume(validation_box, validation_material, "validation_log0");
	validation_phys[0] = new G4PVPlacement(0, validation_pos[0], validation_log[0], "validation_phys0", expHall_log, false, 0);
	validation_log[1] = new G4LogicalVolume(validation_box, validation_material, "validation_log1");
	validation_phys[1] = new G4PVPlacement(0, validation_pos[1], validation_log[1], "validation_phys1", expHall_log, false, 0);
	validation_log[2] = new G4LogicalVolume(validation_box, validation_material, "validation_log2");
	validation_phys[2] = new G4PVPlacement(0, validation_pos[2], validation_log[2], "validation_phys2", expHall_log, false, 0);
	validation_log[3] = new G4LogicalVolume(validation_box, validation_material, "validation_log3");
	validation_phys[3] = new G4PVPlacement(0, validation_pos[3], validation_log[3], "validation_phys3", expHall_log, false, 0);
}

