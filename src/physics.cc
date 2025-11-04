#include "physics.hh"


#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4ProductionCutsTable.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4SystemOfUnits.hh"
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4UserLimits.hh"
#include "G4EmParameters.hh"
#include "G4Material.hh"
#include "G4PhysicsListHelper.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericIon.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"
#include "G4MscStepLimitType.hh"
#include "G4IonConstructor.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4EmStandardPhysicsSS.hh"

#include "G4eMultipleScattering.hh"
#include "G4CoulombScattering.hh"
#include "G4eIonisation.hh"
#include "G4UrbanMscModel.hh"
#include "G4eBremsstrahlung.hh"
#include "G4ePairProduction.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4PenelopeBremsstrahlungModel.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4LivermoreBremsstrahlungModel.hh"
#include "G4eSingleCoulombScatteringModel.hh"
#include "G4eCoulombScatteringModel.hh"
#include "G4GoudsmitSaundersonMscModel.hh"
#include "G4WentzelVIModel.hh"


MyPhysicsList::MyPhysicsList()
{
  
  /////// SINGLE SCATTERING PHYSICS LIST //////  
  //RegisterPhysics(new G4EmStandardPhysicsSS());
  //
  /////// GEANT4  OPT4 PHYSICS LIST     ///////
  //
  RegisterPhysics(new G4EmStandardPhysics_option4());



  G4EmParameters::Instance()->SetApplyCuts(true);
  G4EmParameters::Instance()->SetVerbose(0);
  G4EmParameters::Instance()->SetUseMottCorrection(true);
  G4EmParameters::Instance()->SetLowestElectronEnergy(5 * eV);

  SetCuts();

  G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  de->SetFluo(true);
  de->SetAuger(true);
  de->SetPIXE(true);
  de->SetAugerCascade(true);
  // G4EmParameters::Instance()->SetDeexcitationIgnoreCut(true);
  
  G4LossTableManager::Instance()->SetAtomDeexcitation(de);
}

MyPhysicsList::~MyPhysicsList() {}

void MyPhysicsList::SetCuts()
{
  ////// set cut off for secondaries particle in space / energy ////// 
  isInSpace = false;

  if (isInSpace == true) {  // To set cut value equal for all particle
    G4double NewCutValue = 8 * um;
    SetDefaultCutValue(NewCutValue);

    // To set cut value specified for each particle
    //  // G4double CutValue_gamma = 3*um;
    SetCutValue(0.1 * nm, "gamma");

    // G4double CutValue_positron =3*um;
    SetCutValue(0.1 * nm, "e+");

    // G4double CutValue_electron =3*um;
    SetCutValue(0.1 * nm, "e-");
  }
  else {
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(1 * eV, 100 * keV);
  }
}

