#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VSolid;

#include "sstream"

#include "G4VUserDetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SDManager.hh"
#include "G4Region.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();
    void							 ConstructBeamline();
    G4VSolid*          ConstructBeampipeVacuum();
    void							 ConstructJig();
    void			         ConstructDynamiteSensor();
		void								ConstructDoubleDynamite();
		void								ConstructPRaVDATable();
    void               DefineMaterials();
    void               SetAbsDepth(G4double depth);
    void               SetAbsMaterial(G4String material);
    void							 SetCollimatorInnerRadius(G4double);
    G4double					 GetCollimatorInnerRadius();
		
    void							 SetDynamiteConstruction(bool);
    bool							 GetDynamiteConstruction();
		void							 SetDynamiteBackingDepth(G4double);
		G4double					 GetDynamiteBackingDepth();
		void							 SetDynamitePosition(G4double, G4double, G4double);
		G4ThreeVector     GetDynamitePosition();
		void							 SetDoubleDynamiteConstruction(bool);
		bool							 GetDoubleDynamiteConstruction();
		void							 SetDoubleDynamiteSeperation(G4double);
		G4double					 GetDoubleDynamiteSeperation();
		void 							 SetDoubleDynamiteCollimatorConstruction(bool);
		void              SetDoubleDynamiteCollimatorPosition(G4ThreeVector);
		void							 SetDoubleDynamiteCollimatorRotation(G4ThreeVector);
		void              SetDoubleDynamiteCollimatorMaterial(G4String);
		void              SetDoubleDynamiteCollimatorThickness(G4double);
		void              SetDoubleDynamiteCollimatorRadius(G4double);

		
    void							 SetTrackerConstruction(bool);
    bool							 GetTrackerConstruction();
		void								SetNTrackers(G4int);
		G4int								GetNTrackers();
    void							 ConstructTracker();
		
		void								SetRangeTelescopeConstruction(bool);
		bool								GetRangeTelescopeConstruction();
		void								ConstructRangeTelescope();
		void								SetNRangeTelescopeLayers(G4double);
		G4double						GetNRangeTelescopeLayers();
		void 							  SetRangeTelescopeLayersSeperation(G4double);
		void								SetRangeTelescopeAbsorberThickness(G4double);
		void								SetRangeTelescopeAbsorberMaterial(G4String);
		
		void								SetWaterTankConstruction(bool);
		bool								GetWaterTankConstruction();
		void								SetWaterTankWindowThickness(G4double);
		G4double						GetWaterTankWindowThickness();
		void								ConstructWaterTank();
		void               SetWaterTankSensorPosition(G4double p) { silicon_pos = p;G4cout << silicon_pos << "####" << G4endl; };
		
		void								ConstructScintillatorStack();
		
		void SetScatteringSystemConstruction(bool construct) { constructScatteringSystem = construct; };
		bool GetScatteringSystemConstruction() { return constructScatteringSystem; };
		void SetScattererCollimatorRadius(G4double r) { scattererCollimator_radius = r; };
		void SetFirstScattererSize(G4ThreeVector size) { firstScattererSize = size; };
		void SetFirstScattererThickness(G4double t) { firstScattererThickness = t; };
		void SetFirstScattererPosition(G4ThreeVector pos) { firstScattererPosition = pos; };
		void SetScatteringMaterial(G4String);
		void SetScattererInsideVacuum(G4bool b) { scattererInVacuum = b; };
		void ConstructScatteringSystem();
		void ConstructBeamMonitor(G4double z);
		
		void SetGaFChromicFilmSize(G4ThreeVector size) { gafFilmSize = size; };
		void SetGaFChromicFilmPosition(G4ThreeVector pos) { gafFilmPosition = pos; };
		void SetGaFChromicFilmType(G4String type) { filmType = type; };
		G4String GetGaFChromicFilmType() { return filmType; };
		void ConstructGaFChromicFilm();
		
		void               ConstructValidation();
		
    void               UpdateGeometry();
		
		void               SetDefaultColours();
		
    G4double           GetAbsorberDepth();
    G4double           GetAbsorberPosition();
    G4double           GetBeamPipeInnerRadius();    
    G4double           GetDynamiteZPosition();
    G4double           GetDynamiteDepth();
		G4double						GetWaterTankPosition();
		G4double						GetWaterTankDepth();
    

  private:
  
  	G4VSolid* expHall_box;
    G4VSolid* beampipe_tube;
    G4VSolid* alcollimator_tube;
    G4VSolid* havarwindow_sphere;
    G4VSolid* havarwindow_actual;
    G4VSolid* beampipe_vac;
    G4VSolid* chamber_alring;
    G4VSolid* chamber_airgap;
    G4VSolid* chamber_window;
    G4VSolid* jig_frame;
    G4Box* absorber;
    G4VSolid* dynamite_Si3N4;
    G4VSolid* dynamite_SiO2;
    G4VSolid* dynamite_epi;
    G4VSolid* dynamite_sub;
    G4VSolid* dynamite_back;
		G4VSolid* dd_collimator;
		
		G4VSolid* rt_epi;
		G4VSolid* rt_epi_pcb;
    G4VSolid* rt_sub;
		G4VSolid* rt_absorber;
    
    // Logical volumes
    //
    G4LogicalVolume* expHall_log;
    G4LogicalVolume* beampipe_log;
		G4LogicalVolume* vac_log;
    G4LogicalVolume* alcollimator_log;
    G4LogicalVolume* havarwindow_log;
    G4LogicalVolume* beampipe_vac_log;
    G4LogicalVolume* chamber_alring_log;
    G4LogicalVolume* chamber_airgap_log;
    G4LogicalVolume* chamber_window_log;
    G4LogicalVolume* absorber_log;
    G4LogicalVolume* jig_log;
    G4LogicalVolume* dynamite_Si3N4_log[2];
    G4LogicalVolume* dynamite_SiO2_log[2];
    G4LogicalVolume* dynamite_epi_log[2];
    G4LogicalVolume* dynamite_sub_log[2];
    G4LogicalVolume* dynamite_back_log[2]; // 5mm Al backing
    G4LogicalVolume* dd_collimator_log;
    
    G4LogicalVolume* rt_epi_log[30];
		G4LogicalVolume* rt_epi_pcb_log[30];
    G4LogicalVolume* rt_sub_log[30];
		G4LogicalVolume* rt_absorber_log[30];


    // Physical volumes
    //
    G4VPhysicalVolume* expHall_phys;
    G4VPhysicalVolume* beampipe_phys;
    G4VPhysicalVolume* alcollimator_phys;
    G4VPhysicalVolume* havarwindow_phys;
    G4VPhysicalVolume* beampipe_vac_phys;
    G4VPhysicalVolume* chamber_alring_phys;
    G4VPhysicalVolume* chamber_airgap_phys;
    G4VPhysicalVolume* chamber_window_phys1;
    G4VPhysicalVolume* chamber_window_phys2;
    G4VPhysicalVolume* absorber_phys;
    G4VPhysicalVolume* jig_phys;
    G4VPhysicalVolume* dynamite_Si3N4_phys[2];
    G4VPhysicalVolume* dynamite_SiO2_phys[2];
    G4VPhysicalVolume* dynamite_epi_phys[2];
    G4VPhysicalVolume* dynamite_sub_phys[2];
    G4VPhysicalVolume* dynamite_back_phys[2];
		G4VPhysicalVolume* dd_collimator_phys;
		
		G4VPhysicalVolume* rt_epi_phys[30];
		G4VPhysicalVolume* rt_epi_pcb_phys[30];
    G4VPhysicalVolume* rt_sub_phys[30];
		G4VPhysicalVolume* rt_absorber_phys[30];
		
		
		G4VisAttributes* expHall_vis;
		G4VisAttributes* beampipe_vis;
		G4VisAttributes* beampipe_vac_vis;
		G4VisAttributes* alcollimator_vis;
		G4VisAttributes* havarwindow_vis;
		G4VisAttributes* chamber_alring_vis;
		G4VisAttributes* chamber_airgap_vis;
		G4VisAttributes* chamber_window_vis;
		G4VisAttributes* absorber_vis;
		G4VisAttributes* jig_vis;
		G4VisAttributes* dynamite_Si3N4_vis;
		G4VisAttributes* dynamite_SiO2_vis;
		G4VisAttributes* dynamite_epi_vis;
		G4VisAttributes* dynamite_sub_vis;
		G4VisAttributes* dynamite_back_vis;
		G4VisAttributes* tracker_vis;
		G4VisAttributes* tracker_pcb_vis;
		
    G4Region* absorberRegion;
		G4Region* watertankRegion;
    
    DetectorConstructionMessenger* detectorConstructionMessenger;
    
    G4double jig_outer_hx ;
  G4double jig_outer_hy ;
  G4double jig_outer_hz ;
  G4double jig_hwallthickness;
  G4double jig_airgap_hx ;
  G4double jig_airgap_hy;
  G4double jig_airgap_hz;
  G4double jig_posz;
  
  // the beampipe
	G4double beampipe_outerradius;
	G4double beampipe_thickness ;
	G4double beampipe_innerradius;
	G4double beampipe_hz ;
	G4double beampipe_angle;
  G4double beampipe_spanning ;
 
	
  // the Al collimator within the end of the beampipe
	G4double alcollimator_outerradius; // sits snuggly inside the beampipe
	G4double alcollimator_innerradius;
	G4double alcollimator_hz;
	G4double alcollimator_angle;
	G4double alcollimator_spanning;
	
	// the Havar window at end of beampipe
	G4double havarwindow_thickness;
  G4double havarwindow_outerradius;
  G4double havarwindow_innerradius ;
  G4double havarwindow_startingPhi;
  G4double havarwindow_widthPhi;
  G4double havarwindow_startingEta;
  G4double havarwindow_widthEta;
  
  // Proton Monitor Chamber
  G4double chamberposition_z ;
  G4double chamber_startingangle;
  G4double chamber_spanningangle;
  G4double chamber_alring_outerradius;
  G4double chamber_alring_innerradius;
  G4double chamber_alring_hz;
  G4double chamber_airgap_outerradius;
  G4double chamber_airgap_innerradius;
  G4double chamber_airgap_hz;
  G4double chamber_window_outerradius;
  G4double chamber_window_innerradius;
  G4double chamber_window_hz;
  
  // Absorber parameters
  G4double absorber_hx;
  G4double absorber_hy;
  G4double absorber_hz;
  G4double absorber_posz;      
   G4double absDepth;  
   G4Material* absorber_material; 
   
  // Dynamite sensor
  bool     constructDynamite;
	bool			constructDoubleDynamite;
	G4int			nDynamites;
  G4double dynamite_hx;
  G4double dynamite_hy;
  G4double dynamite_Si3N4_hz;
  G4double dynamite_SiO2_hz;
  G4double dynamite_epi_hz;
  G4double dynamite_sub_hz;
  G4double dynamite_back_hz;
	G4ThreeVector dynamite_pos;
