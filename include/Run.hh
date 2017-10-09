#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4THitsMap.hh"
#include "G4String.hh"

class Run : public G4Run
{
    public:
      Run(const G4String& detectorName);
      virtual ~Run();
      virtual void RecordEvent(const G4Event*);
      void DumpData() const;
    
    private:
       G4int                 colID;
       std::map<G4int, G4THitsMap<G4double>* > fMap;
       void Print(const std::vector<G4String>& title, const std::map< G4int, std::vector<G4double> >&myMap) const;
       
   
   public:
    //  ---- access methods ---- Data members for hits collection IDs and ............
};

#endif
