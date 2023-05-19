/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H 1

#include <map>
#include <utility>

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4NistManager.hh"

using namespace CLHEP;

/// Singleton class.
/// It manages different attributes of geometry in the simulation/
/// Asstributes includes dimensions, color, material, mass, etc.
class GeometryManager{

private:

    GeometryManager();

    ~GeometryManager();

    static GeometryManager* manager;

public:
    
    static GeometryManager* GetGeometryManager();

    static GeometryManager* Get();

    G4LogicalVolume* GetLogicalVolume( G4String name );
        // return pointer to logical volume by name

    G4VPhysicalVolume* GetPhysicalVolume( G4String name );
        // return pointer to physical volume by name

    G4NistManager* GetMaterialManager();

    G4Material* GetMaterial( G4String name );

    void GeometryHasBeenModified();
    
    bool CheckOverlaps(){ return fCheckOverlaps;}
        // returns a global flag regarding overlap check


private:

    G4NistManager* material_manager;

    /// User should define all materials to be used in this function.
    void DefineMaterials();
	std::map< G4String,  G4Color > materialColor;
	
    bool fCheckOverlaps;

};


#endif
