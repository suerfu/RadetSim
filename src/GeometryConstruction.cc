/*
    Author:  Suerfu Burkhant
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file GeometryConstruction.cc
/// \brief Implementation of the GeometryConstruction class

#include "GeometryConstruction.hh"

#include "G4SubtractionSolid.hh"

#include "GeometryConstructionMessenger.hh"
#include "FarsideDetectorMessenger.hh"
#include "FarsideDetector.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//#include "G4IStore.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>



GeometryConstruction::GeometryConstruction( GeometryManager* gm) : G4VUserDetectorConstruction(), fGeometryManager( gm ) {

    fCheckOverlaps = true;
    fDetectorMessenger = new GeometryConstructionMessenger(this);

    // Set default values for world.
    
    world_x = 1.*m;
    world_y = 1.*m;
    world_z = 1.*m;

    simple_cube = new SimpleCube( gm );

    fFarsideMessenger = new FarsideDetectorMessenger( gm );
}



GeometryConstruction::~GeometryConstruction(){
    delete fDetectorMessenger;
    delete fFarsideMessenger;
}



G4VPhysicalVolume* GeometryConstruction::Construct(){

    G4cout << "Constructing geometry...\n";

    G4VPhysicalVolume* world_pv = ConstructWorld();

    /* Other user-defined geometries goes here */
    ConstructUserVolumes();

    return world_pv;
}



G4VPhysicalVolume* GeometryConstruction::ConstructWorld(){

    G4String world_name = "world";
    G4Material* world_material = fGeometryManager->GetMaterial("G4_Galactic");

    G4Box* world_solid = new G4Box( world_name+"_sld", world_x/2.0, world_y/2.0, world_z/2.0);
    G4LogicalVolume* world_lv = new G4LogicalVolume( world_solid, world_material, world_name+"_lv");
    G4VPhysicalVolume* world_pv = new G4PVPlacement( 0, G4ThreeVector(0,0,0), world_lv, world_name, 0, false, 0,fCheckOverlaps);

    world_lv->SetVisAttributes( G4VisAttributes::GetInvisible() );

    return world_pv;
}


void GeometryConstruction::ConstructUserVolumes(){

    simple_cube->Construct();

    const int Nfs = 6;
    FarsideDetector* fs[Nfs];
    for( int i=0; i<Nfs; i++){
        std::stringstream name;
        name << "fs" << i;
        fs[i] = new FarsideDetector( fGeometryManager );

        G4double distance = 50 * cm;
        G4double angle = i*CLHEP::twopi/Nfs;
        G4ThreeVector pos( distance*cos(angle), distance*sin(angle),0 );
        fs[i] -> PlaceDetector( name.str(), pos );
    }

}


/*
G4VIStore* GeometryConstruction::CreateImportanceStore(){
    
    //===============  Importance Sampling to speed up simulation ==============//
    
    G4IStore *istore = G4IStore::GetInstance();

    istore->AddImportanceGeometryCell( 1, *world_pv);
    for( unsigned int i=0; i<list.size(); i++ ){
        istore->AddImportanceGeometryCell( list[i].bias, *list[i].phy);
    }
    
    return istore;
}
*/
