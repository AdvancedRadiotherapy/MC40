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
#include "RunAction.hh"
#include "Run.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(DetectorConstruction* det, HistoManager* histo, PhysicsList* phys)
: detector(det), histoManager(histo), physics(phys)
{
	edeptot=0;
	eniel=0;
}

RunAction::~RunAction()
{}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
	
	histoManager->SetHisto("sensor_WT_edep","",100,0,10);
  
    // 1D Histograms
    // =============
    //
    // define histograms for the whole system
    histoManager->SetHisto("total.EDep(PerEvent)", "Energy deposited in the whole system per event; Energy Deposited [MeV]; Count", 200, 0*MeV, 50*MeV, "MeV");
    histoManager->SetTree("generated","generated particle properties");
  
 
    histoManager->SetHisto2D("generated.XY", "Scatter plot of generated particle positions; x [mm]; y [mm]",
  												100, -25*mm, 25*mm,
  												100, -25*mm, 25*mm,
  												"mm", "mm");
	
	histoManager->SetHisto2D("airmonitor.XY", "Scatter plot of particles into air monitor; x [mm]; y [mm]",
  												100, -25*mm, 25*mm,
  												100, -25*mm, 25*mm,
  												"mm", "mm");

    histoManager->SetHisto2D("absorber.XY", "Scatter plot of particles into absorber; x [mm]; y [mm]",
            100,-25*mm,25*mm,100,-25*mm,25*mm, "mm", "mm");

    histoManager->SetHisto("TotalEdepInAirGap", "",2000, 0.*eV, 100000*eV, "eV");
	
	histoManager->SetHisto("BeamPipeInteractions_Zpos","",500,-5000*mm,0*mm,"mm");
	histoManager->SetHisto("BeamPipeInteractions_Zpos_pInAbsorber","",500,-5000*mm,0*mm,"mm");
	histoManager->SetHisto("BeamPipeInteractions_Xpos_pInAbsorber","",100,-50*mm,50*mm,"mm");
	histoManager->SetHisto("BeamPipeInteractions_Ypos_pInAbsorber","",100,-50*mm,50*mm,"mm");
	
    // define histograms for the absorber binning
    G4double length  = detector->GetAbsorberDepth();
    G4double stepMax = 10*um;//physics->GetStepMaxProcess()->GetMaxStep();
    G4int nbmin = 400;
    G4int nbBins = (G4int)(0.5 + length/stepMax);
    if (nbBins < nbmin) 
        nbBins = nbmin;
    histoManager->SetHisto("absorber.dEdx", "Energy deposited in absorber as function of depth; Depth in Absorber [mm]",nbBins, 0., length, "mm");
    histoManager->SetHisto("absorber.dEdx.Marcus", "Energy deposited in absorber as function of depth; Depth in Absorber [mm]",nbBins, 0., length, "mm");
    histoManager->SetHisto("absorber.incident.angleX", "Incident angle of particles into absorber (x co-ord); Angle [degrees]", 100, -5, 5);
    histoManager->SetHisto("absorber.incident.angleY", "Incident angle of particles into absorber (y co-ord); Angle [degrees]", 100, -5, 5);
	histoManager->SetHisto("absorber.incident.Energy", "Energy of the incident particles into the absorber; Energy [MeV]", 200, 0*MeV, 50*MeV, "MeV");
   histoManager->SetHisto2D("absorber.exit.XY", "Scatter plot of all particles exiting absorber into absorber; x [cm]; y [cm]", 
 													400, -5*cm, 5*cm,
  												400, -5*cm, 5*cm,
  												"cm", "cm");	
	
	histoManager->SetProfile("absorber.Ex", "Energy of the particles as a function of depth; Depth in Absorber [mm]", nbBins, 0, length, "mm");
	histoManager->SetProfile("absorber.Ex.proton", "Energy of the particles as a function of depth (just protons); Depth in Absorber [mm]", nbBins, 0, length, "mm");
	histoManager->SetProfile("absorber.Ex.Marcus", "Energy of the particles as a function of depth; Depth in Absorber [mm]", nbBins, 0, length, "mm");
	histoManager->SetProfile("absorber.Ex.Marcus.proton", "Energy of the particles as a function of depth (just protons); Depth in Absorber [mm]", nbBins, 0, length, "mm");
	
	histoManager->SetTree("GaFChromicFilm", "Properties of steps in GaF Chromic Film");
	
	if(detector->GetWaterTankConstruction())
	{
		G4double depth = detector->GetWaterTankDepth();
		histoManager->SetHisto("watertank.indicent", "Energy of incident particles into the water itself", 400,0,40);
		histoManager->SetHisto("watertank_dEdx", "Energy deposition as a function of depth in the water tank", nbBins, -depth, depth, "mm");
		histoManager->SetHisto("watertank.dEdx.proton", "Energy deposition as a function of depth in the water tank by protons", nbBins, 0., depth, "mm");
		histoManager->SetProfile("watertank.Ex", "Energy of the particles as a function of depth; Depth in Water Tank [mm]", nbBins, 0., depth, "mm");
		histoManager->SetProfile("watertank.Ex.proton", "Energy of the protons as a function of depth; Depth in Water Tank [mm]", nbBins, 0., depth, "mm");
	}
 	
    histoManager->book();
}
/*
G4Run* RunAction::GenerateRun()
{
	return new Run("absorber_mfd");
}*/

void RunAction::EndOfRunAction(const G4Run* aRun)
{ 
	G4cout << " Total energy deposit= "<< G4BestUnit(edeptot,"Energy")
         << G4endl;
    G4cout << " niel energy deposit = "<< G4BestUnit(eniel,"Energy")
         << G4endl;

	//
	// Dump result for all HitsMap collections.
	//
/*	Run* theRun = (Run*)aRun;
	theRun->DumpData();
*/
    // save and clean histo
    histoManager->save();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// count up the total energy in the run deposited in sensitive volume
void RunAction::FillEdep(G4double de, G4double eni) 
{
	edeptot += de; 
	eniel += eni;
}
