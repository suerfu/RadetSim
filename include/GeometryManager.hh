/*
    Author:  Suerfu Burkhant
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
#include "G4Transform3D.hh"

#include "G4Color.hh"

#include "ConfigParser.hh"

using namespace CLHEP;

/// Singleton class.
/// It manages different attributes of geometry in the simulation/
/// Asstributes includes dimensions, color, material, mass, etc.
//
class GeometryManager{

private:

    GeometryManager();

    ~GeometryManager();

    static GeometryManager* manager;

public:
    
    static GeometryManager* GetGeometryManager();

    static GeometryManager* Get();

    static G4LogicalVolume* GetLogicalVolume( G4String name );
        // return pointer to logical volume by name

    static G4VPhysicalVolume* GetPhysicalVolume( G4String name );
        // return pointer to physical volume by name

    static G4ThreeVector GetGlobalPosition( G4VPhysicalVolume* physVol );
    
    G4NistManager* GetMaterialManager();

    G4Material* GetMaterial( G4String name );

    void GeometryHasBeenModified();
	
    G4int GetGeometryType(){ return fGeometryType; }
    
    void  SetGeometryType( int a ){ fGeometryType = a; }
    
    bool CheckOverlaps(){ return fCheckOverlaps;}
        // returns a global flag regarding overlap check

    void LoadFile( G4String );
        // reads configuration file and stores parameters as name-value pairs
	
    void SetVisAttributes();

private:

    G4NistManager* material_manager;

    /// User should define all materials to be used in this function.
    void DefineMaterials();
	
    std::map< G4String,  G4Color > materialColor;
	
    bool fCheckOverlaps;
	
    int  fGeometryType; 

private:

    ConfigParser config;

public:

    const ConfigParser* GetConfigParser() { return &config; }

};


#endif
