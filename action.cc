#include "action.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4SteppingManager.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4VProcess.hh"
#include "runaction.hh"
#include "generator.hh"
#include "Stepping.hh"
#include "construction.hh"

MyActionInitialization::MyActionInitialization()
{}

MyActionInitialization::~MyActionInitialization()
{}

void MyActionInitialization::BuildForMaster() const
{
  MyRunAction* runAction = new MyRunAction();
  SetUserAction(runAction);
}

void MyActionInitialization::Build() const
{
  MyPrimaryGenerator* generator = new MyPrimaryGenerator();
  SetUserAction(generator);

  SetUserAction(new MyRunAction());

  SetUserAction(new MySteppingAction());
}
