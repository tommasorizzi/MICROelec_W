#include "runaction.hh"
#include "construction.hh"
#include "generator.hh"
#include "G4ParticleDefinition.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4ProcessManager.hh"
#include "G4AccumulableManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include <fstream>
#include <sstream>
#include "runactionMessenger.hh"
RunActionMessenger* messenger;

G4String MyRunAction::runName = "default";

MyRunAction::MyRunAction()
{
    messenger = new RunActionMessenger(this);
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
    G4int runID = run->GetRunID();
    G4AccumulableManager* accManager = G4AccumulableManager::Instance();
    accManager->Reset();

    // Create output folders (only once by master thread)
    if (G4Threading::IsMasterThread()) {
        system("mkdir -p output_stepping"); //ONLY WORRY ABOUT THIS ONE, IGNORE OUTPUT_HITS
        system("mkdir -p output_hits");
    }

    // Build a clean unique file suffix: _runX_tY
    std::ostringstream suffix;
    suffix << "_run" << runID;
    if (G4Threading::IsMultithreadedApplication()) {
        G4int threadID = G4Threading::G4GetThreadId();
        suffix << "_t" << threadID;
    }

    G4String nameTag = MyRunAction::GetRunName();

    // ===== Output file for hits =====
    std::ostringstream hitFileName;
    hitFileName << "output_hits/hits_" << nameTag << suffix.str() << ".csv";

    hitFile.open(hitFileName.str(), std::ios::out);

    if (!hitFile.is_open()) {
        G4cerr << "ERROR: Could not open " << hitFileName.str() << " for writing!" << G4endl;
    } else {
        hitFile << "Energy (eV),EventID\n";
    }

    // ===== Output file for stepping/analysis =====
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    G4String steppingFileName = "output_stepping/" + nameTag + ".csv";


    analysisManager->SetDefaultFileType("csv");
    analysisManager->OpenFile(steppingFileName);
    analysisManager->CreateNtuple("data", "data");
    analysisManager->CreateNtupleDColumn("Ekin");
    analysisManager->CreateNtupleIColumn("ParticleID");
    analysisManager->CreateNtupleIColumn("EventID");
    analysisManager->CreateNtupleDColumn("z coordinate");
    analysisManager->CreateNtupleIColumn("ParentID");
    analysisManager->CreateNtupleIColumn("isSEE");
    analysisManager->CreateNtupleIColumn("isBSE");
    analysisManager->CreateNtupleIColumn("isTEY");
    analysisManager->FinishNtuple();
}


void MyRunAction::EndOfRunAction(const G4Run*)
{
    // Close the separate hit file
    if (hitFile.is_open()) {
        hitFile.close();
    }

    G4AccumulableManager* accManager = G4AccumulableManager::Instance();
    accManager->Merge();

    // Write and close analysis file
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

void MyRunAction::RecordHit(G4double energy)
{
    // Get the current event ID
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    if (hitFile.is_open()) {
        hitFile << energy / eV << "," << eventID << "\n";  // Save energy in eV with eventID
        hitFile.flush();
    } else {
        G4cerr << "ERROR: Attempted to write to a closed file!" << G4endl;
    }
}

void MyRunAction::SetRunName(G4String name)
{
    runName = name;
}

G4String MyRunAction::GetRunName()
{
    return runName;
}
