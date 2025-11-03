#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include"G4VUserPhysicsList.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4RegionStore.hh"  
#include "G4Region.hh" 
#include "G4EmStandardPhysics_option4.hh"
#include "G4UAtomicDeexcitation.hh"

class MyPhysicsList : public G4VModularPhysicsList
{
public:
    MyPhysicsList();
    ~MyPhysicsList();

    virtual void SetCuts();
 

private:

    G4bool isInSpace;
};

#endif

