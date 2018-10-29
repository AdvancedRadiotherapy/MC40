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
*///
#include "StepMaxMessenger.hh"
#include "StepMax.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

/////////////////////////////////////////////////////////////////////////////
StepMaxMessenger::StepMaxMessenger(StepMax* stepM)
:stepMax(stepM)
{ 
  StepMaxCmd = new G4UIcmdWithADoubleAndUnit("/Step/waterPhantomStepMax",this);
  StepMaxCmd->SetGuidance("Set max allowed step length");
  StepMaxCmd->SetParameterName("mxStep",false);
  StepMaxCmd->SetRange("mxStep>0.");
  StepMaxCmd->SetUnitCategory("Length");
}

/////////////////////////////////////////////////////////////////////////////
StepMaxMessenger::~StepMaxMessenger()
{
  delete StepMaxCmd;
}

/////////////////////////////////////////////////////////////////////////////
void StepMaxMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if (command == StepMaxCmd)
    { stepMax->SetMaxStep(StepMaxCmd->GetNewDoubleValue(newValue));}
}

