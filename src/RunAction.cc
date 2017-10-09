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
 	
 	// define histograms for the dynamite setup
 	if(detector->GetDynamiteConstruction())
 	{
		histoManager->SetTree("dynamite.incident", "Properties of the particles into the dynamite epi layer");
		histoManager->SetTree("dynamite.epi", "Properties of the particles in the dynamite epi layer");
		
	 	histoManager->SetHisto("dynamite.dEdx", "Energy Deposition in dynaMITE as function of z; Depth in dynAMITe [mm]", 401, -0.01*mm, 1.*mm, "mm");
		histoManager->SetHisto("dynamite.dEdx.full", "Energy Deposition in dynaMITE as function of z (including back); Depth in dynAMITe [mm]", 2401, -0.01*mm, 6.*mm, "mm");
		histoManager->SetHisto("dynamite.dEdx.proton", "Energy Deposition in dynaMITE as function of z (just protons); Depth in dynAMITe [mm]", 401, -0.01*mm, 1.*mm, "mm");
		histoManager->SetHisto("dynamite.dEdx.proton.E>1MeV", "Energy Deposition in dynaMITE as function of z (just protons, E>1MeV); Depth in dynAMITe [mm]", 401, -0.01*mm, 1.*mm, "mm");
 		histoManager->SetHisto("dynamite.dEdx.epi(zoomed)", "Energy Deposition in dynaMITE epi layer as function of z; Depth in dynAMITe epi [mm]", 200, -0.01*um, 20.*um, "um");
		histoManager->SetHisto("dynamite.dEdx(NIEL)", "Energy Deposition by NIEL in dynaMITE as function of z; Depth in dynAMITe [mm]", 401, -0.01*mm, 1.*mm, "mm"); 	
 		histoManager->SetHisto("dynamite.incident.Energy", "Energy of the incident particles on the sensor; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("dynamite.incident.Energy.proton", "Energy of the incident protons on the sensor; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("dynamite.incident.Energy.neutron", "Energy of the incident neutrons on the sensor; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("dynamite.incident.Energy.photon", "Energy of the incident photons on the sensor; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("dynamite.incident.Energy.electron", "Energy of the incident electrons on the sensor; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("dynamite.incident.angleX", "Incident angle of particles into dynamite (x co-ord); Angle [degrees]", 100, -5, 5);
  	histoManager->SetHisto("dynamite.incident.angleY", "Incident angle of particles into dynamite (y co-ord); Angle [degrees]", 100, -5, 5);
		histoManager->SetHisto("dynamite.incident.PDGID", "PDGID of the incident particles into dynamite; PDGID", 6000, -3000, 3000);
		histoManager->SetHisto("dynamite.incident.nParticles", "The number of particles incident on the dynamite face (per Event); nParticles", 21, -0.5, 20.5);
		histoManager->SetHisto("dynamite.processType", "Process type of every PostStepPoint in dynamite; ProcessType", 25, -0.5, 24.5);
		histoManager->SetHisto("dynamite.processType.noAl", "Process type of every PostStepPoint in dynamite minus Al back; ProcessType", 25, -0.5, 24.5);
 		histoManager->SetHisto("dynamite.processType.epi", "Process type of every PostStepPoint in dynamite epitaxial layer; ProcessType", 25, -0.5, 24.5);
		histoManager->SetHisto("dynamite.EDep(PerEvent)", "Energy deposited in the dynamite per event; Energy Deposited [MeV]", 200, 0*MeV, 50*MeV, "MeV");
 		histoManager->SetHisto("dynamite.EDep(PerEvent).noAl", "Energy deposited in the dynamite (no Al) per event; Energy Deposited [MeV]", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("dynamite.EDep(PerEvent).epi", "Energy deposited in the dynamite epi per event; Energy Deposited [keV]", 200, 0*keV, 200*keV, "keV");
		histoManager->SetHisto("dynamite.NIEL(PerEvent)", "Energy deposited by NIEL in the absorber per event; Energy Deposited [MeV]", 200, 0*MeV, 5*MeV, "MeV");
 		histoManager->SetHisto2D("dynamite.incident.XY", "Scatter plot of incident particles into the dynamite sensor; x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												400, -20*cm, 20*cm,
  												"cm", "cm");
		histoManager->SetHisto2D("dynamite.incident.XY.proton", "Scatter plot of incident protons into the dynamite sensor; x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												400, -20*cm, 20*cm,
  												"cm", "cm");
		histoManager->SetHisto2D("dynamite.incident.XY.neutron", "Scatter plot of incident neutrons into the dynamite sensor; x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												400, -20*cm, 20*cm,
  												"cm", "cm");
  												
  	histoManager->SetHisto("dynamite.ratio.nAbsorbertodynamite", "Ratio between number of particles into sensor and out of absorber; dynAMITe in / absorber out", 100, 0.0, 5.0);
 		histoManager->SetHisto2D("dynamite.correlation.nAbsorbertodynamite", "Correlation between number of particles into sensor and out of absorber; dynAMITe in; absorber out", 6, -0.5, 5.5, 6, -0.5, 5.5); 
 	}
 	
 	if(detector->GetDoubleDynamiteConstruction())
	{
		histoManager->SetTree("doubledynamite.first.incident", "Properties of the particles into the first dynamite sensor");
		histoManager->SetTree("doubledynamite.first.exit", "Properties of the particles out of the first dynamite");
		histoManager->SetTree("doubledynamite.second.incident", "Properties of the particles into the second dynamite sensor");
		histoManager->SetTree("doubledynamite.second.exit", "Properties of the particles out of the second dynamite sensor");
		
		histoManager->SetHisto("doubledynamite.dEdx", "Energy deposition as a function of Z in the double dynamite system", 1001, 15*cm, 30*cm, "cm");
		histoManager->SetHisto("doubledynamite.incident.Energy.epi.first", "incident energy in to the first dynamite sensor", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("doubledynamite.exit.Energy.first", "exit energy from the first dynamite sensor", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("doubledynamite.incident.Energy.epi.second", "exit energy from the first dynamite sensor", 200, 0*MeV, 50*MeV, "MeV");
		histoManager->SetHisto("doubledynamite.incident.Energy.oxide.second", "exit energy from the first dynamite sensor", 200, 0*MeV, 50*MeV, "MeV");
		//histoManager->SetHisto("doubledynamite.scatterAngle", "Scatter angle of proton through first dynamite; arccos(a.b/mod(a).mod(b))", 3600, 0, 180);
		//histoManager->SetHisto("doubledynamite.scatterAngle.X", "Scatter angle of proton through first dynamite; angleX_{1} - angleX_{2}", 3600, -180, 180);
		//histoManager->SetHisto("doubledynamite.scatterAngle.Y", "Scatter angle of proton through first dynamite; angleY_{1} - angleY_{2}", 3600, -180, 180);
	
		
		histoManager->SetHisto2D("doubledynamite.first.incident.XY", "Scatter of particles into first dynamite", 200, -5*cm, 5*cm, 200, -5*cm, 5*cm, "cm", "cm");
		histoManager->SetHisto2D("doubledynamite.second.incident.XY", "Scatter of particles into second dynamite", 200, -5*cm, 5*cm, 200, -5*cm, 5*cm, "cm", "cm");
		
		histoManager->SetHisto("doubledynamite.Displacement.X", "Actual hit in dynamite2 - Actual hit in dynamite1 (X)", 200, -5*mm, 5*mm, "mm");
		histoManager->SetHisto("doubledynamite.Displacement.Y", "Actual hit in dynamite2 - Actual hit in dynamite1 (Y)", 200, -5*mm, 5*mm, "mm");
		histoManager->SetHisto("doubledynamite.Displacement.R", "Actual hit in dynamite2 - Actual hit in dynamite1 (Y)", 200, 0*mm, 10*mm, "mm");
		
		histoManager->SetHisto("doubledynamite.Deflection.X", "Actual hit in dynamite2 - projection from dynamite1 (X)", 200, -5*mm, 5*mm, "mm");
		histoManager->SetHisto("doubledynamite.Deflection.Y", "Actual hit in dynamite2 - projection from dynamite1 (Y)", 200, -5*mm, 5*mm, "mm");
		
		histoManager->SetHisto("doubledynamite.EnergyDepositedInEpi0", "Energy deposited per event in Epi layer of dynamite 1", 100, 0*keV, 200*keV, "keV");
		histoManager->SetHisto("doubledynamite.EnergyDepositedInEpi0>3.2keV", "Energy deposited per event in Epi layer of dynamite 1",100, 0*keV, 200*keV, "keV" );
		histoManager->SetHisto("doubledynamite.EnergyDepositedInEpi1", "Energy deposited per event in Epi layer of dynamite 2",100, 0*keV, 200*keV, "keV" );
		histoManager->SetHisto("doubledynamite.EnergyDepositedInEpi1>3.2keV", "Energy deposited per event in Epi layer of dynamite 2",100, 0*keV, 200*keV, "keV");
	
		
	}
	
 	// define the tracker histograms
 	if(detector->GetTrackerConstruction())
 	{
		G4int nTrackers = detector->GetNTrackers();
		
		for( int n=0; n<nTrackers; ++n)
		{
			G4String t = G4UIcommand::ConvertToString(n); //convert the index to a string for name
			
			histoManager->SetTree("tracker"+t+".incident", "Properties of incident particles into tracker"+t);
			histoManager->SetTree("tracker"+t, "Properties of particles in tracker"+t);
			histoManager->SetTree("pcb"+t+".incident", "Properties of incident particles into pcb region around tracker"+t);
			histoManager->SetTree("pcb"+t, "Properties of particles in pcb region around tracker"+t);
			
			// plots within the trackers
			histoManager->SetHisto("tracker"+t+".incident.angleX", "Incident angle of particles into tracker"+t+" (x co-ord); Angle [degrees]", 100, -5, 5);
			histoManager->SetHisto("tracker"+t+".incident.angleY", "Incident angle of particles into tracker"+t+" (y co-ord); Angle [degrees]", 100, -5, 5);
			histoManager->SetHisto("tracker"+t+".incident.Energy", "Energy of the incident particles into tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("tracker"+t+".incident.Energy.proton", "Energy of the protons into tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("tracker"+t+".incident.Energy.neutron", "Energy of the neutrons into tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("tracker"+t+".incident.Energy.electron", "Energy of the electrons into tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("tracker"+t+".incident.Energy.photon", "Energy of the photons into tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("tracker"+t+".exit.Energy", "Energy of the exiting particles from the tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("tracker"+t+".exit.Energy.proton", "Energy of the exiting protons from the tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV")	;
			histoManager->SetHisto("tracker"+t+".exit.Energy.neutron", "Energy of the exiting neutrons from the tracker"+t+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("tracker"+t+".EDep(PerEvent)", "Energy deposited in the tracker"+t+" per event; Energy Deposited [MeV]", 200, 0*keV, 1000*keV, "keV");
			histoManager->SetHisto("tracker"+t+".eLiberated(PerEvent)", "Number of electrons liberated in the tracker"+t+" per event; Energy Deposited [MeV]", 1000, 0, 1000000);
			
			histoManager->SetHisto2D("tracker"+t+".incident.XY", "Scatter plot of particles into tracker"+t+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.XY.proton", "Scatter plot of protons into tracker"+t+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.XY.neutron", "Scatter plot of neutrons into tracker"+t+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.XY.electron", "Scatter plot of electrons into tracker"+t+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.XY.photon", "Scatter plot of photons into tracker"+t+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			
			histoManager->SetHisto2D("tracker"+t+".incident.EvsR", "Scatter plot of energy vs radial position in tracker"+t+";E [GeV]; R [cm]",
  												200, 0*MeV, 50*MeV,
  												200, 0*cm, 10*cm,
  												"MeV", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.EvsR.proton", "Scatter plot of energy vs radial position of protons in tracker"+t+";E [GeV]; R [cm]",
  												200, 0*MeV, 50*MeV,
  												200, 0*cm, 10*cm,
  												"MeV", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.EvsR.neutron", "Scatter plot of energy vs radial position of neutrons in tracker"+t+";E [GeV]; R [cm]",
  												200, 0*MeV, 50*MeV,
  												200, 0*cm, 10*cm,
  												"MeV", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.EvsR.electron", "Scatter plot of energy vs radial position of electrons in tracker"+t+";E [GeV]; R [cm]",
  												200, 0*MeV, 50*MeV,
  												200, 0*cm, 10*cm,
  												"MeV", "cm");
			histoManager->SetHisto2D("tracker"+t+".incident.EvsR.photon", "Scatter plot of energy vs radial position of photons in tracker"+t+";E [GeV]; R [cm]",
  												200, 0*MeV, 50*MeV,
  												200, 0*cm, 10*cm,
  												"MeV", "cm");
			
			//plots within the region around the trackers
			histoManager->SetHisto("pcb"+t+".incident.Energy", "Energy of the incident particles into pcb around tracker"+t+";Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("pcb"+t+".incident.Energy.proton", "Energy of the charged particles into pcb around tracker"+t+";Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("pcb"+t+".incident.Energy.neutron", "Energy of the neutral particles into pcb around tracker"+t+";Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("pcb"+t+".incident.Energy.electron", "Energy of the electrons into pcb around tracker"+t+";Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("pcb"+t+".incident.Energy.photon", "Energy of the photons into pcb around tracker"+t+";Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			
			histoManager->SetHisto2D("pcb"+t+".incident.XY", "Scatter plot of particles into pcb around tracker"+t+";x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												1000, -20*cm, 20*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("pcb"+t+".incident.XY.proton", "Scatter plot of protons into pcb around tracker"+t+";x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												400, -20*cm, 20*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("pcb"+t+".incident.XY.neutron", "Scatter plot of neutrons into pcb around tracker"+t+";x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												400, -20*cm, 20*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("pcb"+t+".incident.XY.electron", "Scatter plot of electrons into pcb around tracker"+t+";x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												400, -20*cm, 20*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("pcb"+t+".incident.XY.photon", "Scatter plot of photons into pcb around tracker"+t+";x [cm]; y [cm]",
  												400, -20*cm, 20*cm,
  												400, -20*cm, 20*cm,
  												"cm", "cm");
		}
		
  }
  
  if(detector->GetRangeTelescopeConstruction())
	{
		histoManager->SetHisto("rangetel.dEdx", "Energy Deposition in range telescope as function of z", 501, 1000*mm, 1500*mm, "mm");
		histoManager->SetHisto("rangetel.dEdx.epi", "Energy Deposition in range telescope as function of z", 501, 1000*mm, 1500*mm, "mm");
		
		for(int i=0; i<detector->GetNRangeTelescopeLayers(); i++)
		{
			G4String rt = G4UIcommand::ConvertToString(i);
			
			histoManager->SetHisto("rangetel"+rt+".incident.Energy", "Energy of the incident particles into range telescope layer"+rt+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("rangetel"+rt+".incident.Energy.proton", "Energy of the protons into range telescope layer"+rt+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("rangetel"+rt+".incident.Energy.neutron", "Energy of the neutrons into range telescope layer"+rt+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("rangetel"+rt+".incident.Energy.electron", "Energy of the electrons into range telescope layer"+rt+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			histoManager->SetHisto("rangetel"+rt+".incident.Energy.photon", "Energy of the photons into range telescope layer"+rt+"; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
			
			histoManager->SetHisto2D("rangetel"+rt+".incident.XY", "Scatter plot of particles into rangetel"+rt+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("rangetel"+rt+".incident.XY.proton", "Scatter plot of protons into rangetel"+rt+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("rangetel"+rt+".incident.XY.neutron", "Scatter plot of neutrons into rangetel"+rt+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("rangetel"+rt+".incident.XY.electron", "Scatter plot of electrons into rangetel"+rt+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			histoManager->SetHisto2D("rangetel"+rt+".incident.XY.photon", "Scatter plot of photons into rangetel"+rt+";x [cm]; y [cm]",
  												200, -10*cm, 10*cm,
  												200, -10*cm, 10*cm,
  												"cm", "cm");
			
			histoManager->SetTree("rangetel"+rt+".incident", "Properties of all the particles into the telescope layer"+rt);
			histoManager->SetTree("rangetelpcb"+rt+".incident", "Properties of all the particles into the pcb around telescope layer"+rt);
		}
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
