#ifndef HistoMessenger_h
#define HistoMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoMessenger: public G4UImessenger
{
  public:

   HistoMessenger(HistoManager* );
  ~HistoMessenger();

   void SetNewValue(G4UIcommand* ,G4String );

  private:

   HistoManager*           histoManager;
   
   G4UIdirectory*          histoDir;   
   G4UIcmdWithAString*     factoryCmd;
   G4UIcmdWithAString*     typeCmd;
   G4UIcmdWithAString*     optionCmd;   
   G4UIcommand*            histoCmd;
   G4UIcmdWithAnInteger*   prhistoCmd;   
   G4UIcmdWithAnInteger*   rmhistoCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

