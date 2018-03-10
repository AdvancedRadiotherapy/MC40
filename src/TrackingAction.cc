#include "TrackingAction.hh"

#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

TrackingAction::TrackingAction(HistoManager* histo): histoManager(histo)
{
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

TrackingAction::~TrackingAction()
{
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
    if(aTrack->GetDynamicParticle()->GetPDGcode()==22) {
        G4cout << "New track #"
           << aTrack->GetTrackID() << " of " 
           << aTrack->GetParticleDefinition()->GetParticleName()
           << " Ekin(MeV)= " << aTrack->GetKineticEnergy()/MeV
           << " parent # " << aTrack->GetParentID()
           << G4endl;
             
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void TrackingAction::PostUserTrackingAction(const G4Track*)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
