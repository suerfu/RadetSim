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
//#include "G4ThreeVector.hh"
//#include "G4VPhysicalVolume.hh"
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

    GeneratorAction( RunAction* runAction, GeometryManager* geoManager);

    virtual ~GeneratorAction();

    virtual void GeneratePrimaries(G4Event* event);

    void SetSpectrum( G4String str );
        // used to specify the gamma spectrum from which to sample energy & momentum

    void SetParticleName( G4String name );
    	// used to specigy the particle being simulated
	// when spectrum is used, particle name information is not available.

    void Sample( int n = -1 );

    void SetPosition();
        // sample and set position from the specified ROOT file.

    void SetDirection();
        // sample and set momentum direction from the specified ROOT file.

    void SetEnergy();
        // sample and set energy from the specified ROOT file.
    
    void ConfineOnWall();
        // When this method is called, it is assumed that the primary particle position is on the surface of the wall.
        // Momentum direction w.r.t normal of the wall has theta and phi as polar and azimuth angle.

private:

    GeneratorMessenger* primaryGeneratorMessenger;

    RunAction* fRunAction;

    CommandlineArguments* fCmdlArgs;

    GeometryManager* fGeometryManager;

    G4ParticleGun*  fgun;

    G4GeneralParticleSource*  fgps;

    bool use_gps;

    G4String particle;

 
    TFile* file;
        // Pointer to the ROOT file.

    TH2F* hist2D;
        // Pointer to the 2D histogram from which energy and angle are sampled.

    G4double E;
    	// Energy

    G4double theta;
	// polar angle w.r.t. the surface


    bool onwall;
        // Flag variable to denote whether particle position should be independently sampled from world surface.
        // If true, generator will use theta and phi as w.r.t. the normal direction at the sampled position.
        // Otherwise, the position and direction in the ROOT file will be used as it is.
    bool sample;
        // Flag variable to denote whether particle position and momentum should be sampled from ROOT file.

    G4double wall_x;
    G4double wall_y;
    G4double wall_z;
        // Dimentions of the experimental hall.

    G4Box* world;
        // Pointer to the experimental hall object. Used to randomly sample surface points.
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
