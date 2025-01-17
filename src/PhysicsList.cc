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

#include "G4RunManager.hh"
#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

#include "G4Version.hh"

// list of all EM physics constructors
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StepLimiter.hh"

// radioactive decay physics
#include "G4DecayPhysics.hh"

// hadronic physics
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"

#if  G4VERSION_NUMBER>=1000
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#else
#include "HadronPhysicsQGSP_BIC.hh"
#include "HadronPhysicsQGSP_BIC_HP.hh"
#include "HadronPhysicsQGSP_BERT.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
#endif

// I think this replaces this, need to check but these are physics lists paganetti uses with TOPAS
#if  G4VERSION_NUMBER>=1000
#include "G4StoppingPhysics.hh"
#else
#include "G4QPhotoNuclearPhysics.hh"
#endif


#include "G4NeutronTrackingCut.hh"

#include "G4LossTableManager.hh"
#include "G4EmConfigurator.hh"
#include "G4UnitsTable.hh"

#include "G4ProcessManager.hh"
#include "G4Decay.hh"

#include "StepMax.hh"

#include "G4IonPhysics.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4UniversalFluctuation.hh"



#include "G4RegionStore.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  G4LossTableManager::Instance();
  defaultCutValue = 0.7*mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;
	cutForProton    = defaultCutValue;
	cutForNeutron   = defaultCutValue;

  helIsRegisted  = false;
  bicIsRegisted  = false;
  biciIsRegisted = false;

  stepMaxProcess  = 0;

  pMessenger = new PhysicsListMessenger(this);

  SetVerboseLevel(1);

  // EM physics
  emName = G4String("emstandard_opt0");  
  emPhysicsList = new G4EmStandardPhysics(1);
	
  // Deacy physics and all particles
  decPhysicsList = new G4DecayPhysics();
	
// 			hadronPhys.push_back( new G4EmExtraPhysics());
// 			hadronPhys.push_back( new G4HadronElasticPhysics());
// 			hadronPhys.push_back( new G4StoppingPhysics());
// 			hadronPhys.push_back( new G4IonBinaryCascadePhysics());
// 			hadronPhys.push_back( new G4NeutronTrackingCut());
// 			hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC());
// 			hadronPhys.push_back( new G4DecayPhysics());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
  delete pMessenger;
  delete emPhysicsList;
  delete decPhysicsList;
  for(size_t i=0; i<hadronPhys.size(); i++) {delete hadronPhys[i];}
  delete stepMaxProcess;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructParticle()
{
  decPhysicsList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
	G4cout << "PhysicsList::Constructing Processes" << G4endl;
	
  // transportation
  //
  AddTransportation();
  
  // electromagnetic physics list
  //
  emPhysicsList->ConstructProcess();

  // decay physics list
  //
  decPhysicsList->ConstructProcess();
  
  // hadronic physics lists
  for(size_t i=0; i<hadronPhys.size(); i++) {
    hadronPhys[i]->ConstructProcess();
  }
  
  // step limitation (as a full process)
  //  
  AddStepMax();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddPhysicsList(const G4String& name)
{
  if (verboseLevel>1) {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }

  if (name == emName) return;

	if (name == "emstandard_opt0") {

    emName = name;
		if(emPhysicsList)
			delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics(1);

  } else if (name == "emstandard_opt1") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option1();

  } else if (name == "emstandard_opt2") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option2();
    
  } else if (name == "emstandard_opt3") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option3();    

  } else if (name == "emlivermore") {
    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmLivermorePhysics();

  } else if (name == "empenelope") {
    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmPenelopePhysics();

  } else if (name == "elastic" && !helIsRegisted) {
    hadronPhys.push_back( new G4HadronElasticPhysics());
    helIsRegisted = true;

  } else if (name == "binary" && !bicIsRegisted) {
    hadronPhys.push_back(new G4HadronInelasticQBBC());
    bicIsRegisted = true;

  } else if (name == "binary_ion" && !biciIsRegisted) {
    hadronPhys.push_back(new G4IonBinaryCascadePhysics());
    biciIsRegisted = true;

  }
		else if (name == "QGSP_BIC_EMY")
    {
			AddPhysicsList("emstandard_opt3");
			hadronPhys.clear();
			hadronPhys.push_back( new G4EmExtraPhysics());
			hadronPhys.push_back( new G4HadronElasticPhysics());
			hadronPhys.push_back( new G4IonBinaryCascadePhysics());
			hadronPhys.push_back( new G4NeutronTrackingCut());
			hadronPhys.push_back( new G4DecayPhysics());
			
#if  G4VERSION_NUMBER>=1000
			hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC());
			hadronPhys.push_back( new G4StoppingPhysics());
#else
			hadronPhys.push_back( new HadronPhysicsQGSP_BIC());
			hadronPhys.push_back( new G4QPhotoNuclearPhysics());
