#include "detector.hh"

#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"

#include "runaction.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name) {}

MySensitiveDetector::~MySensitiveDetector() {}

G4bool MySensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
    // Get the track of the particle
    G4Track* track = aStep->GetTrack();
    
    // Get the particle type
    G4String particleName = track->GetDefinition()->GetParticleName();
    
    // Only count electrons
    if (particleName == "e-") 
    {
        // Get the kinetic energy of the electron
        G4double energy = track->GetKineticEnergy();

        // Get the Run Action
        MyRunAction* runAction = (MyRunAction*)G4RunManager::GetRunManager()->GetUserRunAction();
        if (runAction) {
            runAction->RecordHit(energy);  // Log the hit energy
        }

        // Kill the electron to stop further tracking
        track->SetTrackStatus(fStopAndKill);

        // Debugging: Uncomment if you want to see output per hit
        // G4cout << "Electron detected! Energy: " << energy / keV << " keV" << G4endl;
    }

    return true;
}
