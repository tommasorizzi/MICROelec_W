#include "physicsMicroElecNew.hh"
#include "G4SystemOfUnits.hh"


// Geant4-MicroElec MODELS

#include "G4MicroElecElastic.hh"
#include "G4MicroElecElasticModel_new.hh"

#include "G4MicroElecInelastic.hh"
#include "G4MicroElecInelasticModel_new.hh"

#include "G4MicroElecLOPhononScattering.hh"
#include "G4MicroElecLOPhononModel.hh"
#include "G4MicroElecSurface.hh"

//

#include "G4LossTableManager.hh"
#include "G4EmConfigurator.hh"
#include "G4VEmModel.hh"
#include "G4DummyModel.hh"
#include "G4CoulombScattering.hh"
#include "G4eDPWACoulombScatteringModel.hh"
#include "G4eIonisation.hh"
#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4eMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4BraggModel.hh"
#include "G4BraggIonModel.hh"
#include "G4BetheBlochModel.hh"
#include "G4UrbanMscModel.hh"
#include "G4MollerBhabhaModel.hh"
#include "G4IonFluctuations.hh"
#include "G4UniversalFluctuation.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4eCoulombScatteringModel.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4LivermoreRayleighModel.hh"
#include "G4PenelopeGammaConversionModel.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4SeltzerBergerModel.hh"






#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"
#include "G4ComptonScattering.hh"
#include "G4PenelopeComptonModel.hh"
#include "G4PenelopePhotoElectricModel.hh"
#include "G4PenelopeRayleighModel.hh"

#include "G4eBremsstrahlung.hh"
#include "G4PenelopeBremsstrahlungModel.hh"

#include "G4MicroElecCapture.hh"

