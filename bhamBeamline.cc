#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"
#include "time.h"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "TrackingAction.hh"
#include "HistoManager.hh"
#include "G4ScoringManager.hh"

#include "QGSP_BERT_HP.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_FTFP_BERT.hh"
#include "G4VModularPhysicsList.hh"


int main(int argc,char** argv)
{

// 	// Define the random number seed
// 	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
// 	//set random seed with system time
// 	G4long seed = time(NULL);
// 	CLHEP::HepRandom::setTheSeed(seed);
	
	  // new way to make Geant4 actually take a random seed!
  CLHEP::RanluxEngine defaultEngine(1234567, 4);
  G4Random::setTheEngine( &defaultEngine );
  G4int seed = time(NULL);
  G4Random::setTheSeed(seed);
	
	// User Verbose output class
  //
  G4VSteppingVerbose* verbosity = new SteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);

	// Activate UI-command base scorer
	G4ScoringManager * scManager = G4ScoringManager::GetScoringManager();
	scManager->SetVerboseLevel(1);


  // Construct the default run manager
  //
  G4RunManager* runManager = new G4RunManager;

  // set mandatory initialization classes
  //
  
	// Initialize the physics 
  ///G4VModularPhysicsList* physicsList = 0;
  // Physics list
  //physicsList = new QGSP_BIC_HP();
  ///G4VModularPhysicsList* physicsList = new QGSP_BERT();
	///physicsList = new QGSP_FTFP_BERT();
	
	PhysicsList* physics = new PhysicsList();  
  ///physics->AddPhysicsList("QGSP_BIC_EMY");
	
	//physics->AddPhysicsList("QGSP_BIC_EMY");
  //physics->AddPhysicsList("emlivermore");
	
	
	//physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physics); 
	
	DetectorConstruction* detector = new DetectorConstruction();
	runManager->SetUserInitialization(detector);
	
	HistoManager*           histo  = new HistoManager();
	PrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction(detector);
	RunAction* run_action = new RunAction(detector, histo, physics);
	EventAction* event_action = new EventAction(histo, detector);
	SteppingAction* stepping_action = new SteppingAction(detector, run_action, histo, event_action);
    TrackingAction* tracking_action = new TrackingAction(histo);
	
  runManager->SetUserAction(gen_action);
	runManager->SetUserAction(run_action);
	runManager->SetUserAction(event_action);
  runManager->SetUserAction(stepping_action);
  runManager->SetUserAction(tracking_action);
  
  runManager->Initialize();

  // Visualization manager
#ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive;
  visManager -> Initialize();
#endif
    
  // Get the pointer to the User Interface manager
  //
  G4UImanager * UImanager = G4UImanager::GetUIpointer();  

  if (argc!=1)   // batch mode  
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else           // interactive mode : define UI session
  { 
  
#ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute macros/vis.mac"); //macros/vis.mac");     
#endif
    ui->SessionStart();
    delete ui;
#endif

  }
//   else  {  
//     // interactive mode : define UI session
// #ifdef G4UI_USE
//     G4UIExecutive* ui = new G4UIExecutive(argc, argv, "");
// #ifdef G4VIS_USE
//     UImanager->ApplyCommand("/control/execute macros/vis_scatterer.mac"); 
// #else
//     UImanager->ApplyCommand("/control/execute macros/init.mac"); 
// #endif
//     if (ui->IsGUI())
// //     UImanager->ApplyCommand("/control/execute macros/gui.mac");
//     ui->SessionStart();
//     delete ui;
// #endif
// 	}
	
	
  // Start a run
  //
  //G4int numberOfEvent = 3;
  //runManager->BeamOn(numberOfEvent);
  
//  delete runManager;
//  delete histo;
// 	delete verbosity;

 	return 0;
}
