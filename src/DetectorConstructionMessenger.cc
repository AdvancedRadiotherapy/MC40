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
  detDir = new G4UIdirectory("/det/");
  detDir->SetGuidance("detector construction commands");
  
  absDepthCmd = new G4UIcmdWithADoubleAndUnit("/det/setAbsorberDepth", this);
  absDepthCmd->SetGuidance("Set depth of absorber after air chamber");
  absDepthCmd->SetParameterName("AbsDepth", false);
  absDepthCmd->SetUnitCategory("Length");
  absDepthCmd->SetRange("AbsDepth>=0.0");
  absDepthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  absMaterialCmd = new G4UIcmdWithAString("/det/setAbsorberMaterial", this);
  absMaterialCmd->SetGuidance("Set the material of absorber after air chamber");
	absMaterialCmd->SetDefaultValue("Perspex");
  absMaterialCmd->SetParameterName("AbsMaterial", false);
  absMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  colInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/det/setCollimatorInnerRadius", this);
  colInnerRadiusCmd->SetGuidance("Set the inner radius of the collimator at the end of the beampipe");
  colInnerRadiusCmd->SetParameterName("ColInnerRadius", false);
  colInnerRadiusCmd->SetUnitCategory("Length");
  colInnerRadiusCmd->SetRange("ColInnerRadius>=0.0");
  colInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  dynConstructCmd = new G4UIcmdWithABool("/det/setDynamiteConstruction", this);
  dynConstructCmd->SetGuidance("Set the flag to build the dynamite detector");
  dynConstructCmd->SetParameterName("DynamiteConstruction", false);
  dynConstructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	dynPosCmd = new G4UIcmdWith3VectorAndUnit("/det/setDynamitePosition",this);
	dynPosCmd -> SetGuidance("Set the position of the first dynamite sensor"); 
	dynPosCmd -> SetParameterName("DisplacementAlongX","DisplacementAlongY","DisplacementAlongZ", false);
  dynPosCmd -> SetDefaultUnit("mm");
  dynPosCmd -> SetUnitCategory("Length"); 
  dynPosCmd -> AvailableForStates(G4State_PreInit,G4State_Idle);
	
	dynBackDepthCmd = new G4UIcmdWithADoubleAndUnit("/det/setDynamiteBackingDepth", this);
	dynBackDepthCmd->SetGuidance("Set the backing thickness of dynamite");
	dynBackDepthCmd->SetParameterName("DynamiteBackingDepth", false);
	dynBackDepthCmd->SetUnitCategory("Length");
	dynBackDepthCmd->SetRange("DynamiteBackingDepth>=0.0");
	dynBackDepthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	doubdynConstructCmd = new G4UIcmdWithABool("/det/setDoubleDynamiteConstruction", this);
  doubdynConstructCmd->SetGuidance("Set the flag to build the double dynamite detector");
  doubdynConstructCmd->SetParameterName("DoubleDynamiteConstruction", false);
  doubdynConstructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	doubdynSeperationCmd = new G4UIcmdWithADoubleAndUnit("/det/setDoubleDynamiteSeperation", this);
	doubdynSeperationCmd->SetGuidance("Set the seperation between the two dynamite sensors");
	doubdynSeperationCmd->SetParameterName("DoubleDynamiteSeperation", false);
	doubdynSeperationCmd->SetUnitCategory("Length");
	doubdynSeperationCmd->SetRange("DoubleDynamiteSeperation>=0.0");
	doubdynSeperationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	doubdynConstructCollimatorCmd = new G4UIcmdWithABool("/det/setDoubleDynamiteCollimatorConstruction", this);
  doubdynConstructCollimatorCmd->SetGuidance("Set the flag to build the double dynamite detector collimator");
  doubdynConstructCollimatorCmd->SetParameterName("DoubleDynamiteCollimatorConstruction", false);
  doubdynConstructCollimatorCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	doubdynCollimatorRadiusCmd = new G4UIcmdWithADoubleAndUnit("/det/setDoubleDynamiteCollimatorRadius", this);
	doubdynCollimatorRadiusCmd->SetGuidance("Set the radius of circular collimator");
	doubdynCollimatorRadiusCmd->SetParameterName("DoubleDynamiteCollimatorRadius", false);
	doubdynCollimatorRadiusCmd->SetUnitCategory("Length");
	doubdynCollimatorRadiusCmd->SetRange("DoubleDynamiteCollimatorRadius>0.0 && DoubleDynamiteCollimatorRadius<5.0");
	doubdynCollimatorRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	doubdynCollimatorThicknessCmd = new G4UIcmdWithADoubleAndUnit("/det/setDoubleDynamiteCollimatorThickness", this);
	doubdynCollimatorThicknessCmd->SetGuidance("Set the thickness of the collimator");
	doubdynCollimatorThicknessCmd->SetParameterName("DoubleDynamiteCollimatorThickness", false);
	doubdynCollimatorThicknessCmd->SetUnitCategory("Length");
	doubdynCollimatorThicknessCmd->SetRange("DoubleDynamiteCollimatorThickness>0.0");
	doubdynCollimatorThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);	
	
	doubdynCollimatorMaterialCmd = new G4UIcmdWithAString("/det/setDoubleDynamiteCollimatorMaterial", this);
  doubdynCollimatorMaterialCmd->SetGuidance("Set the material of the collimator in double dynamite system");
	doubdynCollimatorMaterialCmd->SetDefaultValue("Al");
  doubdynCollimatorMaterialCmd->SetParameterName("DoubleDynamiteCollimatorMaterial", false);
  doubdynCollimatorMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	doubdynCollimatorPositionCmd = new G4UIcmdWith3VectorAndUnit("/det/setDoubleDynamiteCollimatorPosition",this);
  doubdynCollimatorPositionCmd->SetGuidance("Set the position of the double dynamite collimator");
  doubdynCollimatorPositionCmd->SetParameterName("DoublDynamitePositionX", "DoublDynamitePosition/y", "DoublDynamitePositionZ", false);
  doubdynCollimatorPositionCmd->SetDefaultUnit("mm");
  doubdynCollimatorPositionCmd->SetUnitCandidates("nm um mm cm m");
  doubdynCollimatorPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	doubdynCollimatorRotationCmd = new G4UIcmdWith3VectorAndUnit("/det/setDoubleDynamiteCollimatorRotation",this);
  doubdynCollimatorRotationCmd->SetGuidance("Set the rotation of the double dynamite collimator");
  doubdynCollimatorRotationCmd->SetParameterName("DoublDynamiteRotationX", "DoublDynamiteRotationY", "DoublDynamiteRotationZ", false);
  doubdynCollimatorRotationCmd->SetDefaultUnit("rad");
  doubdynCollimatorRotationCmd->SetUnitCandidates("rad deg");
  doubdynCollimatorPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  trkrConstructCmd = new G4UIcmdWithABool("/det/setTrackerConstruction", this);
  trkrConstructCmd->SetGuidance("Set the flag to build the strip trackers");
  trkrConstructCmd->SetParameterName("TrackerConstruction", false);
  trkrConstructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	trkrNSensorsCmd = new G4UIcmdWithAnInteger("/det/setNTrackers", this);
	trkrNSensorsCmd->SetGuidance("Set the number of trackers to use (max 12)");
	trkrNSensorsCmd->SetParameterName("nTrackers", false);
  trkrNSensorsCmd->SetRange("nTrackers<=12");
	
	rtConstructCmd = new G4UIcmdWithABool("/det/setRangeTelescopeConstruction", this);
  rtConstructCmd->SetGuidance("Set the flag to build the range telescope");
  rtConstructCmd->SetParameterName("RangeTelescopeConstruction", false);
  rtConstructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	rtNLayersCmd = new G4UIcmdWithAnInteger("/det/setRangeTelescopeNLayers", this);
	rtNLayersCmd->SetGuidance("Set the number of layers to use in range telescope (max 30)");
	rtNLayersCmd->SetParameterName("nRangeTelescopeLayers", false);
  rtNLayersCmd->SetRange("nRangeTelescopeLayers<=30");
	
	rtSetLayerSeperationCmd = new G4UIcmdWithADoubleAndUnit("/det/setRangeTelescopeLayerSeperation", this);
	rtSetLayerSeperationCmd->SetGuidance("Set the absolute seperation between each layer in the range telescope");
	rtSetLayerSeperationCmd->SetParameterName("RangeTelescopeLayerSeperation", false);
	rtSetLayerSeperationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	rtSetLayerSeperationCmd->SetUnitCategory("Length");
	
	rtSetAbsorberThicknessCmd = new G4UIcmdWithADoubleAndUnit("/det/setRangeTelescopeAbsorberThickness", this);
	rtSetAbsorberThicknessCmd->SetGuidance("Set the thickness of the material between the layers in the range telescope");
	rtSetAbsorberThicknessCmd->SetParameterName("RangeTelescopeAbsorberThickness", false);
	rtSetAbsorberThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	rtSetAbsorberThicknessCmd->SetUnitCategory("Length");
	//rtSetAbsorberThicknessCmd->SetRange("RangeTelescopeAbsorberThickness<=1 cm");
	
	rtSetAbsorberMaterialCmd = new G4UIcmdWithAString("/det/setRangeTelescopeAbsorberMaterial", this);
  rtSetAbsorberMaterialCmd->SetGuidance("Set the material of absorbers between the range telescope layers");
	rtSetAbsorberMaterialCmd->SetDefaultValue("Perspex");
  rtSetAbsorberMaterialCmd->SetParameterName("RangeTelescopeAbsorberMaterial", false);
  rtSetAbsorberMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);;
	
	wtConstructCmd = new G4UIcmdWithABool("/det/setWaterTankConstruction", this);
	wtConstructCmd->SetDefaultValue(false);
	wtConstructCmd->SetGuidance("Set the flag to construct water tank around the absorber");
	wtConstructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	wtWindowThicknessCmd = new G4UIcmdWithADoubleAndUnit("/det/setWaterTankWindowThickness", this);
	wtWindowThicknessCmd->SetGuidance("Set the thickness of the window in the water tank");
	wtWindowThicknessCmd->SetParameterName("WaterTankWindowThickness", false);
	wtWindowThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	wtWindowThicknessCmd->SetUnitCategory("Length");
	
	wtSensorPositionCmd = new G4UIcmdWithADoubleAndUnit("/det/setWaterTankSensorPosition", this);
	wtSensorPositionCmd->SetParameterName("WaterTankSensorPos", false);
	wtSensorPositionCmd->AvailableForStates(G4State_Idle, G4State_PreInit);
	wtSensorPositionCmd->SetUnitCategory("Length");
   
	
	scattererColRadiusCmd = new G4UIcmdWithADoubleAndUnit("/det/setScatteringCollimatorRadius", this);
	scattererColRadiusCmd->SetParameterName("ScatteringCollimatorRadius", false);
  scattererColRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	scattererColRadiusCmd->SetUnitCategory("Length");
	
  scatteringMaterialCmd = new G4UIcmdWithAString("/det/setScatteringMaterial", this);
  scatteringMaterialCmd->SetGuidance("Set the material of scattering system");
  scatteringMaterialCmd->SetDefaultValue("Copper");
  scatteringMaterialCmd->SetParameterName("ScatteringMaterial", false);
  scatteringMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	scatteringVolumeCmd = new G4UIcmdWithABool("/det/setScatteringInsideVacuum", this);
	//scatteringVolumeCmd->SetDefaultValue("False");
	scatteringVolumeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
  firstScattererSizeCmd = new G4UIcmdWith3VectorAndUnit("/det/setFirstScattererSize",this);
  firstScattererSizeCmd->SetGuidance("Set the size of the first scattering material");
  firstScattererSizeCmd->SetParameterName("ScattererInnerRadius", "ScattererOuterRadius", "ScattererThickness", false);
  firstScattererSizeCmd->SetDefaultUnit("mm");
  firstScattererSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);	
	
	firstScattererThicknessCmd = new G4UIcmdWithADoubleAndUnit("/det/setFirstScattererThickness",this);
  firstScattererThicknessCmd->SetGuidance("Set the thickness of the first scattering material");
  firstScattererThicknessCmd->SetParameterName("ScattererThickness", false);
  firstScattererThicknessCmd->SetDefaultUnit("mm");
  firstScattererThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);	
	
	firstScattererPositionCmd = new G4UIcmdWith3VectorAndUnit("/det/setFirstScattererPosition",this);
  firstScattererPositionCmd->SetGuidance("Set the position of the first scattering material");
  firstScattererPositionCmd->SetParameterName("ScattererPosX", "ScattererPosY", "ScattererPosZ", false);
  firstScattererPositionCmd->SetDefaultUnit("mm");
  firstScattererPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);	
  
	filmTypeCmd = new G4UIcmdWithAString("/det/setGaFChromicFilmType", this);
	filmTypeCmd->SetGuidance("Choose the type of film being used. Default Value is \"None\"");
	filmTypeCmd->SetParameterName("GaFType",false);
	filmTypeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
  filmSizeCmd = new G4UIcmdWith3VectorAndUnit("/det/setGaFChromicFilmSize",this);
  filmSizeCmd->SetGuidance("Set the size of the GaFChromic Film");
  filmSizeCmd->SetParameterName("GaFSizeX", "GaFSizeY", "GaFSizeZ", false);
  filmSizeCmd->SetDefaultUnit("mm");
  filmSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	filmPositionCmd = new G4UIcmdWith3VectorAndUnit("/det/setGaFChromicFilmPosition",this);
  filmPositionCmd->SetGuidance("Set the position of the GaFChromic Film");
  filmPositionCmd->SetParameterName("GaFPosX", "GaFPosY", "GaFPosZ", false);
  filmPositionCmd->SetDefaultUnit("mm");
  filmPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
  UpdateCmd = new G4UIcmdWithoutParameter("/det/update",this);
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
	if( command == scattererColRadiusCmd )
		pDetectorConstruction->SetScattererCollimatorRadius(scattererColRadiusCmd->GetNewDoubleValue(newValue));
  if( command == scatteringMaterialCmd )
    pDetectorConstruction->SetScatteringMaterial(newValue);
	if( command == scatteringVolumeCmd )
		pDetectorConstruction->SetScattererInsideVacuum(scatteringVolumeCmd->GetNewBoolValue(newValue));
  if(  command == firstScattererSizeCmd )
    pDetectorConstruction->SetFirstScattererSize(firstScattererSizeCmd->GetNew3VectorValue(newValue));
	if( command == firstScattererThicknessCmd)
		pDetectorConstruction->SetFirstScattererThickness(firstScattererThicknessCmd->GetNewDoubleValue(newValue));
	if(  command == firstScattererPositionCmd )
		pDetectorConstruction->SetFirstScattererPosition(firstScattererSizeCmd->GetNew3VectorValue(newValue));	
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
  else if( command == dynConstructCmd )
  {
  	pDetectorConstruction->SetDynamiteConstruction(dynConstructCmd->GetNewBoolValue(newValue));
  }
  else if( command == dynPosCmd )
	{
		G4ThreeVector pos = dynPosCmd->GetNew3VectorValue(newValue);
		pDetectorConstruction->SetDynamitePosition(pos.getX(),pos.getY(),pos.getZ());
	}
  else if( command == dynBackDepthCmd )
  {
  	pDetectorConstruction->SetDynamiteBackingDepth(dynBackDepthCmd->GetNewDoubleValue(newValue));
	}
  else if( command == doubdynConstructCmd )
  {
  	pDetectorConstruction->SetDoubleDynamiteConstruction(doubdynConstructCmd->GetNewBoolValue(newValue));
  }
  else if( command == doubdynSeperationCmd )
  {
  	pDetectorConstruction->SetDoubleDynamiteSeperation(doubdynSeperationCmd->GetNewDoubleValue(newValue));
  }
  else if( command == doubdynConstructCollimatorCmd )
		pDetectorConstruction->SetDoubleDynamiteCollimatorConstruction(doubdynConstructCollimatorCmd->GetNewBoolValue(newValue));
	else if( command == doubdynCollimatorThicknessCmd )
		pDetectorConstruction->SetDoubleDynamiteCollimatorThickness(doubdynCollimatorThicknessCmd->GetNewDoubleValue(newValue));
	else if( command == doubdynCollimatorMaterialCmd )
		pDetectorConstruction->SetDoubleDynamiteCollimatorMaterial(newValue);
	else if( command == doubdynCollimatorPositionCmd )
		pDetectorConstruction->SetDoubleDynamiteCollimatorPosition(doubdynCollimatorPositionCmd->GetNew3VectorValue(newValue));
	else if( command == doubdynCollimatorRadiusCmd )
		pDetectorConstruction->SetDoubleDynamiteCollimatorRadius(doubdynCollimatorRadiusCmd->GetNewDoubleValue(newValue));
	else if( command == doubdynCollimatorRotationCmd )
		pDetectorConstruction->SetDoubleDynamiteCollimatorRotation(doubdynCollimatorPositionCmd->GetNew3VectorValue(newValue));
  else if( command == trkrConstructCmd )
  {
  	pDetectorConstruction->SetTrackerConstruction(trkrConstructCmd->GetNewBoolValue(newValue));
  }
  else if( command == trkrNSensorsCmd )
	{
		pDetectorConstruction->SetNTrackers(trkrNSensorsCmd->GetNewIntValue(newValue));
	}
	else if( command == rtConstructCmd )
  {
  	pDetectorConstruction->SetRangeTelescopeConstruction(rtConstructCmd->GetNewBoolValue(newValue));
  }
  else if( command == rtNLayersCmd )
	{
		pDetectorConstruction->SetNRangeTelescopeLayers(rtNLayersCmd->GetNewIntValue(newValue));
	}
	else if( command == rtSetLayerSeperationCmd )
	{
		pDetectorConstruction->SetRangeTelescopeLayersSeperation(rtSetLayerSeperationCmd->GetNewDoubleValue(newValue));
	}
	else if( command == rtSetAbsorberThicknessCmd )
	{
		pDetectorConstruction->SetRangeTelescopeAbsorberThickness(rtSetAbsorberThicknessCmd->GetNewDoubleValue(newValue));
	}
	else if( command == rtSetAbsorberMaterialCmd )
	{
		pDetectorConstruction->SetRangeTelescopeAbsorberMaterial(newValue);
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
