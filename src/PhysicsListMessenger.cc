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
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: PhysicsListMessenger.cc,v 1.3 2006-06-29 16:58:37 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsListMessenger.hh"

#include "PhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* pPhys)
:pPhysicsList(pPhys)
{
  physDir = new G4UIdirectory("/phys/");
  physDir->SetGuidance("physics list commands");
   
  gammaCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setGammaCut",this);  
  gammaCutCmd->SetGuidance("Set gamma cut.");
  gammaCutCmd->SetParameterName("gcut",false);
  gammaCutCmd->SetUnitCategory("Length");
  gammaCutCmd->SetRange("Gcut>0.0");
  gammaCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  electCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setElectronCut",this);  
  electCutCmd->SetGuidance("Set electron cut.");
  electCutCmd->SetParameterName("ecut",false);
  electCutCmd->SetUnitCategory("Length");
  electCutCmd->SetRange("ecut>0.0");
  electCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  posCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setPositronCut",this);  
  posCutCmd->SetGuidance("Set positron cut.");
  posCutCmd->SetParameterName("pcut",false);
  posCutCmd->SetUnitCategory("Length");
  posCutCmd->SetRange("pcut>0.0");
  posCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
	
	protonCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setProtonCut",this);  
  protonCutCmd->SetGuidance("Set proton cut.");
  protonCutCmd->SetParameterName("Pcut",false);
  protonCutCmd->SetUnitCategory("Length");
  protonCutCmd->SetRange("Pcut>0.0");
  protonCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle); 
	
	neutCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setNeutronCut",this);  
  neutCutCmd->SetGuidance("Set neutron cut.");
  neutCutCmd->SetParameterName("ncut",false);
  neutCutCmd->SetUnitCategory("Length");
  neutCutCmd->SetRange("ncut>0.0");
  neutCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle); 

  allCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setWorldCuts",this);  
  allCutCmd->SetGuidance("Set cut for all.");
  allCutCmd->SetParameterName("cut",false);
  allCutCmd->SetUnitCategory("Length");
  allCutCmd->SetRange("cut>0.0");
  allCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  

	absCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setAbsorberCuts",this);  
  absCutCmd->SetGuidance("Set cut for absorber");
  absCutCmd->SetParameterName("cut",false);
  absCutCmd->SetUnitCategory("Length");
  absCutCmd->SetRange("cut>0.0");
  absCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	wtCutCmd = new G4UIcmdWithADoubleAndUnit("/phys/setWaterTankCuts",this);  
  wtCutCmd->SetGuidance("Set cut for watertank");
  wtCutCmd->SetParameterName("cut",false);
  wtCutCmd->SetUnitCategory("Length");
  wtCutCmd->SetRange("cut>0.0");
  wtCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
  pListCmd = new G4UIcmdWithAString("/phys/addPhysics",this);  
  pListCmd->SetGuidance("Add modula physics list.");
  pListCmd->SetParameterName("PList",false);
  pListCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete gammaCutCmd;
  delete electCutCmd;
	delete posCutCmd;
  delete protonCutCmd;
	delete neutCutCmd;
  delete allCutCmd;
  delete pListCmd;
  delete physDir;    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue)
{       
  if( command == gammaCutCmd )
   { pPhysicsList->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newValue));}
     
  if( command == electCutCmd )
   { pPhysicsList->SetCutForElectron(electCutCmd->GetNewDoubleValue(newValue));}
     
  if( command == posCutCmd )
   { pPhysicsList->SetCutForPositron(posCutCmd->GetNewDoubleValue(newValue));}
   
  if( command == protonCutCmd )
   { pPhysicsList->SetCutForPositron(protonCutCmd->GetNewDoubleValue(newValue));} 

	if( command == neutCutCmd )
   { pPhysicsList->SetCutForPositron(neutCutCmd->GetNewDoubleValue(newValue));}
   
  if( command == allCutCmd )
    {
      G4double cut = allCutCmd->GetNewDoubleValue(newValue);
      pPhysicsList->SetCutForGamma(cut);
      pPhysicsList->SetCutForElectron(cut);
      pPhysicsList->SetCutForPositron(cut);
			pPhysicsList->SetCutForProton(cut);
			pPhysicsList->SetCutForNeutron(cut);
    } 
	if( command == absCutCmd )
	{
		pPhysicsList->SetAbsorberCuts(absCutCmd->GetNewDoubleValue(newValue));
	}
	if( command == wtCutCmd )
	{
		pPhysicsList->SetWaterTankCuts(wtCutCmd->GetNewDoubleValue(newValue));
	}
  if( command == pListCmd )
   { pPhysicsList->AddPhysicsList(newValue);}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
