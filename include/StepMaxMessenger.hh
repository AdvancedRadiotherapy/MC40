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
#ifndef StepMaxMessenger_h
#define StepMaxMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class StepMax;
class G4UIcmdWithADoubleAndUnit;

/////////////////////////////////////////////////////////////////////////////
class StepMaxMessenger: public G4UImessenger
{
  public:
    StepMaxMessenger(StepMax*);
   ~StepMaxMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    StepMax* stepMax;
    G4UIcmdWithADoubleAndUnit* StepMaxCmd;
};

#endif
