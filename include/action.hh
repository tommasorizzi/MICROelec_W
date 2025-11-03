#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "generator.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include "runaction.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
public:
    MyActionInitialization();
    ~MyActionInitialization();
    
    virtual void Build() const;
    virtual void BuildForMaster() const;
};

#endif
