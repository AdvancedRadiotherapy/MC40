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
	G4int    	processType  = GetProcessType(processName);
	
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
		
		
	
	//	
	// in the dynamite sensor
	//
	if( IsInDynamite(pv->GetName()) && detector->GetDynamiteConstruction()  )
	{ 
		eventAction->AddDynamiteStep(step);
		histoManager->FillHisto("dynamite.processType", processType, 1);
		
		// allows processes in Al backing to not bias results when comparing processes
		if( !pv->GetName().contains("back") ) histoManager->FillHisto("dynamite.processType.noAl", processType, 1);
		
		// look at the process in just the epi layer
		if( pv->GetName().contains("epi") ) histoManager->FillHisto("dynamite.processType.epi", processType, 1);
			
		// Bragg curve in the absorber  
 	 	G4double z1 = prePoint->GetPosition().z();
  	G4double z2 = postPoint->GetPosition().z();
  	G4double z  = z1 + G4UniformRand()*(z2-z1) - detector->GetDynamiteZPosition() + 0.5*detector->GetDynamiteDepth();
		
													 
	  
	  if( verboseLevel > 0 )	  
	  {
	  	G4cout << "In DynAMIte sensor " << G4endl;
		  G4cout << " pv->GetName()   = " << pv->GetName() << G4endl;
		  G4cout << " Depth in sensor = " << G4BestUnit(z, "Length") << G4endl;
		}

  	histoManager->FillHisto("dynamite.dEdx", z, edep);
		histoManager->FillHisto("dynamite.dEdx.full", z, edep);
  	histoManager->FillHisto("dynamite.dEdx(NIEL)", z, niel);
		if(definition=="proton") 
		{
			histoManager->FillHisto("dynamite.dEdx.proton", z, edep);
			G4double kinEcut = 1*MeV;
			if(kineticEnergy>kinEcut)
			{
				histoManager->FillHisto("dynamite.dEdx.proton.E>1MeV", z, edep);
			}
		}
		if(pv->GetName()=="dynamite_epi")
		{
			histoManager->FillHisto("dynamite.dEdx.epi(zoomed)", z, edep);
		}
	
		// Properties of incoming particles to dynamite sensor
		if( prePoint->GetStepStatus () == fGeomBoundary && pv->GetName() == "dynamite_epi") 
		{
/*			histoManager->FillTree("dynamite.incident", double(rndm_seed), double(event_id), 
										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
										incidentangleX, incidentangleY,
										prePoint->GetKineticEnergy(), edep, niel,
										pdg, charge, processType);
			
			*/histoManager->FillHisto("dynamite.incident.PDGID", pdg, 1);
			histoManager->FillHisto("dynamite.incident.angleX", incidentangleX, 1);
			histoManager->FillHisto("dynamite.incident.angleY", incidentangleY, 1);
			histoManager->FillHisto("dynamite.incident.Energy", prePoint->GetKineticEnergy(), 1);			
			histoManager->FillHisto2D("dynamite.incident.XY", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			//look at just the protons
			if(definition=="proton")
			{
				histoManager->FillHisto("dynamite.incident.Energy.proton", prePoint->GetKineticEnergy(), 1);
				histoManager->FillHisto2D("dynamite.incident.XY.proton", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			}
			//look at just the neutrons
			if(definition=="neutron")
			{
				histoManager->FillHisto("dynamite.incident.Energy.neutron", prePoint->GetKineticEnergy(), 1);
				histoManager->FillHisto2D("dynamite.incident.XY.neutron", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			}
			if(definition=="gamma")
			{
				histoManager->FillHisto("dynamite.incident.Energy.photon", prePoint->GetKineticEnergy(), 1);
			}	
			if(definition=="e-" || definition=="e+")
			{
				histoManager->FillHisto("dynamite.incident.Energy.electron", prePoint->GetKineticEnergy(), 1);
			}
			
		}
			
		// Properties if outgoing particles from dynamite
		if( postPoint->GetStepStatus () == fGeomBoundary )
  	{  		
			//histoManager->FillHisto(11, kineticEnergy, 1);				
		}
	}
	
	if( detector->GetDoubleDynamiteConstruction() )
	{
		histoManager->FillHisto("doubledynamite.dEdx", z, edep);
		eventAction->AddDoubleDynamiteStep(step);
		
		G4ThreeVector DynamitePosition = detector->GetDynamitePosition();
		G4ThreeVector prePointDynamitePosition = G4ThreeVector(prePoint->GetPosition().getX() + DynamitePosition.getX(),
																													 prePoint->GetPosition().getY() + DynamitePosition.getY(),
																													 prePoint->GetPosition().getZ());
		G4ThreeVector postPointDynamitePosition = G4ThreeVector(postPoint->GetPosition().getX() + DynamitePosition.getX(),
																													 postPoint->GetPosition().getY() + DynamitePosition.getY(),
																													 postPoint->GetPosition().getZ());
		
		if(definition == "proton")
		{
			if( pv->GetName() == "dynamite_epi0" && prePoint->GetStepStatus() == fGeomBoundary )
			{
				histoManager->FillHisto("doubledynamite.incident.Energy.epi.first", prePoint->GetKineticEnergy());
			}
			if( pv->GetName() == "dynamite_back0" && postPoint->GetStepStatus() == fGeomBoundary )
			{
				histoManager->FillHisto("doubledynamite.exit.Energy.first", postPoint->GetKineticEnergy());
			}
			if( pv->GetName() == "dynamite_epi1" && prePoint->GetStepStatus() == fGeomBoundary )
			{
				histoManager->FillHisto("doubledynamite.incident.Energy.epi.second", prePoint->GetKineticEnergy());
			}
			if( pv->GetName() == "dynamite_Si3N41" && prePoint->GetStepStatus() == fGeomBoundary )
			{
				histoManager->FillHisto("doubledynamite.incident.Energy.oxide.second", prePoint->GetKineticEnergy());
			}
		}
		
// 		// fill trees with information 
// 		if( pv->GetName() == "dynamite_epi0" &&  prePoint->GetStepStatus() == fGeomBoundary ) // particles entering dynamite 1
// 		{
// 			histoManager->FillTree("doubledynamite.first.incident", double(rndm_seed), double(event_id), 
// 										prePointDynamitePosition.x(), prePointDynamitePosition.y(), prePointDynamitePosition.z(),
// 										incidentangleX, incidentangleY,
// 										prePoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
// 		}
// 		
// 		if( pv->GetName() == "dynamite_back0" &&  postPoint->GetStepStatus() == fGeomBoundary ) // particles entering dynamite 1
// 		{
// 			histoManager->FillTree("doubledynamite.first.exit", double(rndm_seed), double(event_id), 
// 										postPointDynamitePosition.x(), postPointDynamitePosition.y(), postPointDynamitePosition.z(),
// 										exitangleX, exitangleY,
// 										postPoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
// 		}
// 		
// 		if( pv->GetName() == "dynamite_epi1" &&  prePoint->GetStepStatus() == fGeomBoundary ) // particles entering dynamite 2
// 		{
// 			histoManager->FillTree("doubledynamite.second.incident", double(rndm_seed), double(event_id), 
// 										prePointDynamitePosition.x(), prePointDynamitePosition.y(), prePointDynamitePosition.z(),
// 										incidentangleX, incidentangleY,
// 										prePoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
// 		}
// 		
// 		if( pv->GetName() == "dynamite_back1" &&  postPoint->GetStepStatus() == fGeomBoundary ) // particles entering dynamite 1
// 		{
// 			histoManager->FillTree("doubledynamite.second.exit", double(rndm_seed), double(event_id), 
// 										postPointDynamitePosition.x(), postPointDynamitePosition.y(), postPointDynamitePosition.z(),
// 										exitangleX, exitangleY,
// 										postPoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
// 		}
	}
	
	if(pv->GetName().contains("validation"))
	{
		eventAction->AddValidationStep(step);
	}
	//
	// If the step is in the trackers
	//
	if( detector->GetTrackerConstruction() && IsInTracker(pv->GetName()) )
	{
		eventAction->AddTrackerStep(step);
		
// 		// is it in pcb?
// 		bool inpcb = false;
// 		if( pv->GetName().contains("pcb") ) inpcb=true;
// 		
// 		// what is the element number?
// 		G4int nTracker = -1;
// 		for(int t=0; t<detector->GetNTrackers(); t++)
// 		{
// 			if( pv->GetName().contains(G4UIcommand::ConvertToString(t) ) ) nTracker=t;
// 		}
// 		
// 		G4String t = G4UIcommand::ConvertToString(nTracker);
// 		
// 		double r = pow( pow( prePoint->GetPosition().x(), 2 ) + pow( prePoint->GetPosition().y(), 2 ), 0.5 );
// 		if( prePoint->GetStepStatus () == fGeomBoundary) 
// 		{
// 			histoManager->FillHisto2D("tracker"+t+".incident.EvsR", prePoint->GetKineticEnergy(), r);
// 			if(definition == "proton") histoManager->FillHisto2D("tracker"+t+".incident.EvsR.proton", prePoint->GetKineticEnergy(), r);
// 			if(definition == "neutron") histoManager->FillHisto2D("tracker"+t+".incident.EvsR.neutron", prePoint->GetKineticEnergy(), r);
// 			if(definition == "e-" || definition == "e+") histoManager->FillHisto2D("tracker"+t+".incident.EvsR.electron", prePoint->GetKineticEnergy(), r);
// 			if(definition == "gamma") histoManager->FillHisto2D("tracker"+t+".incident.EvsR.photon", prePoint->GetKineticEnergy(), r);
// 		}
// 		
// 		// if in tracker
// 		if( !inpcb )
// 		{
// 			// Properties of incoming particles to tracker0
// 			if( prePoint->GetStepStatus () == fGeomBoundary) 
// 			{
// /*				histoManager->FillTree("tracker"+t+".incident", double(rndm_seed), double(event_id), 
// 										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
// 										incidentangleX, incidentangleY,
// 										prePoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
// 		*/		
// 				histoManager->FillHisto("tracker"+t+".incident.angleX", incidentangleX, 1);
// 				histoManager->FillHisto("tracker"+t+".incident.angleY", incidentangleY, 1);
// 				histoManager->FillHisto("tracker"+t+".incident.Energy", prePoint->GetKineticEnergy(), 1);
// 				histoManager->FillHisto2D("tracker"+t+".incident.XY", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 				
// 				if( definition == "proton" )
// 				{
// 					histoManager->FillHisto("tracker"+t+".incident.Energy.proton", prePoint->GetKineticEnergy(), 1);
// 					histoManager->FillHisto2D("tracker"+t+".incident.XY.proton", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 				}
// 				
// 				else if( definition == "neutron" )
// 				{
// 					histoManager->FillHisto("tracker"+t+".incident.Energy.neutron", prePoint->GetKineticEnergy(), 1);
// 					histoManager->FillHisto2D("tracker"+t+".incident.XY.neutron", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 				}		
// 				
// 				else if( definition == "e-" || definition == "e+" )
// 				{
// 					histoManager->FillHisto("tracker"+t+".incident.Energy.electron", prePoint->GetKineticEnergy(), 1);
// 					histoManager->FillHisto2D("tracker"+t+".incident.XY.electron", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 				}	
// 				
// 				else if( definition == "gamma" )
// 				{
// 					histoManager->FillHisto("tracker"+t+".incident.Energy.photon", prePoint->GetKineticEnergy(), 1);
// 					histoManager->FillHisto2D("tracker"+t+".incident.XY.photon", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 				}					
// 			}
// 			
// 			//Properties of outgoing particles from tracker
// 			if( postPoint->GetStepStatus() == fGeomBoundary )
// 			{
// 				histoManager->FillHisto("tracker"+t+".exit.Energy", postPoint->GetKineticEnergy(), 1);
// 				if( definition == "proton" )
// 				{
// 					histoManager->FillHisto("tracker"+t+".exit.Energy.proton", postPoint->GetKineticEnergy(), 1);
// 				}
// 				else if( definition == "neutron" )
// 				{
// 					histoManager->FillHisto("tracker"+t+".exit.Energy.neutron", postPoint->GetKineticEnergy(), 1);
// 				}			
// 			} 
// 		}// end of if( !inpcb )
// 		else
// 		{
// // 			histoManager->FillTree("pcb"+t+".incident", double(rndm_seed), double(event_id), 
// // 										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
// // 										incidentangleX, incidentangleY,
// // 										prePoint->GetKineticEnergy(), edep, niel,
// // 										pdg, charge, processType);
// // 										
// 			histoManager->FillHisto("pcb"+t+".incident.Energy", prePoint->GetKineticEnergy(), 1);
// 			histoManager->FillHisto2D("pcb"+t+".incident.XY", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 			if( definition == "proton" )
// 			{
// 				histoManager->FillHisto("pcb"+t+".incident.Energy.proton", prePoint->GetKineticEnergy(), 1);
// 				histoManager->FillHisto2D("pcb"+t+".incident.XY.proton", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 			}
// 			else if( definition == "neutron" )
// 			{
// 				histoManager->FillHisto("pcb"+t+".incident.Energy.neutron", prePoint->GetKineticEnergy(), 1);
// 				histoManager->FillHisto2D("pcb"+t+".incident.XY.neutron", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 			}	
// 			else if( definition == "e-" || definition == "e+" )
// 			{
// 				histoManager->FillHisto("pcb"+t+".incident.Energy.electron", prePoint->GetKineticEnergy(), 1);
// 				histoManager->FillHisto2D("pcb"+t+".incident.XY.electron", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 			}
// 			else if( definition == "gamma" )
// 			{
// 				histoManager->FillHisto("pcb"+t+".incident.Energy.photon", prePoint->GetKineticEnergy(), 1);
// 				histoManager->FillHisto2D("pcb"+t+".incident.XY.photon", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
// 			}
// 		}
		
	}
	
	//
	//
	//
	if( detector->GetRangeTelescopeConstruction() && IsInRangeTelescope(pv->GetName()) )
	{
		
		// is it in pcb?
		bool inpcb = false;
		if( pv->GetName().contains("pcb") ) inpcb=true;
		
		histoManager->FillHisto("rangetel.dEdx", z, edep);
		if(pv->GetName().contains("epi")) histoManager->FillHisto("rangetel.dEdx.epi", z, edep);
		
		// what is the element number?
		G4int nLayer = -1;
		for(int t=0; t<detector->GetNRangeTelescopeLayers(); t++)
		{
			if( pv->GetName().contains(G4UIcommand::ConvertToString(t) ) ) nLayer=t;
		}
		
		G4String t = G4UIcommand::ConvertToString(nLayer);
		
		if( prePoint->GetStepStatus () == fGeomBoundary && !inpcb && pv->GetName().contains("epi") ) 
		{	
			histoManager->FillHisto2D("rangetel"+t+".incident.XY", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			histoManager->FillHisto("rangetel"+t+".incident.Energy", prePoint->GetKineticEnergy(), 1);	
			
// 			histoManager->FillTree("rangetel"+t+".incident", double(rndm_seed), double(event_id), 
// 										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
// 										incidentangleX, incidentangleY,
// 										prePoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
// 			
			if( definition == "proton" )
			{
				histoManager->FillHisto("rangetel"+t+".incident.Energy.proton", prePoint->GetKineticEnergy(), 1);
				histoManager->FillHisto2D("rangetel"+t+".incident.XY.proton", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			}
				
			else if( definition == "neutron" )
			{
				histoManager->FillHisto("rangetel"+t+".incident.Energy.neutron", prePoint->GetKineticEnergy(), 1);
				histoManager->FillHisto2D("rangetel"+t+".incident.XY.neutron", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			}		
				
			else if( definition == "e-" || definition == "e+" )
			{
				histoManager->FillHisto("rangetel"+t+".incident.Energy.electron", prePoint->GetKineticEnergy(), 1);
				histoManager->FillHisto2D("rangetel"+t+".incident.XY.electron", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			}	
				
			else if( definition == "gamma" )
			{
				histoManager->FillHisto("rangetel"+t+".incident.Energy.photon", prePoint->GetKineticEnergy(), 1);
				histoManager->FillHisto2D("rangetel"+t+".incident.XY.photon", prePoint->GetPosition().x(), prePoint->GetPosition().y(), 1);
			}				
		}
		
		if( prePoint->GetStepStatus () == fGeomBoundary && inpcb && pv->GetName().contains("epi") ) 
		{				
// 			histoManager->FillTree("rangetelpcb"+t+".incident", double(rndm_seed), double(event_id), 
// 										prePoint->GetPosition().x(), prePoint->GetPosition().y(), prePoint->GetPosition().z(),
// 										incidentangleX, incidentangleY,
// 										prePoint->GetKineticEnergy(), edep, niel,
// 										pdg, charge, processType);
		}
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
