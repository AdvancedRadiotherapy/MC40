#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "HistoManager.hh"
#include "EventAction.hh"

#include "G4ProcessManager.hh"

#define PI 3.14159265

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(DetectorConstruction*, RunAction*, HistoManager*, EventAction*, G4int verbosity=0);
   ~SteppingAction(){};

    void UserSteppingAction(const G4Step*);
    G4double CalculateAngleX(G4StepPoint* point);
    G4double CalculateAngleY(G4StepPoint* point);
    bool     IsInDynamite(G4String);
		bool			IsInTracker(G4String);
		bool			IsInRangeTelescope(G4String);
		G4int GetProcessType(G4String);
    
  private:
  	DetectorConstruction* detector;
  	RunAction*            runAction;
  	HistoManager*					histoManager;
  	EventAction*          eventAction;
  	
  	G4double absorber_eDep;      // total energy deposited per event in the absorber
  	G4int verboseLevel;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