// 	G4double dynamite_posx;
// 	G4double dynamite_posy;
//   G4double dynamite_posz;
  G4double dynamite_seperation_abs;
	G4double dynamite_double_seperation;
	
	G4double dd_collimator_radius;
	G4double dd_collimator_hz;
	G4Material* dd_collimator_material;
	G4ThreeVector dd_collimator_pos;
	G4RotationMatrix* dd_collimator_rotation;
	bool     constructDoubleDynamiteCollimator;
  
  // Tracking sensors
  bool			constructTracker;
  G4double tracker_sensors_hx;
  G4double tracker_sensors_hy;
  G4double tracker_sensors_hz;
  G4double tracker_sensors_n;
  G4double tracker_sensors_sepz[12];
  G4double tracker_sensors_posz[12];
  G4double tracker_seperation_abs;
  G4double tracker_first_sensor_z;
  G4VSolid* tracker;
    G4VSolid* tracker_pcb;
  G4LogicalVolume* tracker_log[12];
  G4LogicalVolume* tracker_pcb_log[12];
  
  G4VPhysicalVolume* tracker_phys[12];
  G4VPhysicalVolume* tracker_pcb_phys[12];
	
	// Range Telescope
	bool constructRangeTelescope;
	G4double nRangeTelescopeLayers;
	G4double rt_hx;
	G4double rt_hy;
	G4double rt_epi_hz;
	G4double rt_sub_hz;
	G4double rt_gap_hz;
	G4double rt_z;
	G4double rt_absorber_hz;
	G4Material* rt_absorber_material;
	
	// Water Tank
	
	bool	constructWaterTank;
	G4double wt_window_hz;
	G4double wt_depth_hz;
	G4double wt_posz;
	G4VSolid* watertank;
	G4LogicalVolume* watertank_log;
	G4VPhysicalVolume* watertank_phys;
	G4double silicon_pos;
	
	
	// Scattering system
	bool constructScatteringSystem;
	bool scattererInVacuum;
	bool reverseFilm;
	G4String filmType;
	G4double scattererCollimator_radius;
	G4VSolid* scattererCollimator;
	G4LogicalVolume* scattererCollimator_log;
	G4VPhysicalVolume* scattererCollimator_phys;
	G4ThreeVector firstScattererSize;
	G4double firstScattererThickness;
	G4ThreeVector firstScattererPosition;
	G4VSolid* firstScatterer;
	G4LogicalVolume* firstScatterer_log;
	G4VPhysicalVolume* firstScatterer_phys;
	G4Material* scattering_material;
	G4VSolid* gafFilm[2];
	G4LogicalVolume* gafFilm_log[2];
	G4VPhysicalVolume* gafFilm_phys[2];
	G4Material* gafFilm_material;
	G4Material* LiPCD;
	G4Material* PET;
	G4ThreeVector gafFilmPosition;
	G4ThreeVector gafFilmSize;
	
	
	G4double validation_hz;
	G4double validation_hx;
	G4double validation_hy;
	G4Material* validation_material;
	G4ThreeVector validation_pos[4];
	G4VSolid* validation_box;
	G4LogicalVolume* validation_log[4];
	G4VPhysicalVolume* validation_phys[4];
    
    // Materials
    //
    G4Material* Al;
    G4Material* Si;
    G4Material* Ta;
    G4Material* Cr;
    G4Material* Fe;
    G4Material* Ni;
    G4Material* O;
    G4Material* C;
    G4Material* H;
    G4Material* N;
    G4Material* Mn;
    G4Material* Mo;
    G4Material* W;
    G4Material* Co;
    G4Material* Be;
		G4Material* Cu;
		G4Material* Mg;
		G4Material* Ti;
    G4Material* Water;
    G4Material* Vacuum;
    G4Material* Mylar;
    G4Material* Air;
    G4Material* Steel;
    G4Material* Havar;
    G4Material* Kapton;
    G4Material* Perspex;
		G4Material* Dural;
    G4Material* Si3N4;
    G4Material* SiO2;
		G4Material* Parylene;
};

#endif
