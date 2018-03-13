#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3VectorAndUnit;
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstructionMessenger: public G4UImessenger
{
  public:
  
    DetectorConstructionMessenger(DetectorConstruction* );
   ~DetectorConstructionMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
  
    DetectorConstruction* pDetectorConstruction;
      
    G4UIdirectory*             detDir;        
    G4UIcmdWithADoubleAndUnit* absDepthCmd; 
    G4UIcmdWithADoubleAndUnit* colInnerRadiusCmd;
    G4UIcmdWithoutParameter*   UpdateCmd;
    G4UIcmdWithAString*        absMaterialCmd;
    G4UIcmdWithABool*					wtConstructCmd;
	G4UIcmdWithADoubleAndUnit*	wtWindowThicknessCmd;
	G4UIcmdWithADoubleAndUnit* wtSensorPositionCmd;
	
	G4UIcmdWithABool*          scatteringVolumeCmd;
    G4UIcmdWith3VectorAndUnit* filmSizeCmd;
    G4UIcmdWith3VectorAndUnit* filmPositionCmd;
	G4UIcmdWithAString*        filmTypeCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
