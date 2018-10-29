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
