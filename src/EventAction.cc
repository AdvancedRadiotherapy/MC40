#include "EventAction.hh"

#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

#include "TMath.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
EventAction::EventAction(HistoManager* histo, DetectorConstruction* det)
:histoManager(histo), detector(det)
{
	absorberSteps.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void EventAction::BeginOfEventAction(const G4Event* evt)
{
	event_id = evt->GetEventID();
	if( event_id < 100 || event_id%10000 == 0 )
		G4cout << "\n\n@@@@@@@@@@@@ Event " << event_id << "@@@@@@@@@@@\n\n" << G4endl;
	
// 	G4cout << "BeginOfEventAction:: Event = " << event_id << G4endl;
	rndm_seed = CLHEP::HepRandom::getTheSeed();
	
	totalEdepAirGapPerEvent = 0.0;
	
	totalEDep = 0.0;
	totalNIEL = 0.0;
	
	sensorEDEP = 0.0;

	absorberEDep = 0.0;
	absorberNIEL = 0.0;
	absorberSteps.clear();
	nAbsorberIncidentSteps = 0;
	nAbsorberExitingSteps  = 0;
	
	ResetDynamite();
	
	doubledynamiteProton[0] = 0;
	doubledynamiteProton[1] = 0;
	
	doubleDynamiteIncidentPosition[0].set(0,0,0);
	doubleDynamiteIncidentPosition[1].set(0,0,0);
	
	doubleDynamiteIncidentMomentum[0].set(0,0,0);
	doubleDynamiteIncidentMomentum[1].set(0,0,0);
	
	doubleDynamiteEnergyDepositedInEpi[0] = 0.0;
	doubleDynamiteEnergyDepositedInEpi[1] = 0.0;
	
	doubleDynamiteIsAProton[0] = false;
	doubleDynamiteIsAProton[1] = false;
	
	for(int i=0; i<detector->GetNTrackers(); i++)
	{
		trackerSteps[i].clear();
		trackerEDep[i] = 0.0;
		trackerNIEL[i] = 0.0;
	}
	
	MeVtoElectron = 3.6*eV;
	
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<detector->GetNRangeTelescopeLayers(); j++)
		{
			rangetelSteps[i][j].clear();
			rangetelEDep[i][j] = 0.0;
			rangetelNIEL[i][j] = 0.0;
		}
	}
	
	pInAbsorber = 0;
	beampipe_pos_int.clear();
	
	G4PrimaryVertex* primaryVertex = evt->GetPrimaryVertex();
  G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
  //G4double ke = primaryParticle->GetKineticEnergy();
	G4ThreeVector pos = primaryVertex	->GetPosition();
	if(pos[1]<0.5*mm && pos[1]>-0.5*mm)
		histoManager->FillHisto("generated.X", pos[0],1);
	histoManager->FillHisto2D("generated.XY",pos[0],pos[1],1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddAirGapStep(const G4Step* step)
{
	totalEdepAirGapPerEvent += step->GetTotalEnergyDeposit();
}
 
void EventAction::EndOfEventAction(const G4Event* evt)
{
	//std::cout << "Total Energy in the Air Gap = " << G4BestUnit(totalEdepAirGapPerEvent,"Energy") << G4endl;
	if(totalEdepAirGapPerEvent>0)
		histoManager->FillHisto("TotalEdepInAirGap", totalEdepAirGapPerEvent, 1);
	
	//std::cout << "nStepsInBeampipe = " << beampipe_pos_int.size() << std::endl;
	for(uint s(0); s<beampipe_pos_int.size(); s++){
		//std::cout << beampipe_pos_int[s] << std::endl;
		histoManager->FillHisto("BeamPipeInteractions_Zpos", beampipe_pos_int[s].getZ());
		if(pInAbsorber){
			histoManager->FillHisto("BeamPipeInteractions_Zpos_pInAbsorber", beampipe_pos_int[s].getZ());
			histoManager->FillHisto("BeamPipeInteractions_Xpos_pInAbsorber", beampipe_pos_int[s].getX());
			histoManager->FillHisto("BeamPipeInteractions_Ypos_pInAbsorber", beampipe_pos_int[s].getY());
		}
	}
	
	G4int nTrackers = detector->GetNTrackers();
    
  // print out the total amount of energy deposited in each event in the absorber
	if( event_id < 100 || event_id%1000 == 0 )
	{
	//	G4cout << "Event " << event_id << ": Total EDep = " << G4BestUnit(totalEDep, "Energy") << ", NIEL = " << G4BestUnit(totalNIEL, "Energy")  << G4endl;
	//	G4cout << "---> Energy deposited in absorber = " << G4BestUnit(absorberEDep, "Energy") << ", NIEL = " << G4BestUnit(absorberNIEL, "Energy") << G4endl;
		if( detector->GetDynamiteConstruction() )
		{
			G4cout<< "---> Energy deposited in dynaMITE = " << G4BestUnit(dynamiteEDep[0], "Energy") << ", NIEL = " << G4BestUnit(dynamiteNIEL[0], "Energy") 
  				<< "\n    Si3N4 layer     = " << G4BestUnit(dynamiteEDep[1], "Energy") << ", NIEL = " << G4BestUnit(dynamiteNIEL[1], "Energy")
  				<< "\n    SiN02 layer     = " << G4BestUnit(dynamiteEDep[2], "Energy") << ", NIEL = " << G4BestUnit(dynamiteNIEL[2], "Energy")
  				<< "\n    Epitaxial layer = " << G4BestUnit(dynamiteEDep[3], "Energy") << ", NIEL = " << G4BestUnit(dynamiteNIEL[3], "Energy")
  				<< "\n    Substrate layer = " << G4BestUnit(dynamiteEDep[4], "Energy") << ", NIEL = " << G4BestUnit(dynamiteNIEL[4], "Energy")
  				<< "\n    Al layer        = " << G4BestUnit(dynamiteEDep[5], "Energy") << ", NIEL = " << G4BestUnit(dynamiteNIEL[5], "Energy")
  				<< G4endl;
 
		}
	
		if( detector->GetTrackerConstruction() )
		{
			for(int n=0;n<nTrackers;n++)
			{
				G4cout 	<< "---> Energy deposited in tracker" << n << " = " << G4BestUnit(trackerEDep[n], "Energy") << G4endl;
			}
		}
	}  
	histoManager->FillHisto("total.EDep(PerEvent)", totalEDep, 1);
	histoManager->FillHisto("total.NIEL(PerEvent)", totalNIEL, 1);

  histoManager->FillHisto("absorber.EDep(PerEvent)", absorberEDep, 1);
  histoManager->FillHisto("absorber.NIEL(PerEvent)", absorberNIEL, 1);
	
	if(sensorEDEP>0)
		histoManager->FillHisto("sensor_WT_edep",sensorEDEP,1);
	
	//look through the absorber steps and try to match up all the steps to each other
	//std::vector<std::vector<const G4Step*> > matchedsteps; // container to hold all matched up steps
	//for(int i=0; i<int(absorberSteps.size()); i++)
	//{
		//look to match end of one step to beginning of the next!
		
  
  if( detector->GetDynamiteConstruction() )
		EndOfEventDynamiteAction();
	
	
	
	if( detector->GetDoubleDynamiteConstruction() )
	{
		G4ThreeVector displacement = CalculateDoubleDynamiteDisplacement();
		if(displacement.z()!=0)
		{
			histoManager->FillHisto("doubledynamite.Displacement.X", displacement.x());
			histoManager->FillHisto("doubledynamite.Displacement.Y", displacement.y());
			histoManager->FillHisto("doubledynamite.Displacement.R", TMath::Sqrt(displacement.x()*displacement.x() + displacement.y()*displacement.y()) );
		}
		
		G4ThreeVector deflection = CalculateDoubleDynamiteDeflection();
		if(deflection.z()!=0)
		{
			histoManager->FillHisto("doubledynamite.Deflection.X", deflection.x());
			histoManager->FillHisto("doubledynamite.Deflection.Y", deflection.y());
		}
		
		
		// look at the amount of energy edeposited in each layer
		G4double EnergyDepositedInEpiCut = 3.2*keV;
		
		histoManager->FillHisto("doubledynamite.EnergyDepositedInEpi0", doubleDynamiteEnergyDepositedInEpi[0]);
		if( doubleDynamiteEnergyDepositedInEpi[0] > EnergyDepositedInEpiCut )
			histoManager->FillHisto("doubledynamite.EnergyDepositedInEpi0>3.2keV", doubleDynamiteEnergyDepositedInEpi[0]);
		
		histoManager->FillHisto("doubledynamite.EnergyDepositedInEpi1", doubleDynamiteEnergyDepositedInEpi[1]);
		if( doubleDynamiteEnergyDepositedInEpi[1] > EnergyDepositedInEpiCut )
			histoManager->FillHisto("doubledynamite.EnergyDepositedInEpi1>3.2keV", doubleDynamiteEnergyDepositedInEpi[1]);
		
	}
	
	if( detector->GetTrackerConstruction() )
	{
		
		for(int n=0;n<nTrackers;n++)
		{
			G4String t = G4UIcommand::ConvertToString(n);
			if(trackerSteps[n].size() != 0)
			{
				histoManager->FillHisto("tracker"+t+".EDep(PerEvent)", trackerEDep[n], 1);
				histoManager->FillHisto("tracker"+t+".eLiberated(PerEvent)", ConvertEDeptoElectrons(trackerEDep[n]), 1);
			}
		}		
	}
 
}

G4ThreeVector EventAction::CalculateDoubleDynamiteDisplacement()
{
	G4ThreeVector displacement = G4ThreeVector(0,0,0);
	//calculate the absolute offset between the hit in epi1 to the hit in epi2
	if( doubleDynamiteIsAProton[0] && doubleDynamiteIsAProton[1] )
	{
		G4double x_offset = doubleDynamiteIncidentPosition[0].x() - doubleDynamiteIncidentPosition[1].x();
		G4double y_offset = doubleDynamiteIncidentPosition[0].y() - doubleDynamiteIncidentPosition[1].y();
		G4double z_offset = doubleDynamiteIncidentPosition[0].z() - doubleDynamiteIncidentPosition[1].z();
			
		displacement = G4ThreeVector(x_offset, y_offset, z_offset);
	}
	return displacement;
}

G4ThreeVector EventAction::CalculateDoubleDynamiteDeflection()
{
	G4ThreeVector deflection = G4ThreeVector(0,0,0);
	
	if( doubleDynamiteIsAProton[0] && doubleDynamiteIsAProton[1] )
	{
		//project the trajectory of the proton into the second sensor and look how far they are deflected by		
		G4double x_theta = TMath::ATan(doubleDynamiteIncidentMomentum[0].x()/doubleDynamiteIncidentMomentum[0].z());
		G4double y_theta = TMath::ATan(doubleDynamiteIncidentMomentum[0].y()/doubleDynamiteIncidentMomentum[0].z());
		
		double zdiff = doubleDynamiteIncidentPosition[1].z() - doubleDynamiteIncidentPosition[0].z();
		double xdiff = (doubleDynamiteIncidentPosition[0].x() + zdiff*TMath::Tan(x_theta)) - doubleDynamiteIncidentPosition[1].x();
		double ydiff = (doubleDynamiteIncidentPosition[0].y() + zdiff*TMath::Tan(y_theta)) - doubleDynamiteIncidentPosition[1].y();
		
		deflection = G4ThreeVector(xdiff, ydiff, zdiff);
	}
	return deflection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddStep(const G4Step* step)
{
	totalEDep += step->GetTotalEnergyDeposit();
  totalNIEL += step->GetNonIonizingEnergyDeposit();	
	
	
}

void EventAction::AddSensorStep(const G4Step* step)
{
	sensorEDEP += step->GetTotalEnergyDeposit();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddAbsorberStep(const G4Step* step)
{
	ProtonInAbsorber(1); // set the flag to be true when here	
	
	absorberEDep += step->GetTotalEnergyDeposit();
  absorberNIEL += step->GetNonIonizingEnergyDeposit();
	absorberSteps.push_back(step);
	
	
	if( step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary )
	{
		nAbsorberIncidentSteps++;
		AbsorberIncidentEnergy = step->GetPreStepPoint()->GetKineticEnergy();
	
	}
	
	if( step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary )
	{
		nAbsorberExitingSteps++;
		AbsorberExitingEnergy = step->GetPostStepPoint()->GetKineticEnergy();
	}	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::ResetDynamite()
{
	for(int i=0; i<6; i++)
	{
		dynamiteEDep[i] = 0.0;
		dynamiteNIEL[i] = 0.0;
	}
	nDynamiteIncidentSteps = 0;
	dynamiteSteps=0;
}

void EventAction::AddDynamiteStep(const G4Step* step)
{
	G4double edep = step->GetTotalEnergyDeposit();
	G4double niel = step->GetNonIonizingEnergyDeposit();
	
	G4StepPoint* prePoint = step->GetPreStepPoint();
	G4ThreeVector prePointPosition = prePoint->GetPosition();
	G4VPhysicalVolume* pv = prePoint->GetPhysicalVolume();
	G4String pvname = pv->GetName();
	bool isIncident = false;
	if( prePoint->GetStepStatus() == fGeomBoundary )
		isIncident = true;
	
	G4StepPoint* postPoint = step->GetPostStepPoint();	
	G4ThreeVector postPointPosition = postPoint->GetPosition();
	bool isExit = false;
	if( postPoint->GetStepStatus() == fGeomBoundary )
		isExit = true;
	G4String 	processName  = postPoint->GetProcessDefinedStep()->GetProcessName();
	G4int    	processType  = GetProcessType(processName);
	
	G4Track* track = step->GetTrack();
	G4int trackID = track->GetTrackID();
	G4int parentID = track->GetParentID();
	
	const G4DynamicParticle* part = track->GetDynamicParticle();
	G4int pdgid               = part->GetPDGcode();
	G4int charge            = part->GetCharge();
	G4double kineticEnergy	 = part->GetKineticEnergy();
	G4ThreeVector momentum = part->GetMomentum();
	G4String definition     = part->GetDefinition()->GetParticleName();
	
	G4double x1 = prePointPosition.x();
	G4double x2 = postPointPosition.x();
	G4double x =  x1 + G4UniformRand()*(x2-x1);
		
	G4double y1 = prePointPosition.y();
	G4double y2 = postPointPosition.y();
	G4double y =  y1 + G4UniformRand()*(y2-y1);

	G4double z1 = prePointPosition.z();
  G4double z2 = postPointPosition.z();
  G4double z  = z1 + G4UniformRand()*(z2-z1);
	
	
	dynamiteEDep[0] += edep;
	dynamiteNIEL[0] += niel;
		
	if( pvname == "dynamite_Si3N4" )
	{
		dynamiteEDep[1] += edep;
		dynamiteNIEL[1] += niel;
				
	}
	else if( pvname == "dynamite_SiO2" )
	{
		dynamiteEDep[2] += edep;
		dynamiteNIEL[2] += niel;
	}
	else if( pvname == "dynamite_epi" )
	{
		dynamiteEDep[3] += edep;
		dynamiteNIEL[3] += niel;
		
		dynamiteSteps++;
// 		G4cout << "\n particle = " << definition
// 					<< "\n TrackID = " << trackID 
// 					<< "\n ParentTrackID = " << parentID
// 					<< "\n prePoint.Position() = " << G4BestUnit(prePointPosition, "Length")
// 					<< "\n postPoint.Position() = " << G4BestUnit(postPointPosition, "Length")
// 					<< "\n edep = " << G4BestUnit(edep, "Energy") 
// 					<< "\n niel = " << G4BestUnit(niel, "Energy") << G4endl;
	// count the number of particles incident on the sensor surface
		if( isIncident )
		{
			nDynamiteIncidentSteps++;
		}
		
		histoManager->FillTree("dynamite.epi", double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
													 
	}
	else if (pvname == "dynamite_sub" )
	{
		dynamiteEDep[4] += edep;
		dynamiteNIEL[4] += niel;
	}	
	else if( pvname == "dynamite_back" )
	{
		dynamiteEDep[5] += edep;
		dynamiteNIEL[5] += niel;
	}
}

void EventAction::EndOfEventDynamiteAction()
{				  histoManager->FillHisto("dynamite.EDep(PerEvent)", dynamiteEDep[0], 1);
		histoManager->FillHisto("dynamite.EDep(PerEvent).epi", dynamiteEDep[3], 1);
		histoManager->FillHisto("dynamite.EDep(PerEvent).noAl", dynamiteEDep[0]-dynamiteEDep[5], 1);
  	histoManager->FillHisto("dynamite.NIEL(PerEvent)", dynamiteNIEL[0], 1);
		
		//std::cout << "Total number of incident particles = " << nDynamiteIncidentSteps << std::endl;
		//std::cout << "Total number of steps in epi layer = " << dynamiteSteps << std::endl;
		
		histoManager->FillHisto("dynamite.incident.nParticles", nDynamiteIncidentSteps, 1);
  	if(nDynamiteIncidentSteps == 0)  histoManager->FillHisto("dynamite.ratio.nAbsorbertodynamite", 0, 1);
  	else if( nAbsorberExitingSteps > 0 ) histoManager->FillHisto("dynamite.ratio.nAbsorbertodynamite", nDynamiteIncidentSteps/nAbsorberExitingSteps, 1);
  	histoManager->FillHisto2D("dynamite.correlation.nAbsorbertodynamite", nDynamiteIncidentSteps, nAbsorberExitingSteps, 1);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddDoubleDynamiteStep(const G4Step* step)
{	
	G4double edep = step->GetTotalEnergyDeposit();
	G4double niel = step->GetNonIonizingEnergyDeposit();
	
	G4StepPoint* prePoint = step->GetPreStepPoint();
	G4ThreeVector prePointPosition = prePoint->GetPosition();
	G4VPhysicalVolume* pv = prePoint->GetPhysicalVolume();
	G4String pvname = pv->GetName();
	bool isIncident = false;
	if( prePoint->GetStepStatus() == fGeomBoundary )
		isIncident = true;
	
	G4StepPoint* postPoint = step->GetPostStepPoint();	
	G4ThreeVector postPointPosition = postPoint->GetPosition();
	bool isExit = false;
	if( postPoint->GetStepStatus() == fGeomBoundary )
		isExit = true;
	G4String 	processName  = postPoint->GetProcessDefinedStep()->GetProcessName();
	G4int    	processType  = GetProcessType(processName);
	
	G4Track* track = step->GetTrack();
	G4int trackID = track->GetTrackID();
	G4int parentID = track->GetParentID();
	
	const G4DynamicParticle* part = track->GetDynamicParticle();
	G4int pdgid               = part->GetPDGcode();
	G4int charge            = part->GetCharge();
	G4double kineticEnergy	 = part->GetKineticEnergy();
	G4ThreeVector momentum = part->GetMomentum();
	G4String definition     = part->GetDefinition()->GetParticleName();
	
	G4double x1 = prePointPosition.x();
	G4double x2 = postPointPosition.x();
	G4double x =  x1 + G4UniformRand()*(x2-x1);
		
	G4double y1 = prePointPosition.y();
	G4double y2 = postPointPosition.y();
	G4double y =  y1 + G4UniformRand()*(y2-y1);

	G4double z1 = prePointPosition.z();
  G4double z2 = postPointPosition.z();
  G4double z  = z1 + G4UniformRand()*(z2-z1);
	
	G4ThreeVector DynamitePosition = detector->GetDynamitePosition();
	G4ThreeVector prePointDynamitePosition = G4ThreeVector(prePoint->GetPosition().getX() + DynamitePosition.getX(),
																													 prePoint->GetPosition().getY() + DynamitePosition.getY(),
																													 prePoint->GetPosition().getZ());
	G4ThreeVector postPointDynamitePosition = G4ThreeVector(postPoint->GetPosition().getX() + DynamitePosition.getX(),
																													 postPoint->GetPosition().getY() + DynamitePosition.getY(),
																													 postPoint->GetPosition().getZ());
	
	if( pv->GetName() == "dynamite_epi0" && prePoint->GetStepStatus() == fGeomBoundary )
	{
		histoManager->FillTree("doubledynamite.first.incident", double(rndm_seed), double(event_id), 
										prePointDynamitePosition.x(), prePointDynamitePosition.y(), prePointDynamitePosition.z(),
										momentum.x(), momentum.y(), momentum.z(),
												trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
	}
	
	if( pv->GetName() == "dynamite_epi1" && prePoint->GetStepStatus() == fGeomBoundary )
	{
					histoManager->FillTree("doubledynamite.second.incident", double(rndm_seed), double(event_id), 
										prePointDynamitePosition.x(), prePointDynamitePosition.y(), prePointDynamitePosition.z(),
										momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
	}
		
	if(definition == "proton")
	{
		if( pv->GetName() == "dynamite_epi0" && prePoint->GetStepStatus() == fGeomBoundary )
		{
			doubledynamiteProton[0] = step->GetTrack()->GetDynamicParticle();
			doubleDynamiteProtonMomentum[0] = doubledynamiteProton[0]->GetMomentum();
			histoManager->FillHisto2D("doubledynamite.first.incident.XY", prePointDynamitePosition.x(), prePointDynamitePosition.y());
			
			doubleDynamiteIncidentPosition[0] = prePointDynamitePosition;
			doubleDynamiteIncidentMomentum[0] = doubledynamiteProton[0]->GetMomentumDirection(); 
			doubleDynamiteIsAProton[0]=true;
			
			
		}
		if( pv->GetName() == "dynamite_back0" && postPoint->GetStepStatus() == fGeomBoundary )
		{
			doubledynamiteProton[1] = step->GetTrack()->GetDynamicParticle();
			doubleDynamiteProtonMomentum[1] = doubledynamiteProton[1]->GetMomentum();
		}
		else if( pv->GetName() == "dynamite_epi1" && prePoint->GetStepStatus() == fGeomBoundary )
		{
			doubleDynamiteIsAProton[1]=true;
			doubleDynamiteIncidentPosition[1] = prePointDynamitePosition;
			doubleDynamiteIncidentMomentum[1] = doubledynamiteProton[1]->GetMomentumDirection();
			
			histoManager->FillHisto2D("doubledynamite.second.incident.XY", prePointDynamitePosition.x(), prePointDynamitePosition.y());

		}
		
		// count up the total energy deposited in the epi layer for the sensors
		if( pv->GetName() == "dynamite_epi0" )
		{
			doubleDynamiteEnergyDepositedInEpi[0] += edep;
		}
		if( pv->GetName() == "dynamite_epi1" )
		{
			doubleDynamiteEnergyDepositedInEpi[1] += edep;
		}
	}
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddTrackerStep(const G4Step* step)
{
	G4double edep = step->GetTotalEnergyDeposit();
	G4double niel = step->GetNonIonizingEnergyDeposit();
	
	G4StepPoint* prePoint = step->GetPreStepPoint();
	G4ThreeVector prePointPosition = prePoint->GetPosition();
	G4VPhysicalVolume* pv = prePoint->GetPhysicalVolume();
	G4String pvname = pv->GetName();
	bool isIncident = false;
	if( prePoint->GetStepStatus() == fGeomBoundary )
		isIncident = true;
	
	G4StepPoint* postPoint = step->GetPostStepPoint();	
	G4ThreeVector postPointPosition = postPoint->GetPosition();
	bool isExit = false;
	if( postPoint->GetStepStatus() == fGeomBoundary )
		isExit = true;
	G4String 	processName  = postPoint->GetProcessDefinedStep()->GetProcessName();
	G4int    	processType  = GetProcessType(processName);
	
	G4Track* track = step->GetTrack();
	G4int trackID = track->GetTrackID();
	G4int parentID = track->GetParentID();
	
	const G4DynamicParticle* part = track->GetDynamicParticle();
	G4int pdgid               = part->GetPDGcode();
	G4int charge            = part->GetCharge();
	G4double kineticEnergy	 = part->GetKineticEnergy();
	G4ThreeVector momentum = part->GetMomentum();
	G4String definition     = part->GetDefinition()->GetParticleName();
	
	G4double x1 = prePointPosition.x();
	G4double x2 = postPointPosition.x();
	G4double x =  x1 + G4UniformRand()*(x2-x1);
		
	G4double y1 = prePointPosition.y();
	G4double y2 = postPointPosition.y();
	G4double y =  y1 + G4UniformRand()*(y2-y1);

	G4double z1 = prePointPosition.z();
  G4double z2 = postPointPosition.z();
  G4double z  = z1 + G4UniformRand()*(z2-z1);
	
	bool isInTracker = true;
	if( pvname.contains("pcb") )
		isInTracker = false;
	
	G4int nTracker = -1;
	
	for(int t=detector->GetNTrackers(); t>=0; t--)
	{
		if( pvname.contains(G4UIcommand::ConvertToString(t) ) ) 
		{
			trackerSteps[t].push_back(step);			
			trackerEDep[t] += edep;
			trackerNIEL[t] += niel;
			
			nTracker = t;
			
			break;
		}
	}
	
	
	if( isInTracker && isIncident )
	{
		histoManager->FillTree("tracker"+G4UIcommand::ConvertToString(nTracker)+".incident", double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
	}
	
	if( isInTracker )
	{
		histoManager->FillTree("tracker"+G4UIcommand::ConvertToString(nTracker), double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
	}
	
	if( !isInTracker && isIncident )
	{
		histoManager->FillTree("pcb"+G4UIcommand::ConvertToString(nTracker)+".incident", double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
	}
	
	if( !isInTracker )
	{
		histoManager->FillTree("pcb"+G4UIcommand::ConvertToString(nTracker), double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddRangeTelescopeStep(const G4Step* step)
{
	G4double edep = step->GetTotalEnergyDeposit();
	G4double niel = step->GetNonIonizingEnergyDeposit();
	
	G4String pvname = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
	if(pvname.contains("epi"))
	{
		for(int t=detector->GetNRangeTelescopeLayers(); t>=0; t--)
		{
			if( pvname.contains(G4UIcommand::ConvertToString(t) ) ) 
			{
				rangetelSteps[1][t].push_back(step);			
				rangetelEDep[1][t] += edep;
				rangetelNIEL[1][t] += niel;
				break;
			}
		}
	}
}

void EventAction::AddValidationStep(const G4Step* step)
{
	G4double edep = step->GetTotalEnergyDeposit();
	G4double niel = step->GetNonIonizingEnergyDeposit();
	
	G4StepPoint* prePoint = step->GetPreStepPoint();
	G4ThreeVector prePointPosition = prePoint->GetPosition();
	G4VPhysicalVolume* pv = prePoint->GetPhysicalVolume();
	G4String pvname = pv->GetName();
	bool isIncident = false;
	if( prePoint->GetStepStatus() == fGeomBoundary )
		isIncident = true;
	
	G4StepPoint* postPoint = step->GetPostStepPoint();	
	G4ThreeVector postPointPosition = postPoint->GetPosition();
	bool isExit = false;
	if( postPoint->GetStepStatus() == fGeomBoundary )
		isExit = true;
	G4String 	processName  = postPoint->GetProcessDefinedStep()->GetProcessName();
	G4int    	processType  = GetProcessType(processName);
	
	G4Track* track = step->GetTrack();
	G4int trackID = track->GetTrackID();
	G4int parentID = track->GetParentID();
	
	const G4DynamicParticle* part = track->GetDynamicParticle();
	G4int pdgid               = part->GetPDGcode();
	G4int charge            = part->GetCharge();
	G4double kineticEnergy	 = part->GetKineticEnergy();
	G4ThreeVector momentum = part->GetMomentum();
	G4String definition     = part->GetDefinition()->GetParticleName();
	
	G4double x1 = prePointPosition.x();
	G4double x2 = postPointPosition.x();
	G4double x =  x1 + G4UniformRand()*(x2-x1);
		
	G4double y1 = prePointPosition.y();
	G4double y2 = postPointPosition.y();
	G4double y =  y1 + G4UniformRand()*(y2-y1);

	G4double z1 = prePointPosition.z();
  G4double z2 = postPointPosition.z();
  G4double z  = z1 + G4UniformRand()*(z2-z1);
	
	if(isIncident)
	{
		if(pvname.contains("0"))
		{
			histoManager->FillTree("validation0", double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
		}
		if(pvname.contains("1"))
		{
			histoManager->FillTree("validation1", double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
		}
		if(pvname.contains("2"))
		{
			histoManager->FillTree("validation2", double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
		}
		if(pvname.contains("3"))
		{
			histoManager->FillTree("validation3", double(rndm_seed), double(event_id),
													 x,y,z,
													momentum.x(), momentum.y(), momentum.z(),
													 trackID, parentID,
												 isIncident, isExit,
												 kineticEnergy, edep, niel,
												 pdgid, charge, processType);
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double EventAction::ConvertEDeptoElectrons(G4double e)
{
	G4double conversion = 3.6*eV;
	double electrons = e / conversion;
	return electrons;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int EventAction::GetEventID()
{
	return event_id;
}

G4int EventAction::GetRndmSeed()
{
	return rndm_seed;
}

G4int EventAction::GetProcessType(G4String processName)
{
	if( processName == "phot" ) 									return 0;
	else if( processName == "compt" ) 						return 1;
	else if( processName == "conv" ) 						return 2;
	else if( processName == "Rayl" ) 						return 3;
	else if( processName == "msc" ) 							return 4;
	else if( processName == "annihil" ) 					return 5;
	else if( processName == "eIoni" ) 						return 6;
	else if( processName == "eBrem" ) 						return 7;
	else if( processName == "hIoni" ) 						return 8;
	else if( processName == "hBrems" ) 					return 9;
	else if( processName == "hPairProd" ) 				return 10;
	else if( processName == "ionIoni" ) 					return 11;
	else if( processName == "nuclearStopping" ) 	return 12;
	else if( processName == "muMsc" ) 						return 13;
	else if( processName == "muIoni" ) 					return 14;
	else if( processName == "muBrems" ) 					return 15;
	else if( processName == "muPairProd" ) 			return 16;
	else if( processName == "CoulombScat" ) 			return 17;
	else if( processName.contains("Elastic") ) 			return 18;
	else if( processName.contains("Inelastic") ) 			return 19;
	else if( processName == "nCapture" ) 				return 20;
	else 																					return -1;
}