#include "Stepping.hh"
#include "action.hh"
#include "runaction.hh"
#include "generator.hh"
#include "runaction.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHandle.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "G4SteppingManager.hh"
#include "G4RunManager.hh"
#include "G4UserLimits.hh"

#include <fstream>
#include <set> 

std::set<G4int> countedParticles; 


MySteppingAction::MySteppingAction() : G4UserSteppingAction() {}

MySteppingAction::~MySteppingAction() {}

void MySteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String particleName = particle->GetParticleName();
  G4int particleID = track->GetTrackID();  // Get the unique Particle ID
  G4int parentID = track->GetParentID();

  // Check if the particle is an electron (secondaries of interest)
  if (particleName != "e-") return;

  // Get Kinetic Energy of the particle
  G4double kineticEnergy = track->GetKineticEnergy() / eV;

  const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();
  const G4VTouchable* touchable_out = step->GetPostStepPoint()->GetTouchable();
  G4int copyNumber = touchable->GetCopyNumber();
  G4int copyNumber_out = touchable_out->GetCopyNumber();
  G4String volumeName = touchable->GetVolume()->GetName();
  G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

  G4double stepLength = step->GetStepLength();

  // Retrieve user-defined step limit if it exists
  // G4LogicalVolume* logicalVolume = touchable->GetVolume()->GetLogicalVolume();
  // G4UserLimits* userLimits = logicalVolume->GetUserLimits();
  // G4double stepLimit = (userLimits) ? userLimits->GetMaxAllowedStep(*track) : -1.0;

  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  G4String creatorName = (creatorProcess) ? creatorProcess->GetProcessName() : "Primary";

  // Check if this is the first step in the volume
  G4bool isFirstStep = step->IsFirstStepInVolume();
  G4bool isLastStep = step->IsLastStepInVolume();
  
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4ThreeVector position = preStepPoint->GetPosition();
  // Print results
  //G4cout << "--------------------------------" << G4endl;
  //G4cout << "Particle ID: " << particleID << G4endl;
  //G4cout << "Energy: " << kineticEnergy / eV << " eV" << G4endl;
  //G4cout << "Copy Number: " << copyNumber << G4endl;
  //// G4cout << "Born in Volume: " << volumeName << G4endl;
  //G4cout << "First Step in Volume: " << (isFirstStep ? "Yes" : "No") << G4endl;
  //// G4cout << "Created by Process: " << creatorName << G4endl;
  //// G4cout << "Step Length: " << stepLength / nm << " nm" << G4endl;

  //G4int isSEE = (parentID > 0 && kineticEnergy >= 4.55 && kineticEnergy < 50.0) ? 1 : 0;
  //G4int isSEE = (parentID > 0  && kineticEnergy < 50.0) ? 1 : 0;
  //G4int isBSE = (parentID == 0) ? 1 : 0;

  G4int isSEE = (kineticEnergy < 50.0) ? 1 : 0;
  G4int isBSE = (kineticEnergy >= 50.0) ? 1 : 0;

  G4int isTEY = 1;  // All e⁻ hitting detector

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();


  if (copyNumber == 2 && isFirstStep)
  {
    analysisManager->FillNtupleDColumn(0, 0, kineticEnergy);
    analysisManager->FillNtupleIColumn(0, 1, particleID);
    analysisManager->FillNtupleIColumn(0, 2, eventID);
    analysisManager->FillNtupleDColumn(0, 3, position[2]);
    analysisManager->FillNtupleIColumn(0, 4, parentID);
    analysisManager->FillNtupleIColumn(0, 5, isSEE);
    analysisManager->FillNtupleIColumn(0, 6, isBSE);
    analysisManager->FillNtupleIColumn(0, 7, isTEY);
    analysisManager->AddNtupleRow(0);
    track->SetTrackStatus(fStopAndKill);
  }


  //if (kineticEnergy<50*eV && isLastStep == false) 
  //{
   // analysisManager->FillNtupleDColumn(1, 0, kineticEnergy);
    //analysisManager->FillNtupleIColumn(1, 1, particleID);
    //analysisManager->AddNtupleRow(1);
  //}
}
