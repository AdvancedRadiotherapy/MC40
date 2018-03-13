#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include "vector"
#include "cassert"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


class HistoMessenger;

const G4int MaxHisto = 9999;
const G4int MaxHisto2D = 9999;

// Structure to allow ease of use for histograms
struct HistoInfo {
	TH1F*					 histo;
    G4bool                   exist;
    G4String                 Label;
    G4String                 Title;
    G4int                    Nbins;
    G4double                 Vmin ;
    G4double                 Vmax ;
    G4double                 Unit ;
    G4double                 Width;
    G4bool                   ascii;
};

struct Histo2DInfo {
	TH2F*					 histo;
    G4bool                   exist;
    G4String                 Label;
    G4String                 Title;
    G4int                    NbinsX;
    G4int                    NbinsY;
    G4double                 Xmin ;
    G4double                 Xmax ;
    G4double                 Ymin ;
    G4double                 Ymax ;
    G4double                 UnitX ;
    G4double                 UnitY ;
};

struct ProfileInfo {
	TProfile*				 profile;
    G4bool                   exist;
    G4String                 Label;
    G4String                 Title;
    G4int                    Nbins;
    G4double                 Vmin ;
    G4double                 Vmax ;
    G4double                 Unit ;
    G4double                 Width;
    G4bool                   ascii;
};

struct TreeInfo{
	TTree*                  tree;
	G4String                Label;
	G4String                Title;
	G4bool                  exist;
	double                  rndmSeed;
	double                  evtNumber;
	double                  x;
	double                  y;
	double                  z;
	double                  px;
	double                  py;
	double                  pz;
	double                  trackID;
	double                  parentID;
	bool                    isIncident;
	bool                    isExit;
	double                  kineticEnergy;
	double                  edep;
	double                  niel;
	double                  pdgid;
	double                  charge;
	double                  processType;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager
{
  public:

    HistoManager();
    ~HistoManager();

    static HistoManager* GetHistoManager();
   
    void SetFileName   (const G4String& name) { fileName[0] = name;};
    void SetFileType   (const G4String& name) { fileType    = name;};
    void SetFileOption (const G4String& name) { fileOption  = name;};    
    void book();
    void save();
		
	// functions to use TTrees
	void SetTree(const G4String& label, const G4String& title);
	void FillTree(const G4String& label, G4double rndmSeed, G4double evtNumber, 
									G4double x, G4double y, G4double z, 
								  G4double px, G4double py, G4double pz, 
								  G4double trackID, G4double parentID, 
									bool isIncident, bool isExit,
								  G4double kineticEnergy, G4double edep, G4double niel,
									G4double pdgid, G4double charge, G4int processSubType=-1);
	void AddBranch( TTree* input_tree, G4String title, std::vector<double> data );
	void AddBranch( TTree* input_tree, G4String title, std::vector<TString> data );
	void AddBranch( TTree* input_tree, G4String title, std::vector<bool> data );
	G4int GetTreeAddress(const G4String& label);
    
	//functions to use TH1F
	void SetHisto (const G4String& label, const G4String& title, G4int,G4double,G4double,const G4String& unit="none");
    void FillHisto(const G4String& label, G4double e, G4double weight = 1.0);
    G4int GetHistoAddress(const G4String& label);
		
	// functions to use TH2F
	void SetHisto2D (const G4String& label, const G4String& title, 
    								 G4int nbinsx, G4double vminx, G4double vmaxx,
    								 G4int nbinsy, G4double vminy, G4double vmaxy,
    								 const G4String& unitx="none", const G4String& unity="none");  
    void FillHisto2D(const G4String& label, G4double x, G4double y, G4double weight = 1.0);    
    G4int GetHisto2DAddress(const G4String& label);
		
	// functions to use TProfile
	void SetProfile(const G4String& label, const G4String& title, G4int,G4double,G4double,const G4String& unit="none");
    void FillProfile(const G4String& label, G4double e, G4double weight = 1.0);
    G4int GetProfileAddress(const G4String& label);
		
	// functions for HistoMessenger (not yet implimented)
	void Scale (const G4String&, G4double);    
    void RemoveHisto (const G4String&);
    void PrintHisto  (const G4String&);
    
    
    G4bool    HistoExist  (G4int id) {return histoInfo[id].exist;}
    G4double  GetHistoUnit(G4int id) {return histoInfo[id].Unit;}
    G4double  GetBinWidth (G4int id) {return histoInfo[id].Width;}     
    G4bool    Histo2DExist(G4int id) {return histo2DInfo[id].exist;}
    
    TFile* GetFile(){ return file;};

  private:
    
    static HistoManager* fHistoManager;

    G4String                 fileName[2];
    G4String                 fileType;
    G4String                 fileOption; 
	TFile* 					 file;
    
    std::vector<HistoInfo>   histoInfo;
    std::vector<Histo2DInfo> histo2DInfo;
	std::vector<TreeInfo>    treeInfo;
	std::vector<ProfileInfo>  profileInfo;

        
    G4bool                   factoryOn;
    HistoMessenger*          histoMessenger;
    
  private:
    void saveAscii(); 
    
 
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

