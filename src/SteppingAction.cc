#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"

#include "math.h"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* det, RunAction* run, HistoManager* histo, EventAction* event, G4int verbosity)
:detector(det), runAction(run), histoManager(histo), eventAction(event)
{
	verboseLevel = verbosity;
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{ 
	G4int event_id = eventAction->GetEventID();
	G4int rndm_seed = eventAction->GetRndmSeed();
	
	// get basic energy deposition properties
	G4double edep = step->GetTotalEnergyDeposit();  
  G4double niel = step->GetNonIonizingEnergyDeposit();
  G4double total_edep = edep + niel; 
  
	// get the track
	G4Track* track = step->GetTrack();
	
	// get the pre and post point to the step
	G4StepPoint* prePoint     = step->GetPreStepPoint();
  G4StepPoint* postPoint    = step->GetPostStepPoint();
	
	// get the names of the volumes in the step
	G4VSensitiveDetector* sd  = prePoint->GetSensitiveDetector();
	G4VPhysicalVolume* pv     = prePoint->GetPhysicalVolume();
	G4LogicalVolume*   lv     = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
	//G4cout << "Logical Volume Name = " << lv->GetName() << " with a mass of " << G4BestUnit(lv->GetMass(), "Mass") << G4endl;
	
	// get the properties of the particle creating the step
	const G4DynamicParticle* part = step->GetTrack()->GetDynamicParticle();
	G4int pdg               = part->GetPDGcode();
	G4int charge            = part->GetCharge();
	G4double kineticEnergy	 = part->GetKineticEnergy();
	G4String definition     = part->GetDefinition()->GetParticleName();
	
	if( pv->GetName() == "chamberAirGap" )
		eventAction->AddAirGapStep(step);
	
	if( pv->GetName() == "beamPipe" && prePoint->GetStepStatus() == fGeomBoundary)
		eventAction->AddBeamPipePosition(prePoint->GetPosition());
	 
	if(event_id == 1)
	{
		if( pv->GetName() == "beampipe_window")
			G4cout << "Step in beampipe_window" << G4BestUnit(prePoint->GetPosition()/mm, "Length")<< G4endl;
		
		if( pv->GetName() == "firstScatterer_phys")
			G4cout << "Step in scatterer" << G4BestUnit(prePoint->GetPosition()/mm, "Length")<< G4endl;
		
		if( pv->GetName() == "chamberWindow2")
			G4cout << "Step out of air chamber" << G4BestUnit(postPoint->GetPosition()/mm, "Length")<< G4endl;
		
		if( pv->GetName() == "gafFilmSensitive_phys")
			G4cout << "Step in GaFChromicFilm" << G4BestUnit(prePoint->GetPosition()/mm, "Length")<< G4endl;
	}
	
	if(pv->GetName() == "chamberWindow1")
	{
		G4ThreeVector pos = prePoint->GetPosition();
		histoManager->FillHisto2D("airmonitor.XY",pos[0], pos[1],1);
	}
	
	//
	// NOTE this kills the track if in the mop
	//
	if(pv->GetName() == "scatterer_mop")
		track->SetTrackStatus(fStopAndKill);
	
	///if(lv->GetName()=="scattererCollimator_log")
	///	G4cout << "Step in Collimator: " << pdg << " " << G4BestUnit(step->GetStepLength(), "Length") << G4endl;///G4BestUnit(prePoint->GetPosition(), "Length") << " to " << G4BestUnit(postPoint->GetPosition(), "Length") << G4endl;
	
	
	///if(lv->GetName()=="firstScatterer_log")
	///	G4cout << "Step in Scatterer: " << pdg << " " << G4BestUnit(step->GetStepLength(), "Length") << G4endl;///G4BestUnit(prePoint->GetPosition(), "Length") << " to " << G4BestUnit(postPoint->GetPosition(), "Length") << G4endl;
	
	///if(lv->GetName()=="absorber_log")
	///	G4cout << "Step in Absorber: " << pdg << " " << G4BestUnit(step->GetStepLength(), "Length") << G4endl;///G4BestUnit(prePoint->GetPosition(), "Length") << " to " << G4BestUnit(postPoint->GetPosition(), "Length") << G4endl;
	
	
	// calculate the incident and exit angle from volumes
	G4double incidentangleX   = CalculateAngleX(prePoint);
	G4double incidentangleY   = CalculateAngleY(prePoint);
	G4double exitangleX   		 = CalculateAngleX(postPoint);
	G4double exitangleY  			 = CalculateAngleY(postPoint);
	
	// get the process type of the interaction
	G4String 	processName  = postPoint->GetProcessDefinedStep()->GetProcessName();
	G4int    	processType  = 0;//GetProcessType(processName);
	
	G4double x1 = prePoint->GetPosition().x();
	G4double x2 = postPoint->GetPosition().x();
	G4double x_mean =  x1 + G4UniformRand()*(x2-x1);
		
	G4double y1 = prePoint->GetPosition().y();
	G4double y2 = postPoint->GetPosition().y();
	G4double y_mean =  y1 + G4UniformRand()*(y2-y1);
		
	G4double r = pow ( x_mean*x_mean + y_mean*y_mean, 0.5 );
	
	// calculate the z position of the step
	G4double z1 = prePoint->GetPosition().z();
  G4double z2 = postPoint->GetPosition().z();
  G4double z  = z1 + G4UniformRand()*(z2-z1);
	
	// pass the step to relevant functions
	runAction->FillEdep(edep, niel);
  eventAction->AddStep(step);	 
	            
	histoManager->FillHisto("total.PDGID", pdg, 1);
	histoManager->FillHisto("total.dEdx", z, edep);
	histoManager->FillHisto("total.dEdx(NIEL)", z, niel);
	histoManager->FillHisto("total.processType", processType, 1);
	if( definition == "proton"  )		histoManager->FillHisto("total.Energy.proton", kineticEnergy, 1);
	if( definition == "neutron" )		histoManager->FillHisto("total.Energy.neutron", kineticEnergy, 1);
	
	// 
	// Properties of steps within the absorber
  //	
  if( pv->GetName() == "absorber" )
  {  
  	eventAction->AddAbsorberStep(step);
    
  	// Bragg curve in the absorber  
		G4double z_absorber = z - detector->GetAbsorberPosition() + 0.5*detector->GetAbsorberDepth();	
		///G4cout << G4BestUnit(step->GetStepLength(),"Length") << G4endl;
		
		if( verboseLevel>0 )
		{		
			G4cout << "Stepping Action:"
					<< "\n Physical Volume Name = " << pv->GetName()
					<< "\n Sensitive Detector Name = " << sd->GetName()
  				<< "\n z in absorber = " << G4BestUnit(z_absorber,"Length")
  				<< "\n Edep = " << G4BestUnit(edep,"Energy") << std::endl;
		}
		  
  	histoManager->FillHisto("absorber.dEdx", z_absorber, edep);
  	histoManager->FillHisto("absorber.dEdx(NIEL)", z_absorber, niel);
		histoManager->FillProfile("absorber.Ex", z_absorber, kineticEnergy);
		if(definition=="proton") histoManager->FillProfile("absorber.Ex.proton", z_absorber, kineticEnergy);
		
		if( r < 5*mm )
		{
			histoManager->FillHisto("absorber.dEdx.Marcus", z_absorber, edep);
			histoManager->FillProfile("absorber.Ex.Marcus", z_absorber, kineticEnergy);
			if(definition=="proton") histoManager->FillProfile("absorber.Ex.Marcus.proton", z_absorber, kineticEnergy);
		}
  	
  	// Properties of the incoming particles from absorber
  	if( prePoint->GetStepStatus () == fGeomBoundary )
  	{ 	
// 			histoManager->FillTree("absorber.incident", double(rndm_seed), double(event_id), 
// 										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
// 										incidentangleX, incidentangleY,
// 										prePoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
			
	  	histoManager->FillHisto("absorber.incident.angleX", incidentangleX, 1);
			histoManager->FillHisto("absorber.incident.angleY", incidentangleY, 1);
			histoManager->FillHisto("absorber.incident.Energy", prePoint->GetKineticEnergy(), 1);		
			histoManager->FillHisto2D("absorber.incident.XY", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			if(prePoint->GetKineticEnergy()<30*MeV){
			histoManager->FillHisto2D("absorber.incident.LowE.XY", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);}
			histoManager->FillHisto2D("absorber.incident.Energy.XY", prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetKineticEnergy());

			if( verboseLevel>0 )
			{	
				G4cout << "kinetic Energy of Incident Particle upon Absorber = " << G4BestUnit(prePoint->GetKineticEnergy(), "Energy") << G4endl;
			}
		}
		
		// Properties of the outgoing particles from absorber
		if( postPoint->GetStepStatus () == fGeomBoundary )
  	{  	 	
// 			histoManager->FillTree("absorber.exit", double(rndm_seed), double(event_id), 
// 										postPoint->GetPosition().x(), postPoint->GetPosition().y(), postPoint->GetPosition().z(),
// 										incidentangleX, incidentangleY,
// 										postPoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);

			histoManager->FillHisto("absorber.exit.angleX", exitangleX, 1);
			histoManager->FillHisto("absorber.exit.angleY", exitangleY, 1);
			histoManager->FillHisto("absorber.exit.Energy", postPoint->GetKineticEnergy(), 1);
			histoManager->FillHisto("absorber.exit.PDGID", pdg, 1);
			if( definition == "proton" ) histoManager->FillHisto("absorber.exit.Energy.proton", postPoint->GetKineticEnergy(), 1);
			histoManager->FillHisto2D("absorber.exit.XY", postPoint->GetPosition().x(), postPoint->GetPosition().y(), 1);

			if( verboseLevel>0 )
			{				
				G4cout << "kinetic Energy of Exiting Particle from Absorber = " << G4BestUnit(postPoint->GetKineticEnergy(), "Energy") << G4endl;
			}
		}
 	}
 	
 	//
 	// scattering system
 	//
 	bool isIncident = false;
	if( prePoint->GetStepStatus() == fGeomBoundary )
		isIncident = true;

	bool isExit = false;
	if( postPoint->GetStepStatus() == fGeomBoundary )
		isExit = true;
	
	
	G4int trackID = track->GetTrackID();
	G4int parentID = track->GetParentID();
	
	if( pv->GetName().contains("firstScatterer"))
	{
					histoManager->FillTree("firstScatterer", double(rndm_seed), double(event_id), 
										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
										prePoint->GetMomentum().x(), prePoint->GetMomentum().y(), prePoint->GetMomentum().z(),
										trackID,parentID,
										isIncident, isExit,
										prePoint->GetKineticEnergy(), edep, niel,
										pdg, charge, processType);
					
					if(isIncident)
					{
						histoManager->FillTree("firstScatterer.incident", double(rndm_seed), double(event_id), 
										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
										prePoint->GetMomentum().x(), prePoint->GetMomentum().y(), prePoint->GetMomentum().z(),
										trackID,parentID,
										isIncident, isExit,
										prePoint->GetKineticEnergy(), edep, niel,
										pdg, charge, processType);
					}
					if(isExit)
					{
						histoManager->FillTree("firstScatterer.exit", double(rndm_seed), double(event_id), 
										postPoint->GetPosition().x(), postPoint->GetPosition().y(), postPoint->GetPosition().z(),
										postPoint->GetMomentum().x(), postPoint->GetMomentum().y(), postPoint->GetMomentum().z(),
										trackID,parentID,
										isIncident, isExit,
										postPoint->GetKineticEnergy(), edep, niel,
										pdg, charge, processType);
					}
	}
	
	if( pv->GetName().contains("gafFilmSensitive"))
	{
					histoManager->FillTree("GaFChromicFilm", double(rndm_seed), double(event_id), 
										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
										prePoint->GetMomentum().x(), prePoint->GetMomentum().y(), prePoint->GetMomentum().z(),
										trackID,parentID,
										isIncident, isExit,
										prePoint->GetKineticEnergy(), edep, niel,
										pdg, charge, processType);
	}
 	
 	//
 	// in the water tank
 	//
 	
 	if(pv->GetName()=="sensorPV")
	{
		eventAction->AddSensorStep(step);
	}
 	
 	if( pv->GetName().contains("watertank") )
	{
		G4TouchableHistory*theTouchable = (G4TouchableHistory*)(prePoint->GetTouchable());
		G4ThreeVector worldPosition = prePoint->GetPosition();
		G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
		
		if(isIncident && localPosition.z()<(detector->GetWaterTankDepth()/2.0)*-0.8)
			histoManager->FillHisto("watertank.indicent", kineticEnergy);
		
		// want actual positon in water not where sensor is
		G4double z_watertank = localPosition.z() + detector->GetWaterTankDepth()/2.0 + (1.219*mm);// - 100*um/2;// - detector->GetWaterTankPosition() + 0.5*detector->GetWaterTankDepth();	
		histoManager->FillHisto("watertank_dEdx", z_watertank, edep);
		histoManager->FillProfile("watertank.Ex", z_watertank, kineticEnergy);
		if(pdg==2212)
		{
			histoManager->FillHisto("watertank.dEdx.proton", z_watertank, edep);
			histoManager->FillProfile("watertank.Ex.proton", z_watertank, kineticEnergy);
		}
	}
		
		

	if(pv->GetName().contains("validation"))
	{
		eventAction->AddValidationStep(step);
	}

}

G4double SteppingAction::CalculateAngleX(G4StepPoint* point)
{
	G4double x = point->GetMomentumDirection().x();
	G4double z = point->GetMomentumDirection().z();
	G4double theta = atan(x/z) * 180/PI;
	return theta;
}

G4double SteppingAction::CalculateAngleY(G4StepPoint* point)
{
	G4double y = point->GetMomentumDirection().y();
	G4double z = point->GetMomentumDirection().z();
	G4double theta = atan(y/z) * 180/PI;
	return theta;
}

bool SteppingAction::IsInDynamite(G4String pvname)
{
	//if( pvname.contains("dynamite_back") ) return false;
	if( pvname.contains("dynamite") ) return true;
	else return false;
}

bool SteppingAction::IsInTracker(G4String pvname)
{
	if( pvname.contains("tracker") || pvname.contains("string") ) return true;
	else return false;
}

bool SteppingAction::IsInRangeTelescope(G4String pvname)
{
	if( pvname.contains("rangetel") ) return true;
	else return false;
}

G4int SteppingAction::GetProcessType(G4String processName)
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
