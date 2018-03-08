#include "RunAction.hh"
#include "Run.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(DetectorConstruction* det, HistoManager* histo, PhysicsList* phys)
: detector(det), histoManager(histo), physics(phys)
{
	edeptot=0;
	eniel=0;
}

RunAction::~RunAction()
{}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
	
	histoManager->SetHisto("sensor_WT_edep","",100,0,10);
  
  // 1D Histograms
  // =============
  //
  // define histograms for the whole system
  histoManager->SetHisto("total.PDGID", "PDGID of all particles in all steps;PDGID;Count", 6000, -3000, 3000);
  histoManager->SetHisto("total.dEdx", "Energy deposited in whole system as function of z; z [mm]; Count",9000, -30*cm, 60*cm, "mm");
  histoManager->SetHisto("total.dEdx(NIEL)", "Energy deposited by NIEL in whole system as function of z; z [mm]; Count",9000, -30*cm, 60*cm, "mm");
	histoManager->SetHisto("total.processType", "Process type of every PostStepPoint; ProcessIndex; Count", 25, -0.5, 24.5);
  histoManager->SetHisto("total.EDep(PerEvent)", "Energy deposited in the whole system per event; Energy Deposited [MeV]; Count", 200, 0*MeV, 50*MeV, "MeV");
 	histoManager->SetHisto("total.NIEL(PerEvent)", "Energy deposited by NIEL in the absorber per event; Energy Deposited [MeV]; Count", 200, 0*MeV, 5*MeV, "MeV");
	histoManager->SetHisto("total.Energy", "Energy of all steps; Energy [MeV]; Count", 2000, 0*MeV, 50*MeV, "MeV");
	histoManager->SetHisto("total.Energy.proton", "Energy of all proton steps; Energy [MeV]; Count", 2000, 0*MeV, 50*MeV, "MeV");
	histoManager->SetHisto("total.Energy.neutron", "Energy of all neutron steps; Energy [MeV]; Count", 2000, 0*MeV, 50*MeV, "MeV");

    histoManager->SetHisto("total.Energy.gamma", "Energy of all gamma tracks; Energy [MeV]; Count", 200, 0*keV, 10*keV, "keV"); 
  
	histoManager->SetTree("generated","generated particle properties");
  
  histoManager->SetHisto("generated.X", "Profile plot of generated particle positions; x [mm]",
  												100, -25*mm, 25*mm,
  												"mm");	
	
  histoManager->SetHisto2D("generated.XY", "Scatter plot of generated particle positions; x [mm]; y [mm]",
  												100, -25*mm, 25*mm,
  												100, -25*mm, 25*mm,
  												"mm", "mm");
	
	histoManager->SetHisto2D("airmonitor.XY", "Scatter plot of particles into air monitor; x [mm]; y [mm]",
  												100, -25*mm, 25*mm,
  												100, -25*mm, 25*mm,
  												"mm", "mm");
	
	histoManager->SetTree("validation0","generated particle properties");
	histoManager->SetTree("validation1","generated particle properties");
	histoManager->SetTree("validation2","generated particle properties");
	histoManager->SetTree("validation3","generated particle properties");
  
	histoManager->SetHisto("TotalEdepInAirGap", "",2000, 0.*eV, 100000*eV, "eV");
	
	histoManager->SetHisto("BeamPipeInteractions_Zpos","",500,-5000*mm,0*mm,"mm");
	histoManager->SetHisto("BeamPipeInteractions_Zpos_pInAbsorber","",500,-5000*mm,0*mm,"mm");
	histoManager->SetHisto("BeamPipeInteractions_Xpos_pInAbsorber","",100,-50*mm,50*mm,"mm");
	histoManager->SetHisto("BeamPipeInteractions_Ypos_pInAbsorber","",100,-50*mm,50*mm,"mm");
	
  // define histograms for the absorber binning
  G4double length  = detector->GetAbsorberDepth();
  G4double stepMax = 10*um;//physics->GetStepMaxProcess()->GetMaxStep();
  G4int nbmin = 400;
  G4int nbBins = (G4int)(0.5 + length/stepMax);
  if (nbBins < nbmin) nbBins = nbmin;
  histoManager->SetHisto("absorber.dEdx", "Energy deposited in absorber as function of depth; Depth in Absorber [mm]",nbBins, 0., length, "mm");
  histoManager->SetHisto("absorber.dEdx.Marcus", "Energy deposited in absorber as function of depth; Depth in Absorber [mm]",nbBins, 0., length, "mm");
	histoManager->SetHisto("absorber.dEdx(NIEL)", "Energy deposited by NIEL in absorber as function of depth; Depth in Absorber [mm]",nbBins, 0., length, "mm");
  histoManager->SetHisto("absorber.incident.angleX", "Incident angle of particles into absorber (x co-ord); Angle [degrees]", 100, -5, 5);
  histoManager->SetHisto("absorber.incident.angleY", "Incident angle of particles into absorber (y co-ord); Angle [degrees]", 100, -5, 5);
	histoManager->SetHisto("absorber.incident.Energy", "Energy of the incident particles into the absorber; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
  histoManager->SetHisto("absorber.exit.angleX", "Incident angle of particles into absorber (x co-ord); Angle [degrees]", 100, -5, 5);
  histoManager->SetHisto("absorber.exit.angleY", "Incident angle of particles into absorber (y co-ord); Angle [degrees]", 100, -5, 5);	
 	histoManager->SetHisto("absorber.exit.Energy", "Energy of the exiting particles from the absorber; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
	histoManager->SetHisto("absorber.exit.Energy.proton", "Energy of the exiting protons from the absorber; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
	histoManager->SetHisto("absorber.exit.PDGID", "PDGID of the exiting particles from the absorber; PDGID; Count", 6000, -3000, 3000);
 	histoManager->SetHisto("absorber.EDep(PerEvent)", "Energy deposited in the absorber per event; Energy Depostied [MeV]", 200, 0*MeV, 50*MeV, "MeV");
 	histoManager->SetHisto("absorber.NIEL(PerEvent)", "Energy deposited by NIEL in the absorber per event; Energy Depostied [MeV]", 200, 0*MeV, 5*MeV, "MeV");
 	histoManager->SetHisto2D("absorber.incident.XY", "Scatter plot of all incident particles into absorber; x [cm]; y [cm]", 
 													400, -5*cm, 5*cm,
  												400, -5*cm, 5*cm,
  												"cm", "cm"); 
 	histoManager->SetHisto2D("absorber.incident.LowE.XY", "Scatter plot of all incident particles into absorber; x [cm]; y [cm]", 
 													400, -5*cm, 5*cm,
  												400, -5*cm, 5*cm,
  												"cm", "cm");	
 	histoManager->SetHisto2D("absorber.incident.Energy.XY", "Scatter plot of all incident particles energy into absorber; x [cm]; y [cm]", 
 													400, -5*cm, 5*cm,
  												400, -5*cm, 5*cm,
  												"cm", "cm"); 	
  histoManager->SetHisto2D("absorber.exit.XY", "Scatter plot of all particles exiting absorber into absorber; x [cm]; y [cm]", 
 													400, -5*cm, 5*cm,
  												400, -5*cm, 5*cm,
  												"cm", "cm");	
	
	histoManager->SetProfile("absorber.Ex", "Energy of the particles as a function of depth; Depth in Absorber [mm]", nbBins, 0, length, "mm");
	histoManager->SetProfile("absorber.Ex.proton", "Energy of the particles as a function of depth (just protons); Depth in Absorber [mm]", nbBins, 0, length, "mm");
	histoManager->SetProfile("absorber.Ex.Marcus", "Energy of the particles as a function of depth; Depth in Absorber [mm]", nbBins, 0, length, "mm");
	histoManager->SetProfile("absorber.Ex.Marcus.proton", "Energy of the particles as a function of depth (just protons); Depth in Absorber [mm]", nbBins, 0, length, "mm");
	
	
	histoManager->SetTree("absorber.incident", "Properties of incident particles into the absorber");
	histoManager->SetTree("absorber.exit", "Properties of the particles leaving the absorber");
	
//	histoManager->SetTree("braggpeak", "braggpeak");
	
	if(detector->GetScatteringSystemConstruction())
	{
		histoManager->SetTree("firstScatterer", "Properties of steps in first scatterer");
		histoManager->SetTree("firstScatterer.incident", "Properties of steps incident on first scatterer");
		histoManager->SetTree("firstScatterer.exit", "Properties of steps exiting from first scatterer");
		histoManager->SetTree("GaFChromicFilm", "Properties of steps in GaF Chromic Film");
	}
	histoManager->SetTree("GaFChromicFilm", "Properties of steps in GaF Chromic Film");
  		
	if(detector->GetWaterTankConstruction())
	{
		G4double depth = detector->GetWaterTankDepth();
		histoManager->SetHisto("watertank.indicent", "Energy of incident particles into the water itself", 400,0,40);
		histoManager->SetHisto("watertank_dEdx", "Energy deposition as a function of depth in the water tank", nbBins, -depth, depth, "mm");
		histoManager->SetHisto("watertank.dEdx.proton", "Energy deposition as a function of depth in the water tank by protons", nbBins, 0., depth, "mm");
		histoManager->SetProfile("watertank.Ex", "Energy of the particles as a function of depth; Depth in Water Tank [mm]", nbBins, 0., depth, "mm");
		histoManager->SetProfile("watertank.Ex.proton", "Energy of the protons as a function of depth; Depth in Water Tank [mm]", nbBins, 0., depth, "mm");
	}
 	
 
  histoManager->book();
}
/*
G4Run* RunAction::GenerateRun()
{
	return new Run("absorber_mfd");
}*/

void RunAction::EndOfRunAction(const G4Run* aRun)
{ 
	G4cout << " Total energy deposit= "<< G4BestUnit(edeptot,"Energy")
         << G4endl;
  G4cout << " niel energy deposit = "<< G4BestUnit(eniel,"Energy")
         << G4endl;

	//
	// Dump result for all HitsMap collections.
	//
/*	Run* theRun = (Run*)aRun;
	theRun->DumpData();
*/
  // save and clean histo
  histoManager->save();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// count up the total energy in the run deposited in sensitive volume
void RunAction::FillEdep(G4double de, G4double eni) 
{
	edeptot += de; 
	eniel += eni;
}
