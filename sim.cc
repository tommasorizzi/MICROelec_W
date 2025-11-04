#include "physicsMicroElecNew.hh"
#include "physics.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"
#include "G4MTRunManager.hh"

#include "action.hh"
#include "construction.hh"
#include "generator.hh"
#include "detector.hh"
#include "runaction.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include "ElectronCapture.hh"
#include "Stepping.hh"
#include "Randomize.hh"


int main(int argc, char** argv)
{

////////////////////////////////////////////
////////////////////////////////////////////
////TEST for SEEDING with RANDON number/////
////////////////////////////////////////////
//
// G4long fixedSeed = 123456789; 
// G4Random::setTheSeed(fixedSeed);

// G4cout << "Initial Random Seed: "
// << G4Random::getTheSeed() << G4endl;


  G4UIExecutive* ui = nullptr;

#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager();
#else
  G4RunManager* runManager = new G4RunManager();
#endif

  runManager->SetUserInitialization(new MyDetectorConstruction());

  G4bool useMicroElec = true;
  if (useMicroElec) {
    runManager->SetUserInitialization(new MicroElecPhysics());
  } else {
    runManager->SetUserInitialization(new MyPhysicsList());
  }

 
  runManager->SetUserInitialization(new MyActionInitialization());

  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();

  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc == 1)
  {
    ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute vis.mac");

    // === Batch execution of all macros from macro_list.txt ===
    // Here you must change the path with the correct name of the macros you have generated i.e macros150 corresponds to 150nm dust
    std::ifstream macroList("macros150_surface/macro_list.txt");
    if (!macroList.is_open()) {
      G4cerr << "Error: Could not open macros/macro_list.txt" << G4endl;
      return 1;
    }

    G4String macroPath;
    while (std::getline(macroList, macroPath)) {
      G4cout << ">>> Running macro: " << macroPath << G4endl;
      std::string envVar = "CURRENT_MACRO=" + std::string(macroPath);
      putenv(strdup(envVar.c_str()));
      G4String command = "/control/execute " + macroPath;
      UImanager->ApplyCommand(command);
    }
    macroList.close();

    ui->SessionStart();
    delete ui;
  }
  else
  {
    // Allow command-line execution of a single macro
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  delete visManager;
  delete runManager;
  return 0;
}
