#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

#include "detector.hh"
#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4GlobalMagFieldMessenger.hh"

class G4UserLimits;

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction();
    ~MyDetectorConstruction();
    
    virtual G4VPhysicalVolume *Construct();

private:
    G4LogicalVolume* logicTungsten;
    G4LogicalVolume* logicDetector;
    virtual void ConstructSDandField();

    G4double fieldValuex,fieldValuey,fieldValuez;

    G4UserLimits* fStepLimit = nullptr;
    G4FieldManager* fieldMgr = nullptr;
    G4MagIntegratorStepper*  fStepper = nullptr;
    G4ChordFinder* fChordFinder = nullptr;
    G4UniformMagField* magField = nullptr;    

};

#endif
