/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

// $Id: GeneratorAction.hh $
//
/// \file GeneratorAction.hh
/// \brief Definition of the GeneratorAction class

#ifndef GeneratorAction_h
#define GeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4Box.hh"
#include "G4GeneralParticleSource.hh"

#include "GeometryManager.hh"

#include "RunAction.hh"

#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"

class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;
class GeneratorMessenger;

using std::string;

class GeneratorAction : public G4VUserPrimaryGeneratorAction {

public:

    GeneratorAction( RunAction* runAction );

    virtual ~GeneratorAction();

    virtual void GeneratePrimaries( G4Event* event );

    void SetSpectrum( G4String str );
        // used to specify the gamma spectrum from which to sample energy & momentum

    void SetParticleName( G4String name );
        // used to specigy the particle being simulated
    	// when spectrum is used, particle name information is not available.

    G4double SetEnergy( double E );
        // sets the energy by taking care of the unit
        // ROOT file has unit keV
    
    void GPSSetMaterial( G4String materialName );
        // this function samples particle position based on material instead of volume.
    
private:

    GeneratorMessenger* primaryGeneratorMessenger;

    RunAction* fRunAction;

    CommandlineArguments* fCmdlArgs;

    GeometryManager* fGeometryManager;

    G4ParticleGun*  fgun;

    G4GeneralParticleSource*  fgps;

    bool use_gps;

    bool sample;
        // if true, particle energy vs angle will be sampled from histogram
    
    bool GPSInMaterial;
        // if true, particles will be generated based on material

    double fCumulativeMaterialVolume;
        // total mass (actually) corresponding to a certain material
    
    std::vector<G4VPhysicalVolume*> fVolumesInMaterial;
        // this vector contains list of volumes that uses a certain material
        // this is updated each time the function is invoked.

    G4String particle;

    TFile* file;
        // Pointer to the ROOT file containing 2D histogram.

    TH2F* hist2D;
        // Pointer to the 2d histogram containing energy vs angle

    G4double Energy;
        // Energy

    G4double Theta;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
