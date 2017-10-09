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
//
// $Id: ExN02PrimaryGeneratorAction.cc,v 1.7 2006-06-29 17:48:13 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(
                                               DetectorConstruction* myDC)
:myDetector(myDC)
{
  particleGun = new G4GeneralParticleSource();

// default particle

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("proton");
  
  particleGun->SetParticleDefinition(particle);
  //particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  //particleGun->SetParticleEnergy(3.0*GeV);
  //particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,-30*cm));
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  //G4double position = -0.5*(myDetector->GetWorldFullLength());
  //particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,-15*cm));
  
  
  //G4double x = G4RandGauss::shoot(0.0*cm,2.0*cm);
  //G4double y = G4RandGauss::shoot(0.0*cm,2.0*cm);
  
  // Get the original positions of hits
  G4double x0 = particleGun->GetParticlePosition().x();
  G4double y0 = particleGun->GetParticlePosition().y();
  G4double z0 = particleGun->GetParticlePosition().z();
  
  //G4cout << "PrimaryGeneratorAction::SetParticlePosition = (" 
	//			<< G4BestUnit(x0,"Length") 
	//			<< ", " << G4BestUnit(y0,"Length") 
	//			<< ", " << G4BestUnit(z0,"Length") 
	//			<< ")" << G4endl;
	
  //G4ThreeVector fPosition = G4ThreeVector(x,y,z0);
  //particleGun->SetParticlePosition(fPosition);

  particleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

