#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "action.hh"
#include "construction.hh"
#include "generator.hh"
#include "construction.hh"
#include "G4Transportation.hh"
#include "G4Accumulable.hh"

#include <fstream>
#include <iostream>

class G4Run;

class MyRunAction : public G4UserRunAction
{
  public:
    MyRunAction();
    virtual ~MyRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    void RecordHit(G4double energy);  // Function to store hits
    static void SetRunName(G4String name);
    static G4String GetRunName();

  private:
    std::ofstream outputFile;
    std::ofstream hitFile;  // File stream for writing data
    static G4String runName;
};

#endif
