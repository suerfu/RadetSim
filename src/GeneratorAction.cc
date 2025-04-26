/*
    Author:  Suerfu Burkhant
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file GeneratorAction.cc
/// \brief Implementation of the GeneratorAction class

#include "GeneratorAction.hh"
#include "GeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SPSPosDistribution.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VisExtent.hh"

#include "TKey.h"

// Standard includes
#include <algorithm>
#include <sstream>
#include <iterator>



GeneratorAction::GeneratorAction( RunAction* runAction ) : G4VUserPrimaryGeneratorAction(), fRunAction( runAction) {
    
    fCmdlArgs = fRunAction->GetCommandlineArguments();

    fgun = new G4ParticleGun();
    fgps = new G4GeneralParticleSource();

    file = 0;
    particle = "geantino";

    useGPS = true;
	GPSInMaterial = false;
		// default values.

    primaryGeneratorMessenger = new GeneratorMessenger( this );

    fVolumesInMaterial.clear();
    fCumulativeMaterialVolume = 0;
}


GeneratorAction::~GeneratorAction(){

    if( !file==0 ){
        file->Close();
        delete file;
    }

    delete primaryGeneratorMessenger;
}


void GeneratorAction::SetSpectrum( G4String str ){

    G4cout << "Setting generator spectrum to " << str << G4endl;

    // If a file is already opened, close it first.
    if( file!=0 && file->IsOpen() ){
        file->Close();
    }

    G4cout << "Opening " << str << G4endl;

    // open in read mode (default)
    file = new TFile( str.c_str() );
    if( !file->IsOpen() ){
        G4cerr << "Error opening " << s << G4endl;
        return;
    }

    // Note: the spectrum file should contain only one 2D histogram
    // The histogram should have energy (in keV) and polar angle (in rad) as x and y coordinates.
    //
    TIter next( file->GetListOfKeys() );

    TString T2HF_name;
    TKey *key;
    while( (key=(TKey*)next()) ) {
        T2HF_name=key->GetName();
    }
    hist2D = (TH2F*)file->Get(T2HF_name);

	// When this function is called, particle gun should be used instead of GPS
	// set the corresponding flag variable.
    useGPS = false;
}


// Set the energy from the histogram to have the correct unit.
//
G4double GeneratorAction::SetEnergy( double E ){
    return E * keV;
}


void GeneratorAction::SetParticleName( G4String str ){
     particle = str;
     cout << "Simulated particle is" << str << endl;
}


// Specify the material in which to generate particles (mainly radioactive decays)
// 
void GeneratorAction::GPSSetMaterial( G4String materialName ){
    
    fVolumesInMaterial.clear();
        // clear the previous material mass information.
    
    fCumulativeMaterialVolume = 0;
        // this variable contains the total mass of the material of concern.

    G4cout<<"Generator setting material to be " << materialName << G4endl;

    // Iterate over all physical volumes and add the mass of volumes with matching material.
    //
    G4PhysicalVolumeStore *PVStore = G4PhysicalVolumeStore::GetInstance();
    
    G4int i = 0;

    while( i<(G4int)PVStore->size() ){

        G4VPhysicalVolume* pv = (*PVStore)[i];

        G4cout << "Checking " << pv->GetName() << G4endl;

        if (pv->GetLogicalVolume()->GetMaterial()->GetName() == materialName) {
            fVolumesInMaterial.push_back(pv);
                //fCumulativeMaterialVolume+=pv->GetLogicalVolume()->GetSolid()->GetCubicVolume()/CLHEP::cm3;
            fCumulativeMaterialVolume+=pv->GetLogicalVolume()->GetMass( false, false )/CLHEP::kg;
        }

        i++;
    }

    if( fVolumesInMaterial.empty() ){
        G4cout << "Generator::GPSInMaterial::SetMaterial did not find volume made of '" + materialName + "'" << G4endl;
        throw std::runtime_error("Generator::GPSInMaterial::SetMaterial did not find volume made of '" + materialName + "'");
    }

	// Set the correct flags.
    useGPS = true;
    GPSInMaterial = true;
}


// This function randomly returns a pointer to physical volume 
// with probability corresponding to the mass of the volume
//
G4VPhysicalVolume* GPSInMaterialPickVolume( double CumulativeMaterialVolume,  std::vector<G4VPhysicalVolume*> VolumesInMaterial ) {

    // Generate a random variable 
    G4double r = CLHEP::RandFlat::shoot(0.,CumulativeMaterialVolume);
    G4int selectedVolume=0;
    G4double sum=0;
    do {
        //sum+=VolumesInMaterial[selectedVolume]->GetLogicalVolume()->GetSolid()->GetCubicVolume()/CLHEP::cm3;
        sum+=VolumesInMaterial[selectedVolume]->GetLogicalVolume()->GetMass( false, false )/CLHEP::kg;
        ++selectedVolume;
    } while(sum<r);

    return VolumesInMaterial[selectedVolume-1];
}


void GeneratorAction::GeneratePrimaries( G4Event* anEvent ){
    
	// If using particle gun, sample E and theta from the spectrum
	//
    if( useGPS == false ){
        hist2D->GetRandom2( Energy, Theta );
        fgun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( particle ) );
        fgun->SetParticleMomentumDirection( G4ThreeVector(0, sin(Theta), cos(Theta)) );
        fgun->SetParticleEnergy( SetEnergy( Energy ) );
        fgun->GeneratePrimaryVertex( anEvent );
    }

	// Otherwise, if in material, get the source
	//
    else if ( useGPS == true && GPSInMaterial == true ) {

        if( fVolumesInMaterial.empty() ){
            throw std::runtime_error( "Generator::GPSInMaterial::GeneratePrimaries : no material set");
        }

        G4VPhysicalVolume* selectedVolume = GPSInMaterialPickVolume( fCumulativeMaterialVolume,  fVolumesInMaterial );

        G4ThreeVector selectedVolumePosition = GeometryManager::GetGlobalPosition( selectedVolume );

        G4VisExtent extent = selectedVolume->GetLogicalVolume()->GetSolid()->GetExtent();

        G4ThreeVector translationToVolumeCenter( (extent.GetXmax()+extent.GetXmin())/2., (extent.GetYmax()+extent.GetYmin())/2., (extent.GetZmax()+extent.GetZmin())/2. );

        G4SPSPosDistribution* pd= fgps->GetCurrentSource()->GetPosDist();
        pd->ConfineSourceToVolume( selectedVolume->GetName() );
        pd->SetPosDisType( "Volume" );
        pd->SetPosDisShape( "Para" );
        pd->SetCentreCoords( selectedVolumePosition + translationToVolumeCenter );
        pd->SetHalfX( (extent.GetXmax()-extent.GetXmin()) / 2. );
        pd->SetHalfY( (extent.GetYmax()-extent.GetYmin()) / 2. );
        pd->SetHalfZ( (extent.GetZmax()-extent.GetZmin()) / 2. );

        /*
        G4cout << "GPSInMaterial confined to volume " << selectedVolume->GetName() << G4endl;
        G4cout << selectedVolumePosition << "\t trans. to vol center : " << translationToVolumeCenter << G4endl;
        G4cout << "extent is " << (extent.GetXmax()-extent.GetXmin()) / 2. << ' ' 
            << (extent.GetYmax()-extent.GetYmin()) / 2. << ' ' 
            << (extent.GetZmax()-extent.GetZmin()) / 2. << G4endl;
        */

        fgps->GeneratePrimaryVertex( anEvent );
    }

	// in this case, user will be setting everything from macros.
	//
    else{
        fgps->GeneratePrimaryVertex( anEvent );
    }

}



