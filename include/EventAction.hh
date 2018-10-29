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
#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Step.hh"
#include "G4DynamicParticle.hh"
#include "HistoManager.hh"
#include "DetectorConstruction.hh"

class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
  public:
    EventAction(HistoManager* histo, DetectorConstruction* det);
   ~EventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void AddStep(const G4Step* step);
    void AddAbsorberStep(const G4Step* step);
	void AddSensorStep(const G4Step* step);
	void AddAirGapStep(const G4Step* step);
	void AddValidationStep(const G4Step* step);
	
	//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
	void AddBeamPipePosition(G4ThreeVector pos){ beampipe_pos_int.push_back(pos);};
	void ProtonInAbsorber(G4bool b){ pInAbsorber = b; };

		
		
	G4ThreeVector ProjectTrack(G4ThreeVector trajectory, G4double z);
		
	G4double ConvertEDeptoElectrons(G4double);
	G4int			GetEventID();
	G4int			GetRndmSeed();

  private:
  	HistoManager* histoManager;
  	DetectorConstruction* detector;
		
	G4int event_id;
	G4int rndm_seed;
		
	std::vector<G4ThreeVector> beampipe_pos_int;
	G4bool pInAbsorber;
	G4double totalEdepAirGapPerEvent;
  	G4double             totalEDep;
  	G4double             totalNIEL;
		
	G4double             sensorEDEP;
  
  	G4double             absorberEDep;
  	G4double             absorberNIEL;
  	std::vector<const G4Step*> absorberSteps;
  	G4int                nAbsorberIncidentSteps;
  	G4int                nAbsorberExitingSteps;
  	G4double             AbsorberIncidentEnergy;
  	G4double             AbsorberExitingEnergy;
  	
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