#include "G4UAtomicDeexcitation.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicroElecPhysics::MicroElecPhysics() : G4VUserPhysicsList()
{
  defaultCutValue = 1 * micrometer;
  cutForGamma = defaultCutValue;
  cutForElectron = defaultCutValue;
  cutForPositron = defaultCutValue;
  cutForProton = defaultCutValue;

  //G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(5 * eV, 10 * MeV);

  SetVerboseLevel(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicroElecPhysics::~MicroElecPhysics() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::ConstructParticle()
{
  ConstructBosons();
  ConstructLeptons();
  ConstructBarions();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::ConstructBosons()
{
  // gamma
  G4Gamma::GammaDefinition();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::ConstructLeptons()
{
  // leptons
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::ConstructBarions()
{
  //  baryons
  G4Proton::ProtonDefinition();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::ConstructProcess()
{
  AddTransportation();
  ConstructEM();
  ConstructGeneral();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::ConstructEM()
{
  G4EmParameters* param = G4EmParameters::Instance();
  // param->SetDefaults();
  param->SetBuildCSDARange(true);
  // param->SetMscStepLimitType(fUseSafetyPlus);
  // param->SetMscStepLimitType(fUseDistanceToBoundary);
  param->SetMscStepLimitType(fUseSafety);
  param->RegionsMicroElec();
  // physicList ISS
  param->SetDefaults();
  param->SetMinEnergy(0.1 * eV);
  param->SetMaxEnergy(10 * TeV);
  param->SetLowestElectronEnergy(0 * eV);  //<--- Energie de cut dans le vide!!! A fixer  0eV pour ne pas fausser les SEY
  param->SetNumberOfBinsPerDecade(20);
  param->ActivateAngularGeneratorForIonisation(true);
  param->SetAugerCascade(true);
 // param->SetUseMottCorrection(true);  //*/
  

  auto particleIterator = GetParticleIterator();
  particleIterator->reset();

  while ((*particleIterator)()) {
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    // *********************************
    // 1) Processes for the World region
    // *********************************

    if (particleName == "e-") {
      // STANDARD msc is active in the world
      G4eMultipleScattering* msc = new G4eMultipleScattering();
      msc->AddEmModel(1, new G4UrbanMscModel());
      pmanager->AddProcess(msc, -1, 1, -1);

      // STANDARD ionisation is active in the world
      G4eIonisation* eion = new G4eIonisation();
      pmanager->AddProcess(eion, -1, 2, 2);

      G4eBremsstrahlung* brem = new G4eBremsstrahlung();
      pmanager->AddProcess(brem, -1, 3, 3);
    
      G4CoulombScattering* cs = new G4CoulombScattering();
      cs->SetEmModel(new G4eCoulombScatteringModel());
      pmanager->AddDiscreteProcess(cs);

      // MicroElec elastic is not active in the world
      // G4MicroElecElasticCorrected* theMicroElecElasticProcess = new
      // G4MicroElecElasticCorrected("e-_G4MicroElecElastic");
      // theMicroElecElasticProcess->SetEmModel(new G4DummyModel(),1);
      // pmanager->AddDiscreteProcess(theMicroElecElasticProcess);

      G4MicroElecElastic* theMicroElecElasticProcess =
        new G4MicroElecElastic("e-_G4MicroElecElastic");
      theMicroElecElasticProcess->SetEmModel(new G4DummyModel(), 1);
      // G4MicroElecElasticModel_new* mod = new G4MicroElecElasticModel_new();
      // theMicroElecElasticProcess->AddEmModel(0,mod);
      pmanager->AddDiscreteProcess(theMicroElecElasticProcess);

      // MicroElec ionisation is not active in the world
      /*G4MicroElecInelastic* microelecioni = new G4MicroElecInelastic("e-_G4MicroElecInelastic");
      microelecioni->SetEmModel(new G4DummyModel(),1);
      pmanager->AddDiscreteProcess(microelecioni);*/

      G4MicroElecInelastic* microelecioni = new G4MicroElecInelastic("e-_G4Dielectrics");
      microelecioni->SetEmModel(new G4DummyModel(), 1);
      pmanager->AddDiscreteProcess(microelecioni);

      // Phonons for SiO2

      G4MicroElecLOPhononScattering* opticalPhonon =
        new G4MicroElecLOPhononScattering("e-_G4LOPhononScattering");
      opticalPhonon->SetEmModel(new G4DummyModel(), 1);
      pmanager->AddDiscreteProcess(opticalPhonon);

      /*G4LOPhononScattering* LO60 = new G4LOPhononScattering("e-_G4LO60");
      LO60->SetEmModel(new G4DummyModel(), 1);
      pmanager->AddDiscreteProcess(LO60);//*/

      G4MicroElecSurface* MicroElecSurf = new G4MicroElecSurface("e-_G4MicroElecSurface");
      MicroElecSurf->SetProcessManager(pmanager);
      pmanager->AddDiscreteProcess(MicroElecSurf);  //*/

      G4MicroElecCapture* ecap =
        new G4MicroElecCapture("Target", 0.9 * eV);  //<--- Piges pour Al2O3
      pmanager->AddDiscreteProcess(ecap);  //*/
    }
    else if (particleName == "proton") {
      // STANDARD msc is active in the world
      /*G4hMultipleScattering* msc = new G4hMultipleScattering();
      msc->AddEmModel(1, new G4UrbanMscModel());
      pmanager->AddProcess(msc, -1, 1, -1);*/

      // STANDARD ionisation is active in the world
      G4hIonisation* hion = new G4hIonisation();
      pmanager->AddProcess(hion, -1, 2, 2);

      //// Dielectric ionisation is not active in the world
      //G4MicroElecInelastic* dielectricioni = new G4MicroElecInelastic("p_G4Dielectrics");
      //dielectricioni->SetEmModel(new G4DummyModel(), 1);
      //// dielectricioni->SetEmModel(new G4DummyModel(),2);
      //pmanager->AddDiscreteProcess(dielectricioni);

      G4MicroElecCapture* pcap = new G4MicroElecCapture("Target", 100.1 * eV);
      pmanager->AddDiscreteProcess(pcap);  //*/
    }
    else if (particleName == "alpha") {
      // STANDARD ionisation is active in the world
      G4ionIonisation* hion = new G4ionIonisation();
      pmanager->AddProcess(hion, -1, 2, 2);
      //// Dielectric ionisation is not active in the world
      //G4MicroElecInelastic* dielectricioni = new G4MicroElecInelastic("alpha_G4Dielectrics");
      //dielectricioni->SetEmModel(new G4DummyModel(), 1);
      //dielectricioni->SetEmModel(new G4DummyModel(), 2);
      //pmanager->AddDiscreteProcess(dielectricioni);
    }
    else if (particleName == "GenericIon") {
      // STANDARD msc is active in the world
      /*G4hMultipleScattering* msc = new G4hMultipleScattering();
      msc->AddEmModel(1, new G4UrbanMscModel());
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, -1);*/

      /*G4CoulombScattering* cs = new G4CoulombScattering();
      cs->AddEmModel(0, new G4IonCoulombScatteringModel());
      cs->SetBuildTableFlag(false);
      pmanager->AddDiscreteProcess(cs);*/

      // STANDARD ionisation is active in the world
      G4ionIonisation* hion = new G4ionIonisation();
      pmanager->AddProcess(hion, -1, 2, 2);

      //// Dielectric ionisation is not actived in the world
      //G4MicroElecInelastic* dielectricioni = new G4MicroElecInelastic("ion_G4Dielectrics");
      //dielectricioni->SetEmModel(new G4DummyModel(), 1);
      //dielectricioni->SetEmModel(new G4DummyModel(), 2);
      //pmanager->AddDiscreteProcess(dielectricioni);
    }
  }

  // **************************************
  // 2) Define processes for Target region
  // **************************************

  // STANDARD EM processes should be inactivated when corresponding MicroElec processes are used
  // - STANDARD EM e- processes are inactivated below 100 MeV
  // - STANDARD EM proton & ion processes are inactivated below standEnergyLimit
  //
  G4EmConfigurator* em_config = G4LossTableManager::Instance()->EmConfigurator();

  G4VEmModel* mod;
  // *** e-

  // ---> STANDARD EM processes are inactivated below 100 MeV


 // G4DummyModel* dummyMsc = new G4DummyModel();
 // em_config->SetExtraEmModel("e-", "msc", dummyMsc, "Target", 0*eV, 10*TeV);

  // Then set up single scattering
  //G4CoulombScattering* ss = new G4CoulombScattering();
  //G4VEmModel* coulombModel = nullptr;
  //if (param->UseMottCorrection()) {
  //  coulombModel = new G4eDPWACoulombScatteringModel();
  //}
  //else {
  //  coulombModel = new G4eCoulombScatteringModel();
  //}
  //coulombModel->SetActivationLowEnergyLimit(7 * keV);
  //ss->SetEmModel(coulombModel);

  // Apply Coulomb scattering to Target region for 7 keV - 100 MeV range
  //em_config->SetExtraEmModel("e-", "CoulombScat", coulombModel, "Target", 7 * keV, 100 * MeV);





  G4UrbanMscModel* msc = new G4UrbanMscModel();
  msc->SetActivationLowEnergyLimit(7 * keV);
  em_config->SetExtraEmModel("e-", "msc", msc, "Target", 7 *keV, 100 * MeV);






  // mod = new G4MollerBhabhaModel();
  // mod->SetActivationLowEnergyLimit(10 * keV);
  // em_config->SetExtraEmModel("e-", "eIoni", mod, "Target", 10 * keV, 10 * TeV,
  //                            new G4UniversalFluctuation());

  mod = new G4PenelopeIonisationModel();
  mod->SetActivationLowEnergyLimit(7 * keV);
  mod->SetHighEnergyLimit(1 * MeV);
  em_config->SetExtraEmModel("e-", "eIoni", mod, "Target", 7 * keV, 1 * MeV,
                          new G4UniversalFluctuation());

      
  mod = new G4eCoulombScatteringModel();
  mod->SetActivationLowEnergyLimit(100 * MeV);
  em_config->SetExtraEmModel("e-", "CoulombScat", mod, "Target", 100 * MeV, 1* TeV);
  
  // brem
  mod = new G4SeltzerBergerModel();
  mod->SetActivationLowEnergyLimit(7 * keV);
  mod->SetHighEnergyLimit(1 * GeV);
  em_config->SetExtraEmModel("e-", "eBrem", mod, "Target", 7 * keV, 1 * GeV);

                        
  // ---> MicroElec processes activated

  mod = new G4MicroElecElasticModel_new();
  em_config->SetExtraEmModel("e-", "e-_G4MicroElecElastic", mod, "Target", 0.1 * eV, 7 * keV);

  mod = new G4MicroElecInelasticModel_new();
  //std::cout << "SIUM"
  em_config->SetExtraEmModel("e-", "e-_G4Dielectrics", mod, "Target", 0.1 * eV, 7 * keV);

  //  G4double hw = 0.15*eV;

  // Old phonon

  /*mod = new LOPhononModel(0.153*eV,false);
  em_config->SetExtraEmModel("e-", "e-_G4LOPhononScattering", mod, "Target", 0.153*eV, 10 * MeV);

  mod = new LOPhononModel(0.063*eV,false);
  em_config->SetExtraEmModel("e-", "e-_G4LO60", mod, "Target", 0.06*eV, 10 * MeV);//*/

  // Phonons LO pour sio2 et al2o3

  // mod = new G4MicroElecLOPhononModel();
  // //mod->SetActivationLowEnergyLimit(5 * eV);
  // em_config->SetExtraEmModel("e-", "e-_G4LOPhononScattering", mod, "Target", 0.1 * eV,
  //                           10*MeV);  //*/

  // *** proton ----------------------------------------------------------

  // ---> STANDARD EM processes inactivated below standEnergyLimit

  // STANDARD msc is still active
  // Inactivate following STANDARD processes

  // il faut desactiver Bragg puisque notre modle descend en-dessous de 50 keV
  /*mod = new G4BraggModel();
  mod->SetActivationHighEnergyLimit(50*keV);
  em_config->SetExtraEmModel("proton","hIoni",mod,"Target",0.0,2*MeV, new G4IonFluctuations());*/

  mod = new G4BetheBlochModel();
  mod->SetActivationLowEnergyLimit(10 * MeV);
  em_config->SetExtraEmModel("proton", "hIoni", mod, "Target", 2 * MeV, 10 * TeV,
                             new G4IonFluctuations());

  // ---> Dielectric processes activated

  mod = new G4MicroElecInelasticModel_new();
  mod->SetActivationLowEnergyLimit(100 * eV);
  em_config->SetExtraEmModel("proton", "p_G4Dielectrics", mod, "Target", 100 * eV, 7 * keV);
  // em_config->SetExtraEmModel("proton","p_G4Dielectrics",new G4DummyModel,"Target",10*MeV,10*TeV);

  //*/

  // *** alpha ----------------------------------------------------------
  mod = new G4BetheBlochModel();
  mod->SetActivationLowEnergyLimit(10 * MeV);
  em_config->SetExtraEmModel("alpha", "ionIoni", mod, "Target", 10 * MeV, 10 * TeV,
                             new G4IonFluctuations());

  /*mod = new G4MicroElecInelasticModel_new();
  //mod->SetActivationLowEnergyLimit(100*eV);
  em_config->SetExtraEmModel("alpha","alpha_G4Dielectrics",mod,"Target",0.0,10*MeV);//*/

  // *** ion ----------------------------------------------------------

  // ---> STANDARD EM processes inactivated below standEnergyLimit

  // STANDARD msc is still active
  // Inactivate following STANDARD processes

  /*mod = new G4BraggIonModel();
  mod->SetActivationHighEnergyLimit(50*keV);
  em_config->SetExtraEmModel("GenericIon","ionIoni",mod,"Target",0.0,2*MeV, new
  G4IonFluctuations());*/

  mod = new G4BetheBlochModel();
  mod->SetActivationLowEnergyLimit(10 * MeV);
  em_config->SetExtraEmModel("GenericIon", "ionIoni", mod, "Target", 10 * MeV, 10 * TeV,
                             new G4IonFluctuations());

  // ---> Dielectric processes activated
  mod = new G4MicroElecInelasticModel_new();
  mod->SetActivationLowEnergyLimit(100 * eV);
  em_config->SetExtraEmModel("GenericIon", "ion_G4Dielectrics", mod, "Target", 0.0, 7 * keV);
  // em_config->SetExtraEmModel("GenericIon","ion_G4Dielectrics",new
  // G4DummyModel,"Target",10*GeV,10*TeV);

  // *** gamma ----------------------------------------------------------

  G4VEmModel* comptonModel = new G4LivermoreComptonModel();
  comptonModel->SetLowEnergyLimit(7 * keV);
  em_config->SetExtraEmModel("gamma", "compt", comptonModel, "Target", 7 * keV, 10 * GeV);

  G4VEmModel* photoModel = new G4LivermorePhotoElectricModel();
  photoModel->SetLowEnergyLimit(7 * keV);
  em_config->SetExtraEmModel("gamma", "phot", photoModel, "Target", 7 * keV, 10 * GeV);

  G4VEmModel* convModel = new G4PenelopeGammaConversionModel();
  convModel->SetLowEnergyLimit(7 * keV);
  em_config->SetExtraEmModel("gamma", "conv", convModel, "Target", 7 * keV, 10 * GeV);

  G4VEmModel* rayleighModel = new G4LivermoreRayleighModel();
  rayleighModel->SetLowEnergyLimit(7 * keV);
  rayleighModel->SetHighEnergyLimit(10 * GeV);
  em_config->SetExtraEmModel("gamma", "Rayleigh", rayleighModel, "Target", 7 * keV, 10 * GeV);





  // Deexcitation
  //
  G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  G4LossTableManager::Instance()->SetAtomDeexcitation(de);
  de->SetFluo(true);
  de->SetAuger(true);
  de->SetPIXE(true);
  de->InitialiseForNewRun();

  // G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(5*eV, 100.0*GeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::ConstructGeneral() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicroElecPhysics::SetCuts()
{
  if (verboseLevel > 0) {
    G4cout << "MicroElecPhysics::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue, "Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
  SetCutValue(cutForProton, "proton");



  if (verboseLevel > 0) { DumpCutValuesTable(); }
}
