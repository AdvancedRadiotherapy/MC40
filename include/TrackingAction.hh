#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class TrackingAction : public G4UserTrackingAction
{

public:
  TrackingAction(HistoManager* histo);
  virtual ~TrackingAction();

  void PreUserTrackingAction(const G4Track*);
  void PostUserTrackingAction(const G4Track*);

private:

  TrackingAction(const TrackingAction & right);
  const TrackingAction & operator = (const TrackingAction & right);
  
  HistoManager* histoManager;

};

#endif

