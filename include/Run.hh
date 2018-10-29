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
