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