#endif
			
			G4cout <<  "PhysicsList::AddPhysicsList: <" << name << "> set!" << G4endl;
			G4RunManager::GetRunManager() -> PhysicsHasBeenModified();

    }
    else if (name == "QGSP_BERT")
		{
			AddPhysicsList("emstandard_opt3");
			hadronPhys.clear();
			hadronPhys.push_back( new G4EmExtraPhysics());
			hadronPhys.push_back( new G4HadronElasticPhysics() );
			hadronPhys.push_back( new G4IonPhysics());
			hadronPhys.push_back( new G4NeutronTrackingCut());
			

#if  G4VERSION_NUMBER>=1000
			hadronPhys.push_back( new G4HadronPhysicsQGSP_BERT());
			hadronPhys.push_back( new G4StoppingPhysics());
#else
			hadronPhys.push_back( new HadronPhysicsQGSP_BERT());
			hadronPhys.push_back( new G4QPhotoNuclearPhysics());
#endif
			
			G4cout <<  "PhysicsList::AddPhysicsList: <" << name << "> set!" << G4endl;
			G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
		}
    else if (name == "QGSP_BERT_HP")
		{
			AddPhysicsList("emstandard_opt3");
			hadronPhys.clear();
			hadronPhys.push_back( new G4EmExtraPhysics());
			hadronPhys.push_back( new G4HadronElasticPhysicsHP() );
			hadronPhys.push_back( new G4IonPhysics());
			hadronPhys.push_back( new G4NeutronTrackingCut());
			
#if  G4VERSION_NUMBER>=1000
			hadronPhys.push_back( new G4HadronPhysicsQGSP_BERT_HP());
			hadronPhys.push_back( new G4StoppingPhysics());
#else
			hadronPhys.push_back( new HadronPhysicsQGSP_BERT_HP());
			hadronPhys.push_back( new G4QPhotoNuclearPhysics());
#endif
			
			
			G4cout <<  "PhysicsList::AddPhysicsList: <" << name << "> set!" << G4endl;
			
		}
		
// have a physics list which is only selectable for Geant4.9.6.p02 TOPAS version
#if  G4VERSION_NUMBER==962
		else if(name == "TOPAS")
		{
			AddPhysicsList("emstandard_opt3");
			hadronPhys.clear();
			hadronPhys.push_back( new G4HadronElasticPhysicsHP());
			hadronPhys.push_back( new G4DecayPhysics());
			hadronPhys.push_back( new G4IonBinaryCascadePhysics());
			hadronPhys.push_back( new HadronPhysicsQGSP_BIC_HP());
			hadronPhys.push_back( new G4QPhotoNuclearPhysics());
			
			G4cout << "PhysicsList::AddPhysicsList: <" << name << "> set!" << G4endl;
		}	
#endif	

// have a physics list that is only selectable for Geant4.9.2.p04 GATE version
		
    else {

    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }
  
  G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddStepMax()
{
	stepMaxProcess = new StepMax();
  // Step limitation seen as a process
  G4StepLimiter* stepLimiter = new G4StepLimiter();
  ////G4UserSpecialCuts* userCuts = new G4UserSpecialCuts();
  
  theParticleIterator->reset();
  while ((*theParticleIterator)()){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      //if (particle->GetPDGCharge() != 0.0)
        {
          pmanager ->AddDiscreteProcess(stepLimiter);
          ////pmanager ->AddDiscreteProcess(userCuts);
        }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCuts()
{

  if (verboseLevel >0){
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
	SetCutValue(cutForProton, "proton");

	SetAbsorberCuts(defaultCutValue);
	SetWaterTankCuts(defaultCutValue);
	
  if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForGamma(G4double cut)
{
  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForElectron(G4double cut)
{
  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForPositron(G4double cut)
{
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

void PhysicsList::SetCutForProton(G4double cut)
{
  cutForProton = cut;
  SetParticleCuts(cutForProton, G4Proton::Proton());
}

void PhysicsList::SetCutForNeutron(G4double cut)
{
  cutForNeutron = cut;
  SetParticleCuts(cutForNeutron, G4Neutron::Neutron());
}

//*****************************************************************************************
void PhysicsList::SetAbsorberCuts(G4double cut)
//-----------------------------------------------------------------------------------------
{
  G4String regionName = "absorberRegion";
  G4Region* Detregion = G4RegionStore::GetInstance()->GetRegion(regionName);

  G4ProductionCuts* cuts = new G4ProductionCuts ;
  cuts -> SetProductionCut(cut,"gamma");
  cuts -> SetProductionCut(cut,"e-");
  cuts -> SetProductionCut(cut,"e+");

  if(Detregion)
	{
		Detregion -> SetProductionCuts(cuts);
		G4cout << "### absorberRegion gamma production cut = " << G4BestUnit(Detregion->GetProductionCuts()->GetProductionCut("gamma"), "Length") << G4endl;
	}
	else
	{
		G4cout << "### G4Region with name " << regionName << " not found" << G4endl;
	}
	
}

void PhysicsList::SetWaterTankCuts(G4double cut)
//-----------------------------------------------------------------------------------------
{
  G4String regionName = "watertankRegion";
  G4Region* Detregion = G4RegionStore::GetInstance()->GetRegion(regionName);

  G4ProductionCuts* cuts = new G4ProductionCuts ;
  cuts -> SetProductionCut(cut,"gamma");
  cuts -> SetProductionCut(cut,"e-");
  cuts -> SetProductionCut(cut,"e+");
  if(Detregion)
	{
		Detregion -> SetProductionCuts(cuts);
		G4cout << "### watertankRegion gamma production cut = " << G4BestUnit(Detregion->GetProductionCuts()->GetProductionCut("gamma"), "Length") << G4endl;
	}
	else
	{
		G4cout << "### G4Region with name " << regionName << " not found" << G4endl;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

