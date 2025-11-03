#ifndef MicroElecPhysics_h
#define MicroElecPhysics_h 1

#include "G4ParticleTypes.hh"
#include "G4ProcessManager.hh"
#include "G4VUserPhysicsList.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class MicroElecPhysics : public G4VUserPhysicsList

{
  public:
    MicroElecPhysics();
    virtual ~MicroElecPhysics();

    void SetGammaCut(G4double);
    void SetElectronCut(G4double);
    void SetPositronCut(G4double);
    void SetProtonCut(G4double);

  protected:
    // these methods construct particles
    void ConstructBosons();
    void ConstructLeptons();
    void ConstructBarions();

    // these methods construct physics processes and register them
    void ConstructGeneral();
    void ConstructEM();

    // Construct particle and physics
    void ConstructParticle();
    void ConstructProcess();

    // set cuts
    void SetCuts();

  private:
    G4double cutForGamma;
    G4double cutForElectron;
    G4double cutForPositron;
    G4double cutForProton;
};
#endif
