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
		G4cout << "Event " << event_id << G4endl;
	
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
 
	histoManager->FillHisto("total.EDep(PerEvent)", totalEDep, 1);
	histoManager->FillHisto("total.NIEL(PerEvent)", totalNIEL, 1);

  histoManager->FillHisto("absorber.EDep(PerEvent)", absorberEDep, 1);
  histoManager->FillHisto("absorber.NIEL(PerEvent)", absorberNIEL, 1);
	
	if(sensorEDEP>0)
		histoManager->FillHisto("sensor_WT_edep",sensorEDEP,1);

	

}
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
	G4int    	processType  = 0;//GetProcessType(processName);
	
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

