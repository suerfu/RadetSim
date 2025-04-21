/*
    Author:  Suerfu Burkhant
    Date:    April 21, 2025
    Contact: suerfu@post.kek.jp
*/

/// \file GeometryConstruction.hh
/// \brief Definition of the GeometryConstruction class

#ifndef GEOMETRYCONSTRUCTION_H
#define GEOMETRYCONSTRUCTION_H 1

#include "G4VUserDetectorConstruction.hh"
#include "GeometryManager.hh"

#include "globals.hh"

#include "NaIDetector.hh"

#include <string>

using std::string;

class GeometryConstructionMessenger;


/// Detector construction class to define materials and geometry.
//
class GeometryConstruction : public G4VUserDetectorConstruction{

public:

    GeometryConstruction( GeometryManager* man );

    virtual ~GeometryConstruction();

public:

    /// An integer is used to identify the type of geometry/work being simulated
    /// This function converts the user-given string to appropriate integer for calling the right function
    /// 0 - rock
    /// 100s - NaI measurement
    ///     101 - 1.5-inch NaI in Kamioka in 202304
    ///     102 -   3-inch NaI in Kamioka in 202504
    int GetGeometryCode( G4String input );

    virtual G4VPhysicalVolume* Construct();
        // This method calls DefineMaterials and DefineVolumes successively.

    G4VPhysicalVolume* ConstructRock();
    
    G4VPhysicalVolume* ConstructCrystal();

    G4VPhysicalVolume* ConstructKamiokaGamma2023();

    G4VPhysicalVolume* ConstructKamiokaGamma2025( int option );

private:

    string GetClassName(){ return "GeometryConstruction"; }

    GeometryManager* fGeometryManager;
    GeometryConstructionMessenger* fDetectorMessenger;

    bool fCheckOverlaps;

    NaIDetector detector_assembly;
};


#endif
