/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file GeometryConstruction.hh
/// \brief Definition of the GeometryConstruction class

#ifndef GEOMETRYCONSTRUCTION_H
#define GEOMETRYCONSTRUCTION_H 1

#include "G4VUserDetectorConstruction.hh"
#include "GeometryManager.hh"

#include "SimpleCube.hh"
#include "FarsideDetectorMessenger.hh"

#include "globals.hh"


class GeometryConstructionMessenger;


/// Detector construction class to define materials and geometry.



class GeometryConstruction : public G4VUserDetectorConstruction{

public:

    GeometryConstruction( GeometryManager* man );

    virtual ~GeometryConstruction();

public:

    virtual G4VPhysicalVolume* Construct();
        // This method calls DefineMaterials and DefineVolumes successively.

    G4VPhysicalVolume* ConstructWorld();

    void ConstructUserVolumes();

    //G4VIStore* CreateImportanceStore();


private:

    GeometryManager* fGeometryManager;
    GeometryConstructionMessenger* fDetectorMessenger;

    FarsideDetectorMessenger* fFarsideMessenger;

    bool fCheckOverlaps;

    G4double world_x;
    G4double world_y;
    G4double world_z;

    SimpleCube* simple_cube;

};


#endif
