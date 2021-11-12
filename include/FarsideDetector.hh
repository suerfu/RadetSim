#ifndef FARSIDEDETECTOR_H
#define FARSIDEDETECTOR_H 1

#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "GeometryManager.hh"

class FarsideDetector{

public:
    
    FarsideDetector( GeometryManager* gm );

    ~FarsideDetector(){}

    void PlaceDetector( G4String name, G4ThreeVector, G4RotationMatrix* );

private:

    GeometryManager* fGeometryManager;

    G4double radius;
    G4double height;

};

#endif
