#ifndef SIMPLECUBE_H
#define SIMPLECUBE_H 1

#include "GeometryManager.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class SimpleCubeMessenger;

class SimpleCube{

public:

    SimpleCube( GeometryManager* gman);
    
    ~SimpleCube();

    void SetMaterial( G4String material );

    void SetPosition( G4ThreeVector s);

    void RotateX( G4double a);
    
    void RotateY( G4double a);
    
    void RotateZ( G4double a);
    

    void Construct();

private:

    G4String thisName;

    SimpleCubeMessenger* fSimpleCubeMessenger;

    G4double L_in, L_out;

    G4bool fCheckOverlaps;

    GeometryManager* gman;
};


#endif
