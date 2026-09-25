#include "detector.hh"

#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"

#include "runaction.hh"

MySensitiveDetector::MySensitiveDetector(G4String name)
    : G4VSensitiveDetector(name), fName(name) {}

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
        // Get the kinetic energy and momentum
        G4double energy = track->GetKineticEnergy();
        G4ThreeVector momentum = track->GetMomentumDirection();

        // Get the Run Action
        MyRunAction* runAction =
            (MyRunAction*)G4RunManager::GetRunManager()->GetUserRunAction();

        // Top detector: redepositing electrons
        if (fName == "topDetector")
        {
            // Only electrons coming BACK
            if (momentum.z() < 0)
            {
                if (runAction) runAction->RecordHit(energy, 1);
                track->SetTrackStatus(fStopAndKill);
            }
        }

        // Original detector: escaping electrons
        if (fName == "SensitiveDetector")
        {
            if (runAction) runAction->RecordHit(energy, 0);
            track->SetTrackStatus(fStopAndKill);

            // Debugging: Uncomment to print each hit
            // G4cout << "Electron detected! Energy: " << energy / keV << " keV" << G4endl;
        }
    }

    return true;
}
