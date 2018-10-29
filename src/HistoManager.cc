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
// $Id: HistoManager.cc,v 1.1 2010-09-21 17:55:58 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"
#include "HistoMessenger.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

// line to check svn

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager* HistoManager::fHistoManager = 0;

HistoManager* HistoManager::GetHistoManager()
{ return fHistoManager; }

HistoManager::HistoManager()
:factoryOn(false)
{
  fileName[0] = "iThembaPassiveBeamline";
  fileType    = "root";
  fileOption  = "export=root";
  
  histoMessenger = new HistoMessenger(this);
  
  
  if(fHistoManager)
  {
    G4Exception("HistoManager::HistoManager", "Run9999",
                FatalException, "G4HistoManager constructed twice.");
  }
  
  fHistoManager = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
  delete histoMessenger; 
  delete fHistoManager;
	delete file; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::book()
{
    std::cout << "BOOKY BOOKY" << std::endl;
    
	// Creating a tree mapped to an hbook file.
	fileName[1] = fileName[0] + "." + fileType;
  file = new TFile(fileName[1], "RECREATE");
  
  // create selected histograms
  for (G4int k=0; k<int(histoInfo.size()); k++) {
    if (histoInfo[k].exist) {
      
      histoInfo[k].histo = new TH1F( histoInfo[k].Label, histoInfo[k].Title,
																			histoInfo[k].Nbins, histoInfo[k].Vmin, histoInfo[k].Vmax);

      factoryOn = true;
    }
  }
  
  // create selected 2D histograms
  for (G4int k=0; k<int(histo2DInfo.size()); k++) {
    if (histo2DInfo[k].exist) {
      
			 histo2DInfo[k].histo = new TH2F( histo2DInfo[k].Label, histo2DInfo[k].Title,
                                          histo2DInfo[k].NbinsX, histo2DInfo[k].Xmin, histo2DInfo[k].Xmax,
                                          histo2DInfo[k].NbinsY, histo2DInfo[k].Ymin, histo2DInfo[k].Ymax);
      
			factoryOn = true;
    }
  }
  
  // create selectred TProfiles
  for(G4int k=0; k<int(profileInfo.size()); k++)
	{
		if(profileInfo[k].exist)
		{
			profileInfo[k].profile = new TProfile(profileInfo[k].Label, profileInfo[k].Title,
																						profileInfo[k].Nbins, profileInfo[k].Vmin, profileInfo[k].Vmax);
			factoryOn=true;
		}
	}
  
  // create the selected TTrees
	for(G4int k=0; k<int(treeInfo.size()); k++) {
		if(treeInfo[k].exist) {
			treeInfo[k].tree = new TTree(treeInfo[k].Label, treeInfo[k].Title);
			treeInfo[k].tree -> Branch( "rndmSeed", &treeInfo[k].rndmSeed);
			treeInfo[k].tree -> Branch( "evtNumber" , &treeInfo[k].evtNumber );
			treeInfo[k].tree -> Branch( "x", &treeInfo[k].x);
			treeInfo[k].tree -> Branch( "y", &treeInfo[k].y);
			treeInfo[k].tree -> Branch( "z", &treeInfo[k].z);
			treeInfo[k].tree -> Branch( "px", &treeInfo[k].px);
			treeInfo[k].tree -> Branch( "py", &treeInfo[k].py);
			treeInfo[k].tree -> Branch( "pz", &treeInfo[k].pz);
			treeInfo[k].tree -> Branch( "trackID", &treeInfo[k].trackID);
			treeInfo[k].tree -> Branch( "parentID", &treeInfo[k].parentID);
			treeInfo[k].tree -> Branch( "isIncident", &treeInfo[k].isIncident);
			treeInfo[k].tree -> Branch( "isExit", &treeInfo[k].isExit);
			treeInfo[k].tree -> Branch( "kineticEnergy", &treeInfo[k].kineticEnergy);
			treeInfo[k].tree -> Branch( "edep", &treeInfo[k].edep );
			treeInfo[k].tree -> Branch( "niel", &treeInfo[k].niel);
			treeInfo[k].tree -> Branch( "pdgid", &treeInfo[k].pdgid);
			treeInfo[k].tree -> Branch( "charge", &treeInfo[k].charge);
			treeInfo[k].tree -> Branch( "processSubType", &treeInfo[k].processType);
			factoryOn = true;
		}
	}
  
  if (factoryOn) 
     G4cout << "\n----> Histogram Tree is opened in " << file->GetName() << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::save()
{
  if (factoryOn && file->IsOpen()) 
	{

		
    //saveAscii();          // Write ascii file, if any   
		file->cd();
		file->ls();
		
		file->Write();
		file->Close();

		G4cout << "\n----> Histogram Tree is saved in " << file->GetName() << G4endl;

		delete file;
		factoryOn = false;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillHisto(const G4String& label, G4double e, G4double weight)
{
	G4int ih = GetHistoAddress(label);

  if (ih < 0) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << ih
           << "does not exist; e= " << e << " w= " << weight << G4endl;
    return;
  }

  if(histoInfo[ih].exist) histoInfo[ih].histo->Fill(e/histoInfo[ih].Unit, weight);

}

void HistoManager::FillHisto2D(const G4String& label, G4double x, G4double y, G4double weight)
{
	G4int ih = GetHisto2DAddress(label);

  if (ih < 0) {
    G4cout << "---> warning from HistoManager::FillHisto2D() : histo2D " << ih
           << "does not exist; x= " << x << " y= " << y << " w= " << weight << G4endl;
    return;
  }

  if(histo2DInfo[ih].exist) histo2DInfo[ih].histo->Fill(x/histo2DInfo[ih].UnitX, y/histo2DInfo[ih].UnitY, weight);

}

void HistoManager::FillProfile(const G4String& label, G4double e, G4double weight)
{
	G4int ih = GetProfileAddress(label);

  if (ih < 0) {
    G4cout << "---> warning from HistoManager::FillProfile() : profile " << ih
           << "does not exist; e= " << e << " w= " << weight << G4endl;
    return;
  }

  if(profileInfo[ih].exist) profileInfo[ih].profile->Fill(e/histoInfo[ih].Unit, weight);

}

void HistoManager::FillTree(const G4String& label, G4double rndmSeed, G4double evtNumber, 
									G4double x, G4double y, G4double z, 
								  G4double px, G4double py, G4double pz, 
								  G4double trackID, G4double parentID, 
									bool isIncident, bool isExit,
								  G4double kineticEnergy, G4double edep, G4double niel,
									G4double pdgid, G4double charge, G4int processSubType)
{
	G4int ih = GetTreeAddress(label);
	
	if (ih < 0) {
    G4cout << "---> warning from HistoManager::FillTree() : tree " << ih
           << "does not exist;" << G4endl;
    return;
  }
  

  if(treeInfo[ih].exist) 
	{
		treeInfo[ih].rndmSeed = rndmSeed;
		treeInfo[ih].evtNumber = evtNumber;
		treeInfo[ih].x = x;
		treeInfo[ih].y = y;
		treeInfo[ih].z = z;
		treeInfo[ih].px = px;
		treeInfo[ih].py = py;
		treeInfo[ih].pz = pz;
		treeInfo[ih].trackID = trackID;
		treeInfo[ih].parentID = parentID;
		treeInfo[ih].isIncident = isIncident;
		treeInfo[ih].isExit = isExit;
		treeInfo[ih].kineticEnergy = kineticEnergy;
		treeInfo[ih].edep = edep;
		treeInfo[ih].niel = niel;
		treeInfo[ih].pdgid = pdgid;
		treeInfo[ih].charge = charge;
		treeInfo[ih].processType = double(processSubType);
	}
 
 treeInfo[ih].tree->Fill();
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::SetHisto(const G4String& label, const G4String& title,
                 G4int nbins, G4double valmin, G4double valmax, const G4String& unit)
{
	for(unsigned i=0; i<histoInfo.size(); i++)
	{
		if( label == histoInfo[i].Label )
		{
			G4cout << "---> warning from HistoManager::SetHisto() : histo " << histoInfo[i].Label
           << "already exists" << G4endl;
			return;
		}
	}
	
	HistoInfo hi;
	hi.Label = label;
	histoInfo.push_back(hi);
	
	G4int ih = GetHistoAddress(label);
	
  G4String titl = title;
  G4double vmin = valmin, vmax = valmax;
  histoInfo[ih].Unit = 1.;

  if (unit != "none") {
    titl = title + " (" + unit + ")";
    histoInfo[ih].Unit = G4UnitDefinition::GetValueOf(unit);
    vmin = valmin/histoInfo[ih].Unit; 
    vmax = valmax/histoInfo[ih].Unit;
  }

  histoInfo[ih].exist = true;
  histoInfo[ih].Label = label;
  histoInfo[ih].Title = titl;
  histoInfo[ih].Nbins = nbins;
  histoInfo[ih].Vmin  = vmin;
  histoInfo[ih].Vmax  = vmax;
  histoInfo[ih].Width = (valmax-valmin)/nbins;

  G4cout << "----> SetHisto " << ih << ": " << titl << ";  "
         << nbins << " bins from "
         << vmin << " " << unit << " to " << vmax << " " << unit << G4endl;

}

void HistoManager::SetProfile(const G4String& label, const G4String& title,
                 G4int nbins, G4double valmin, G4double valmax, const G4String& unit)
{
	for(unsigned i=0; i<profileInfo.size(); i++)
	{
		if( label == profileInfo[i].Label )
		{
			G4cout << "---> warning from HistoManager::SetProfile() : profile " << profileInfo[i].Label
           << "already exists" << G4endl;
			return;
		}
	}
	
	ProfileInfo pi;
	pi.Label = label;	
	
  G4String titl = title;
  G4double vmin = valmin, vmax = valmax;
  pi.Unit = 1.;

  if (unit != "none") {
    titl = title + " (" + unit + ")";
		pi.Unit = G4UnitDefinition::GetValueOf(unit);
    vmin = valmin/pi.Unit; 
    vmax = valmax/pi.Unit;
  }

  pi.exist = true;
  pi.Label = label;
  pi.Title = titl;
  pi.Nbins = nbins;
  pi.Vmin  = vmin;
  pi.Vmax  = vmax;
  pi.Width = (valmax-valmin)/nbins;
	
	profileInfo.push_back(pi);
	G4int ih = GetProfileAddress(label);

  G4cout << "----> SetProfile " << ih << ": " << titl << ";  "
         << nbins << " bins from "
         << vmin << " " << unit << " to " << vmax << " " << unit << G4endl;

}

void HistoManager::SetHisto2D(const G4String& label, const G4String& title,
                 G4int nbinsx, G4double vminx, G4double vmaxx, 
                 G4int nbinsy, G4double vminy, G4double vmaxy,
                 const G4String& unitx, const G4String& unity)
{
	for(unsigned i=0; i<histo2DInfo.size(); i++)
	{
		if( label == histo2DInfo[i].Label )
		{
			G4cout << "---> warning from HistoManager::SetHisto() : histo " << histoInfo[i].Label
           << "already exists" << G4endl;
			return;
		}
	}
	
	Histo2DInfo hi;
	hi.Label = label;
	histo2DInfo.push_back(hi);
	
	G4int ih = GetHisto2DAddress(label);

  G4String titl = title;
  
  histo2DInfo[ih].UnitX = 1.;

  if (unitx != "none") {
    histo2DInfo[ih].UnitX = G4UnitDefinition::GetValueOf(unitx);
    vminx = vminx/histo2DInfo[ih].UnitX; 
    vmaxx = vmaxx/histo2DInfo[ih].UnitX;
  }
  
  histo2DInfo[ih].UnitY = 1.;

  if (unity != "none") {
    histo2DInfo[ih].UnitY = G4UnitDefinition::GetValueOf(unity);
    vminy = vminy/histo2DInfo[ih].UnitY; 
    vmaxy = vmaxy/histo2DInfo[ih].UnitY;
  }

  histo2DInfo[ih].exist = true;
  histo2DInfo[ih].Label = label;
  histo2DInfo[ih].Title = titl;
  histo2DInfo[ih].NbinsX = nbinsx;
  histo2DInfo[ih].NbinsY = nbinsy;
  histo2DInfo[ih].Xmin  = vminx;
  histo2DInfo[ih].Xmax  = vmaxx;
  histo2DInfo[ih].Ymin  = vminy;
  histo2DInfo[ih].Ymax  = vmaxy;

  G4cout << "----> SetHisto2D " << ih << ": " << titl << ";  "
         << nbinsx << " x-bins from "
         << vminx << " " << unitx << " to " << vmaxx << " " << unitx << ";  "
         << nbinsy << " y-bins from "
         << vminy << " " << unity << " to " << vmaxy << " " << unity
         << G4endl;

}


void HistoManager::SetTree(const G4String& label, const G4String& title)
{
	for(unsigned i=0; i<treeInfo.size(); i++)
	{
		if( label == treeInfo[i].Label )
		{
			G4cout << "---> warning from HistoManager::SetTree() : tree " << treeInfo[i].Label
           << "already exists" << G4endl;
			return;
		}
	}
	
	TreeInfo ti;
	ti.Label = label;
	ti.Title = title;
	ti.exist = true;
	treeInfo.push_back(ti);
}

G4int HistoManager::GetHistoAddress(const G4String& label)
{
	G4int address = -1;
	for(unsigned i=0; i<histoInfo.size(); i++)
	{
		if( histoInfo[i].Label == label ) address = i;
	}
	if(address <0 ) G4cerr << "Histogram " << label << " not defined" << G4endl;
	assert(address>=0);
	return address;
}

G4int HistoManager::GetHisto2DAddress(const G4String& label)
{
	G4int address = -1;
	for(unsigned i=0; i<histo2DInfo.size(); i++)
	{
		if( histo2DInfo[i].Label == label ) address = i;
	}
    if(address<0) G4cerr << "Histogram2D " << label << " not defined" << G4endl;
	assert(address>=0);
	return address;
}

G4int HistoManager::GetProfileAddress(const G4String& label)
{
	G4int address = -1;
	for(unsigned i=0; i<profileInfo.size(); i++)
	{
		if( profileInfo[i].Label == label ) address = i;
	}
	if(address <0 ) G4cerr << "Profile " << label << " not defined" << G4endl;
	assert(address>=0);
	return address;
}

G4int HistoManager::GetTreeAddress(const G4String& label)
{
	G4int address = -1;
	for(unsigned i=0; i<treeInfo.size(); i++)
	{
		if( treeInfo[i].Label == label ) address = i;
	}
	assert(address>=0);
	return address;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Scale(const G4String& label, G4double fac)
{
	G4int ih = GetHistoAddress(label);
 	if (ih > MaxHisto) {
    G4cout << "---> warning from HistoManager::Scale() : histo " << ih
           << "does not exist  (fac = " << fac << ")"  << G4endl;
    return;
  }

  //if(histoInfo[ih].exist) histoInfo[ih].histo->scale(fac);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::RemoveHisto(const G4String& label)
{
	G4int ih = GetHistoAddress(label);
	
 	if (ih > MaxHisto) {
    G4cout << "---> warning from HistoManager::RemoveHisto() : histo " << ih
           << "does not exist" << G4endl;
    return;
  }

  histoInfo[ih].histo = 0;  
  histoInfo[ih].exist = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::PrintHisto(const G4String& label)
{
	G4int ih = GetHistoAddress(label);
	
 	if (ih < MaxHisto) { histoInfo[ih].ascii = true; histoInfo[0].ascii = true; }
 	else
    G4cout << "---> warning from HistoManager::PrintHisto() : histo " << ih
           << "does not exist" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <fstream>

void HistoManager::saveAscii()
{

/*
 if (!ascii[0]) return;
 
 G4String name = fileName[0] + ".ascii";
 std::ofstream File(name, std::ios::out);
 File.setf( std::ios::scientific, std::ios::floatfield );
 
 //write selected histograms
 for (G4int ih=0; ih<MaxHisto; ih++) {
    if (exist[ih] && ascii[ih]) {
      File << "\n  1D histogram " << ih << ": " << Title[ih] 
           << "\n \n \t     X \t\t     Y" << G4endl;
     
      for (G4int iBin=0; iBin<Nbins[ih]; iBin++) {
         File << "  " << iBin << "\t" 
	      << 0.5*(histo[ih]->axis().binLowerEdge(iBin) +
	              histo[ih]->axis().binUpperEdge(iBin)) << "\t"
	      << histo[ih]->binHeight(iBin) 
	      << G4endl;
      } 
    }
 }*/

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::AddBranch( TTree* input_tree, G4String title, std::vector<double> data )
{
        if( (input_tree->GetEntries() != (int)data.size()) && (input_tree->GetEntries() != 0) )
        {
                G4cout << "ERROR: CANNOT ADD BRANCH " << title << " DATA SIZE DID NOT MATCH TREE." << G4endl;
                return;
        }

        unsigned int size = (unsigned)data.size();//input_tree->GetEntries();

        double thisValue=0.;
        TString name=title.c_str();
        name.Append("/D");
        TBranch* output_branch_obj = input_tree->Branch( title.c_str(), &thisValue, name );

        input_tree->SetEntries( (int)size );
        for( unsigned int i=0; i< size; ++i )
        {
                thisValue = data[i];
                output_branch_obj->Fill();
        }
}

void HistoManager::AddBranch( TTree* input_tree, G4String title, std::vector<TString> data )
{
        if( (input_tree->GetEntries() != (int)data.size()) && (input_tree->GetEntries() != 0) )
        {
                G4cout << "ERROR: CANNOT ADD BRANCH " << title << " DATA SIZE DID NOT MATCH TREE." << G4endl;
                return;
        }

        unsigned int size = (unsigned)data.size();//input_tree->GetEntries();

        TString thisValue="";
        TString name=title.c_str();
        name.Append("/C");
        TBranch* output_branch_obj = input_tree->Branch( title.c_str(), &thisValue, name );

        input_tree->SetEntries( (int)size );
        for( unsigned int i=0; i< size; ++i )
        {
                thisValue = data[i]	;
                output_branch_obj->Fill();
        }
}

void HistoManager::AddBranch( TTree* input_tree, G4String title, std::vector<bool> data )
{
        if( (input_tree->GetEntries() != (int)data.size()) && (input_tree->GetEntries() != 0) )
        {
                G4cout << "ERROR: CANNOT ADD BRANCH " << title << " DATA SIZE DID NOT MATCH TREE." << G4endl;
                return;
        }

        unsigned int size = (unsigned)data.size();//input_tree->GetEntries();

        bool thisValue=false;
        TString name=title.c_str();
        name.Append("/O");
        TBranch* output_branch_obj = input_tree->Branch( title.c_str(), &thisValue, name );

        input_tree->SetEntries( (int)size );
        for( unsigned int i=0; i< size; ++i )
        {
                thisValue = data[i]	;
                output_branch_obj->Fill();
        }
}



