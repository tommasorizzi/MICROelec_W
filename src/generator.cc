#include "generator.hh"
#include "G4GeneralParticleSource.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleSource = new G4GeneralParticleSource();
    
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("e-");

    G4ThreeVector pos(0., 0., 0.);
    G4ThreeVector mom(0., 0., -1.0);
    //G4double energy = GetRandomEnergy();

    //fParticleGun->SetParticlePosition(pos);
    //fParticleGun->SetParticleMomentumDirection(mom);
    //fParticleGun->SetParticleEnergy(1000 * keV);
    //fParticleGun->SetParticleDefinition(particle);
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleSource;
    
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    fParticleSource->GeneratePrimaryVertex(anEvent);
}

//G4double MyPrimaryGenerator::GetRandomEnergy()
//{
  //  static std::random_device rd;
    //static std::mt19937 gen(rd());
    //static std::uniform_real_distribution<G4double> dis(1000* eV, 1000 * keV);
    //return dis(gen);
//}
