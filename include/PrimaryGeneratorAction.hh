#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class DetectorConstruction;
class G4GeneralParticleSource;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(DetectorConstruction*);    
   ~PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event*);

  private:
    G4GeneralParticleSource* particleGun;
    DetectorConstruction* myDetector;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


