#include "construction.hh"
#include "G4UserLimits.hh"
#include "G4ProductionCuts.hh"
#include "G4ProductionCutsTable.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"


///////// for B field implementation ///////

#include "G4GlobalMagFieldMessenger.hh"
#include "G4UniformMagField.hh"
#include "G4MagneticField.hh"
#include "G4AutoDelete.hh"
#include "G4SDManager.hh"
#include "G4FieldManager.hh"
#include "globals.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4SimpleHeum.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4ExactHelixStepper.hh"
#include "G4HelixMixedStepper.hh"
#include "G4ClassicalRK4.hh"
#include "G4EllipticalTube.hh"
#include "G4Ellipsoid.hh"
#include "G4GlobalMagFieldMessenger.hh"


MyDetectorConstruction::MyDetectorConstruction()
  : logicTungsten(nullptr), logicDetector(nullptr)  
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
    G4NistManager *nist = G4NistManager::Instance();

    // define world volume - can either use G4_Galactic or the vacuum parameters
    //G4Material *worldMat = nist->FindOrBuildMaterial("G4_Galactic");
    
    
    // Define world volume with a material named "Vacuum"
    G4Material* worldMat = new G4Material("Vacuum", 1., 1.01 * g / mole, 1.e-25 * g / cm3,
                                          kStateGas, 0.1 * kelvin, 1.e-19 * pascal);

    // world volume - the size should be large enough to contain the target and detector
    G4Box *solidWorld = new G4Box("solidWorld", 10 * mm, 10 * mm, 10 * mm);
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0);
    
    // define tungsten material
    G4Material* tungsten = nist->FindOrBuildMaterial("G4_W");
    
    //define target volume
    G4double lengthWall = 2 * mm;
    G4double widthWall = 0.5 * mm;
    G4double depthWall = 0.2 * mm;
    G4Box* solidTungsten = new G4Box("Target", lengthWall, widthWall, depthWall); 
    
    logicTungsten = new G4LogicalVolume(solidTungsten, tungsten, "Target");
    G4VPhysicalVolume* physTungsten = 
    	 new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicTungsten, "Target",
    			  logicWorld, false, 1);
    

    // define detector volume
    G4double detGap = 65 * micrometer;
    
    G4double lengthDetector = 2.5 * mm;
    G4double widthDetector = 1 * mm;
    G4double depthDetector = 10 * micrometer;
    
    G4double detZ = depthWall + detGap + depthDetector;
    
	


	G4Box* solidDetector = new G4Box("solidDetector", lengthDetector, widthDetector, depthDetector);
	logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
	G4VPhysicalVolume* physDetector = new G4PVPlacement(0, G4ThreeVector(0, 0, detZ), logicDetector, "physDetector", logicWorld, false, 2);



    // Define region for target which will be used in the MicroElec models
    G4Region* fregion = new G4Region("Target");
    G4ProductionCuts* cuts = new G4ProductionCuts();
    G4double defCut = 1 * nanometer;
    cuts->SetProductionCut(defCut, "gamma");
    cuts->SetProductionCut(defCut, "e-");
    cuts->SetProductionCut(defCut, "e+");
    cuts->SetProductionCut(defCut, "proton");

    fregion->SetProductionCuts(cuts);
    fregion->AddRootLogicalVolume(logicTungsten);
    

    return physWorld;
}


void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector* sensDet = new MySensitiveDetector("SensitiveDetector");

    if (logicDetector) {
      logicDetector->SetSensitiveDetector(sensDet);
    }


    fieldValuex= (0)*tesla;
    fieldValuey= (7)*tesla;
    fieldValuez= (0)*tesla;

    magField =new G4UniformMagField(G4ThreeVector(fieldValuex, fieldValuey, fieldValuez));


    fieldMgr  = G4TransportationManager::GetTransportationManager()->GetFieldManager();

    fieldMgr->SetDetectorField(magField); //create the field in all volume

    fieldMgr->CreateChordFinder(magField);

    logicDetector->SetFieldManager(nullptr, false);

    // // For controling the accurancy

   G4double fDesiredEpsilonMin = 1.0e-10; //min value for smallest step
   fieldMgr -> SetMinimumEpsilonStep( fDesiredEpsilonMin ) ;
   G4double fDesiredEpsilonMax = 1.0e-8; //max value for biggest step
   fieldMgr -> SetMaximumEpsilonStep( fDesiredEpsilonMax ) ;
   G4double fStep = 0.1e-3*mm;
   fieldMgr -> SetDeltaOneStep(fStep);

   //TO SPECIFY PARAMETRS FOR INTEGRATION
   G4Mag_UsualEqRhs* fEquation;
   fEquation = new G4Mag_UsualEqRhs(magField);

   //Choose the stepper
   //fStepper = new G4ClassicalRK4( fEquation );
   //fStepper = new G4NystromRK4( fEquation ); //for lower computational cost at the same high order
   //OTHER OPTIONS
   //fStepper = new G4HelixSimpleRunge( fEquation );
   //fStepper = new G4HelixExplicitEuler( fEquation );
   fStepper = new G4ExactHelixStepper( fEquation );
   //fStepper = new G4HelixMixedStepper( fEquation );

   G4double fMinStep = 1.0e-5*mm;
   fChordFinder = new G4ChordFinder(magField, fMinStep, fStepper);
   fieldMgr -> SetChordFinder(fChordFinder);

   G4double deltaChord = 0.00001*mm;
   fieldMgr -> GetChordFinder()->SetDeltaChord(deltaChord);

}
