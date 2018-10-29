/*
    MC40 Cyclotron Model
    Copyright (C) 2017  Tony Price

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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
    G4double                CalculateAngleX(G4StepPoint* point);
    G4double                CalculateAngleY(G4StepPoint* point);
   
  private:
  	DetectorConstruction*   detector;
  	RunAction*              runAction;
  	HistoManager*	    	histoManager;
  	EventAction*            eventAction;
  	
  	G4double absorber_eDep;      // total energy deposited per event in the absorber
  	G4int verboseLevel;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
