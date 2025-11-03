#ifndef MYSTEPPINGACTION_HH
#define MYSTEPPINGACTION_HH 


#include "construction.hh"
#include "action.hh"
#include "runaction.hh"
#include "G4RunManager.hh"
#include "G4SteppingManager.hh"
#include "G4VProcess.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"

class MySteppingAction : public G4UserSteppingAction
{
  public:
    MySteppingAction();
    virtual ~MySteppingAction();

    virtual void UserSteppingAction(const G4Step* step);


};

#endif