
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

#include <string>

#include "RunAction.hh"

#include "TFile.h"
#include "TTree.h"

class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;
class GeneratorMessenger;

using std::string;

class GeneratorAction : public G4VUserPrimaryGeneratorAction {

public:

    GeneratorAction( RunAction* runAction);

    virtual ~GeneratorAction();

    virtual void GeneratePrimaries(G4Event* event);

    void SetSpectrum( string str );
        // used to specify the gamma spectrum from which to sample energy & momentum

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

    G4ParticleGun*  fgun;
    G4GeneralParticleSource*  fgps;

    bool use_gps;

    TFile* file;
        // Pointer to the ROOT file.

    TTree* tree;
        // Pointer to the TTree object.

    int nentries;
        // No. of entries in the root tree.
    int index;
        // Following variables used to read and hold sampled values from the ROOT file.
    char particle[16];
        // Name of the particle
    int nparticle;
        // No. of particle generated in this event.
    double x;
    double y;
    double z;
        // Location of the particle
    double px;
    double py;
    double pz;
        // Momentmum
    double E;
        // Energy
    double theta;
    double phi;
        // Polar angle with respect to z direction

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
