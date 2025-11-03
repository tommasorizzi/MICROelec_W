//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4MicroElecCapture.cc,v 1.1 2010-08-31 11:23:58 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4MicroElecCapture
//
// Description: The process to kill particles to save CPU
//
// Author:      V.Ivanchenko 31 August 2010 modified and adapted to MicorElec by C. Inguimbert 321/01/2022
//
//----------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4MicroElecCapture.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4PhysicalConstants.hh"

#include "G4Track.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4Electron.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4MicroElecCapture::G4MicroElecCapture(const G4String& regName, G4double ekinlim)
  : G4VDiscreteProcess("MicroElecCapture", fElectromagnetic), kinEnergyThreshold(ekinlim),
    regionName(regName), region(0)
{
  if(regName == "" || regName == "world") { 
    regionName = "DefaultRegionForTheWorld";
  }
  isInitialised = false;
  pParticleChange = &fParticleChange;
  G4cout << ">>> Using MyCaptureModel!" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4MicroElecCapture::~G4MicroElecCapture() 
{

	std::map< G4String, G4MicroElecMaterialStructure*, std::less<G4String> >::iterator pos;
	for (pos = tableWF.begin(); pos != tableWF.end(); ++pos)
	{
		G4MicroElecMaterialStructure* table = pos->second;
		delete table;
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4MicroElecCapture::SetKinEnergyLimit(G4double val)
{
  kinEnergyThreshold = val;
 // if(verboseLevel > 0) {
 //    G4cout << "### G4MicroElecCapture: Tracking cut E(MeV) = " 
 //	   << kinEnergyThreshold/MeV << G4endl;
//system("PAUSE");
 // }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4MicroElecCapture::BuildPhysicsTable(const G4ParticleDefinition&)
{
  region = (G4RegionStore::GetInstance())->GetRegion(regionName);
 // if(region && verboseLevel > 0) {
    G4cout << "### G4MicroElecCapture: Tracking cut E(MeV) = " 
	   << kinEnergyThreshold/MeV << " is assigned to " << regionName 
	   << G4endl;
	//system("PAUSE");
 // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool G4MicroElecCapture::IsApplicable(const G4ParticleDefinition&)
{
  return true;
}


void G4MicroElecCapture::Initialise()
{
	if (isInitialised) { return; }

	G4ProductionCutsTable* theCoupleTable =
		G4ProductionCutsTable::GetProductionCutsTable();
	G4int numOfCouples = theCoupleTable->GetTableSize();
	G4cout << numOfCouples << G4endl;

	for (G4int i = 0; i < numOfCouples; ++i) {
		const G4Material* material =
			theCoupleTable->GetMaterialCutsCouple(i)->GetMaterial();

		G4cout << "G4Capture, Material " << i + 1 << " / " << numOfCouples << " : " << material->GetName() << G4endl;
		if (material->GetName() == "Vacuum") { tableWF[material->GetName()] = 0; continue; }
		G4String mat = material->GetName();
		G4MicroElecMaterialStructure* str = new G4MicroElecMaterialStructure(mat);
		//tableWF[mat] = str->GetWorkFunction();
		tableWF[mat] = str;

		//delete str;
	}



	isInitialised = true;
}
/*
G4double 
G4MicroElecCapture::PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
							G4double, 
							G4ForceCondition* condition)
{
	if (!isInitialised) { Initialise(); }

  // condition is set to "Not Forced"
  *condition = NotForced;
  G4String mat = aTrack.GetMaterial()->GetName();
  G4double limit = DBL_MAX; 
  //G4MicroElecMaterialStructure structure = G4MicroElecMaterialStructure(mat);

		   if (aTrack.GetVolume()->GetLogicalVolume()->GetRegion() == region &&
			   aTrack.GetKineticEnergy() < kinEnergyThreshold) {
			   limit = 0.0;
			  
			   if (aTrack.GetKineticEnergy() < 0) {
				   G4cout << "capture, energy = " << aTrack.GetKineticEnergy() / eV << G4endl;
				   system("pause");
			   }
		   }
	   
  return limit;
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VParticleChange* G4MicroElecCapture::PostStepDoIt(const G4Track& aTrack, 
						   const G4Step&)
{
	if (!isInitialised) { Initialise(); }

	G4String mat = aTrack.GetMaterial()->GetName();
	G4int Ztarget = ((*(aTrack.GetMaterial()->GetElementVector()))[0])->GetZ();
	G4int Atarget = ((*(aTrack.GetMaterial()->GetElementVector()))[0])->GetAtomicMassAmu();
	G4double Nbelements = aTrack.GetMaterial()->GetNumberOfElements();
	G4double moleculeMass = aTrack.GetMaterial()->GetMassOfMolecule() / amu;
	G4double * FractionMass = const_cast<double*> (aTrack.GetMaterial()->GetFractionVector());
	G4int Zinc = aTrack.GetParticleDefinition()->GetAtomicNumber();
	G4int Ainc = aTrack.GetParticleDefinition()->GetAtomicMass();
	G4String IncPartName = aTrack.GetParticleDefinition()->GetParticleName();
	G4double NIEdep = 0.0;


	for (int i = 0; i < Nbelements; i++) {
		Ztarget = ((*(aTrack.GetMaterial()->GetElementVector()))[i])->GetZ();
		Atarget = ((*(aTrack.GetMaterial()->GetElementVector()))[i])->GetAtomicMassAmu();
		NIEdep = NIEdep + moleculeMass*FractionMass[i] / Atarget*G_Lindhard_Rob(aTrack.GetKineticEnergy(), Zinc, Ainc, Ztarget, Atarget);
	}

	

	WorkFunctionTable::iterator matWF;
	matWF = tableWF.find(mat);


	if (matWF == tableWF.end()) {
		G4String str = "Material ";
		str += mat + " not found!";
		G4Exception("G4MicroElecCapture::PostStepGPIL", "em0002", FatalException, str);
		return 0;
	}

	else
	{
		G4MicroElecMaterialStructure* str = matWF->second;
		pParticleChange->Initialize(aTrack);
		pParticleChange->ProposeTrackStatus(fStopAndKill);
		
		G4double InitE = str->GetEnergyGap() + str->GetInitialEnergy();
		
		
		if (IncPartName == "e-"){
				// metals = Non ionizing deposited energy = 0.0
				if (((str->GetEnergyGap()) / eV)<(0.001)) {
					pParticleChange->ProposeNonIonizingEnergyDeposit(0.0);
					pParticleChange->ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy());
				}
				else { // MicroElec materials Non ionizing deposited energy different from zero
					int c = (int)((aTrack.GetKineticEnergy()) / (InitE));
					pParticleChange->ProposeNonIonizingEnergyDeposit(aTrack.GetKineticEnergy() - InitE*c);
					pParticleChange->ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy());
				}
		}
		else{
				if ((IncPartName == "Genericion") || (IncPartName == "alpha") || (IncPartName == "He3") || (IncPartName == "deuteron") || (IncPartName == "triton") || (IncPartName == "proton")) {
					pParticleChange->ProposeNonIonizingEnergyDeposit(NIEdep);
					pParticleChange->ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy());
				}
				else {
					pParticleChange->ProposeNonIonizingEnergyDeposit(0.0);
					pParticleChange->ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy());
				}
			}
			
			
		} //matWF == tableWF.end())
		
		

		fParticleChange.SetProposedKineticEnergy(0.0);
		return pParticleChange;
		
	
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4MicroElecCapture::GetMeanFreePath(const G4Track& aTrack,G4double,
					    G4ForceCondition*)
{											
  G4String material = aTrack.GetMaterial()->GetName();
 // test particle type in order to applied the capture to both electrons, protons and heavy ions
  
  if ((aTrack.GetParticleDefinition()->GetParticleName()) == "e-") {


	  if (material != "G4_ALUMINUM_OXIDE" && material != "G4_SILICON_DIOXIDE" && material != "G4_BORON_NITRIDE") { return DBL_MAX; }
	  G4double    S = 0;
	  G4double    y = 0;
	  if (material == "G4_ALUMINUM_OXIDE") {
		  S = 1 * (1 / nm);
		  y = 0.25 * (1 / eV);
	  }
	  if (material == "G4_SILICON_DIOXIDE") {
		  S = 0.3 * (1 / nm);
		  y = 0.2 * (1 / eV);
	  }
	  if (material == "G4_BORON_NITRIDE") {
		  S = 0 * (1 / nm);
		  y = 1 * (1 / eV);
	  }

	  G4double P = S * exp(-y * aTrack.GetKineticEnergy());
	  if (P <= 0) { return DBL_MAX; }
	  else { return 1 / P; }
  }
  else return DBL_MAX;
  
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



G4double G4MicroElecCapture::G_Lindhard_Rob(G4double Trecoil, G4int Zrecoil, G4int Arecoil, G4int Zcible, G4int Acible)
{
    
    G4double Lind =0.0;

    if (Arecoil<=0 || Zrecoil==0) {Lind=0.0;}
    else{

        G4double El = 30.724*Zcible*Zrecoil*pow(  (pow(Zcible,2./3.)+pow(Zrecoil,2./3.))  , 0.5)*(Arecoil+Acible)/Acible;
   
   
        // multiplication by 1e6 to change El from eV to MeV
        G4double e  = Trecoil/El*1000000.;
        G4double Fl = (0.0793*pow(Zrecoil,2./3.)*pow(Zcible,0.5)*pow((Arecoil+Acible),1.5))/
                    ( pow(  (pow(Zcible,2./3.)+pow(Zrecoil,2./3.)), 3./4.)* pow(Arecoil,3./2.)* pow(Acible,1./2.) );

        Lind      = 1./( 1+Fl*(3.4008*pow(e,1./6.) + 0.40244*pow(e,3./4.)+e) );
   
        // to get the energie that go into displacement
        Lind = Lind *Trecoil ;
    }
    return Lind;                                                                   
}


