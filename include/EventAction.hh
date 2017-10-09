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
		void ResetDynamite();
    void AddDynamiteStep(const G4Step* step);
		void AddSensorStep(const G4Step* step);
		void EndOfEventDynamiteAction();
		void AddAirGapStep(const G4Step* step);
		void AddDoubleDynamiteStep(const G4Step* step);
		void AddTrackerStep(const G4Step* step);
		void AddRangeTelescopeStep(const G4Step* step);
		void AddValidationStep(const G4Step* step);
		G4ThreeVector CalculateDoubleDynamiteDisplacement();
		G4ThreeVector CalculateDoubleDynamiteDeflection();
		
		//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
		void AddBeamPipePosition(G4ThreeVector pos){ beampipe_pos_int.push_back(pos);};
		void ProtonInAbsorber(G4bool b){ pInAbsorber = b; };

		
		
		G4ThreeVector ProjectTrack(G4ThreeVector trajectory, G4double z);
		
		G4double ConvertEDeptoElectrons(G4double);
		G4int			GetEventID();
		G4int			GetRndmSeed();
		G4int     GetProcessType(G4String processName);
    
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
  	
  	G4double             dynamiteEDep[6];
  	G4double             dynamiteNIEL[6];
  	G4int                nDynamiteIncidentSteps;
		G4int dynamiteSteps;
		
		const G4DynamicParticle*   doubledynamiteProton[2];
		G4ThreeVector        doubleDynamiteProtonMomentum[2];
		std::vector<const G4Step*> doubledynamiteSteps[2];
		G4ThreeVector        doubleDynamiteIncidentPosition[2];
		G4ThreeVector        doubleDynamiteIncidentMomentum[2];
		G4double             doubleDynamiteEnergyDepositedInEpi[2];
		bool                 doubleDynamiteIsAProton[2];
		
		
		G4double							MeVtoElectron;
		std::vector<const G4Step*> trackerSteps[12];
		G4double							trackerEDep[12];
		G4double							trackerNIEL[12];
		
		std::vector<const G4Step*> rangetelSteps[3][30];
		G4double							rangetelEDep[3][30]; // total, epi, sub
		G4double							rangetelNIEL[3][30]; // total, epi, sub
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
