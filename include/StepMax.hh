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
#ifndef StepMax_h
#define StepMax_h 1

#include "globals.hh"
#include "G4VDiscreteProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"

class StepMaxMessenger;

/////////////////////////////////////////////////////////////////////////////
class StepMax : public G4VDiscreteProcess
{
  public:     

     StepMax(const G4String& processName ="UserStepMax");
    ~StepMax();

     G4bool     IsApplicable(const G4ParticleDefinition&);    
     void       SetMaxStep(G4double);
     G4double   GetMaxStep() {return MaxChargedStep;};
     
     G4double   PostStepGetPhysicalInteractionLength( const G4Track& track,
			                     G4double   previousStepSize,
			                     G4ForceCondition* condition);

     G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);

     G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*)
       {return 0.;};     // it is not needed here !

  private:

     G4double    MaxChargedStep;
     StepMaxMessenger* pMess;
};

#endif

