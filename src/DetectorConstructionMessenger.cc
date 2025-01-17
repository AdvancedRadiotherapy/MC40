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
#include "DetectorConstructionMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

/////////////////////////////////////////////////////////////////////////////
DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* Det)
:pDetectorConstruction(Det)
{  
  detDir = new G4UIdirectory("/MC40/");
  detDir->SetGuidance("detector construction commands");
  
  absDepthCmd = new G4UIcmdWithADoubleAndUnit("/MC40/setAbsorberDepth", this);
  absDepthCmd->SetGuidance("Set depth of absorber after air chamber");
  absDepthCmd->SetParameterName("AbsDepth", false);
  absDepthCmd->SetUnitCategory("Length");
  absDepthCmd->SetRange("AbsDepth>=0.0");
  absDepthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  absMaterialCmd = new G4UIcmdWithAString("/MC40/setAbsorberMaterial", this);
  absMaterialCmd->SetGuidance("Set the material of absorber after air chamber");
	absMaterialCmd->SetDefaultValue("Perspex");
  absMaterialCmd->SetParameterName("AbsMaterial", false);
  absMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  colInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/MC40/setCollimatorInnerRadius", this);
  colInnerRadiusCmd->SetGuidance("Set the inner radius of the collimator at the end of the beampipe");
  colInnerRadiusCmd->SetParameterName("ColInnerRadius", false);
  colInnerRadiusCmd->SetUnitCategory("Length");
  colInnerRadiusCmd->SetRange("ColInnerRadius>=0.0");
  colInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
wtConstructCmd = new G4UIcmdWithABool("/MC40/setWaterTankConstruction", this);
	wtConstructCmd->SetDefaultValue(false);
	wtConstructCmd->SetGuidance("Set the flag to construct water tank around the absorber");
	wtConstructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	wtWindowThicknessCmd = new G4UIcmdWithADoubleAndUnit("/MC40/setWaterTankWindowThickness", this);
	wtWindowThicknessCmd->SetGuidance("Set the thickness of the window in the water tank");
	wtWindowThicknessCmd->SetParameterName("WaterTankWindowThickness", false);
	wtWindowThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	wtWindowThicknessCmd->SetUnitCategory("Length");
	
	wtSensorPositionCmd = new G4UIcmdWithADoubleAndUnit("/MC40/setWaterTankSensorPosition", this);
	wtSensorPositionCmd->SetParameterName("WaterTankSensorPos", false);
	wtSensorPositionCmd->AvailableForStates(G4State_Idle, G4State_PreInit);
	wtSensorPositionCmd->SetUnitCategory("Length");
   

	scatteringVolumeCmd = new G4UIcmdWithABool("/MC40/insertScatteringFoil", this);
	scatteringVolumeCmd->SetDefaultValue("False");
	scatteringVolumeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


filmTypeCmd = new G4UIcmdWithAString("/MC40/setGaFChromicFilmType", this);
	filmTypeCmd->SetGuidance("Choose the type of film being used. Default Value is \"None\"");
	filmTypeCmd->SetParameterName("GaFType",false);
	filmTypeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
  filmSizeCmd = new G4UIcmdWith3VectorAndUnit("/MC40/setGaFChromicFilmSize",this);
  filmSizeCmd->SetGuidance("Set the size of the GaFChromic Film");
  filmSizeCmd->SetParameterName("GaFSizeX", "GaFSizeY", "GaFSizeZ", false);
  filmSizeCmd->SetDefaultUnit("mm");
  filmSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	filmPositionCmd = new G4UIcmdWith3VectorAndUnit("/MC40/setGaFChromicFilmPosition",this);
  filmPositionCmd->SetGuidance("Set the position of the GaFChromic Film");
  filmPositionCmd->SetParameterName("GaFPosX", "GaFPosY", "GaFPosZ", false);
  filmPositionCmd->SetDefaultUnit("mm");
  filmPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
  UpdateCmd = new G4UIcmdWithoutParameter("/MC40/update",this);
  UpdateCmd->SetGuidance("Update calorimeter geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_PreInit,G4State_Idle); 
}

/////////////////////////////////////////////////////////////////////////////
DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
  delete absDepthCmd;
}

/////////////////////////////////////////////////////////////////////////////
void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{   
if( command == scatteringVolumeCmd )
		pDetectorConstruction->SetScatteringSystemConstruction(scatteringVolumeCmd->GetNewBoolValue(newValue));

 if(  command == filmSizeCmd )
    pDetectorConstruction->SetGaFChromicFilmSize(filmSizeCmd->GetNew3VectorValue(newValue));

	if(  command == filmPositionCmd )
    pDetectorConstruction->SetGaFChromicFilmPosition(filmSizeCmd->GetNew3VectorValue(newValue));

 if( command == filmTypeCmd )
		pDetectorConstruction->SetGaFChromicFilmType(newValue);
  
  if( command == absDepthCmd )
  { 
  	pDetectorConstruction->SetAbsDepth(absDepthCmd->GetNewDoubleValue(newValue));
  }
  else if( command == absMaterialCmd )
  {
  	pDetectorConstruction->SetAbsMaterial(newValue);
  }
  else if( command == colInnerRadiusCmd )
  {
  	pDetectorConstruction->SetCollimatorInnerRadius(colInnerRadiusCmd->GetNewDoubleValue(newValue));
  }
else if( command == wtConstructCmd )
	{
		pDetectorConstruction->SetWaterTankConstruction(wtConstructCmd->GetNewBoolValue(newValue));
	}
	else if( command == wtWindowThicknessCmd )
	{
		pDetectorConstruction->SetWaterTankWindowThickness(wtWindowThicknessCmd->GetNewDoubleValue(newValue));
	}
	
	else if( command ==  wtSensorPositionCmd)
		pDetectorConstruction->SetWaterTankSensorPosition(wtSensorPositionCmd->GetNewDoubleValue(newValue));
	
  else if( command == UpdateCmd )
  { 
  	pDetectorConstruction->UpdateGeometry(); 
  }
}
