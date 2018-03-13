#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;
#include "DetectorConstruction.hh"
#include "HistoManager.hh"
#include "PhysicsList.hh"

class RunAction : public G4UserRunAction
{
  public:
    RunAction(DetectorConstruction* det, HistoManager* histo, PhysicsList* phys);
   ~RunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void FillEdep(G4double de, G4double eni);
   // G4Run* GenerateRun();
    
  private:
  	G4double edeptot; 
	G4double eniel;
	DetectorConstruction* detector;
	HistoManager*         histoManager;
	PhysicsList*          physics;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
