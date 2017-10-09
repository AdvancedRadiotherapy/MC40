#include "StepMax.hh"
#include "StepMaxMessenger.hh"

/////////////////////////////////////////////////////////////////////////////
StepMax::StepMax(const G4String& processName)
 : G4VDiscreteProcess(processName),MaxChargedStep(DBL_MAX)
{
  pMess = new StepMaxMessenger(this);
}
 
/////////////////////////////////////////////////////////////////////////////
StepMax::~StepMax() { delete pMess; }

/////////////////////////////////////////////////////////////////////////////
G4bool StepMax::IsApplicable(const G4ParticleDefinition& particle) 
{ 
  return (particle.GetPDGCharge() != 0.);
}

/////////////////////////////////////////////////////////////////////////////    
void StepMax::SetMaxStep(G4double step) {MaxChargedStep = step;}

/////////////////////////////////////////////////////////////////////////////
G4double StepMax::PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
                                                  G4double,
                                                  G4ForceCondition* condition )
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  
  G4double ProposedStep = DBL_MAX;

  if((MaxChargedStep > 0.) &&
     (aTrack.GetVolume() != 0) &&
     (aTrack.GetVolume()->GetName() == "experimentalHall"))
     ProposedStep = MaxChargedStep;

  return ProposedStep;
}

/////////////////////////////////////////////////////////////////////////////
G4VParticleChange* StepMax::PostStepDoIt(const G4Track& aTrack, const G4Step&)
{
   // do nothing
   aParticleChange.Initialize(aTrack);
   return &aParticleChange;
}

