
/// \file GeometryConstruction.cc
/// \brief Implementation of the GeometryConstruction class

#include "GeometryConstruction.hh"

#include "G4SubtractionSolid.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "GeometryConstructionMessenger.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "Randomize.hh"

#include "G4IStore.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


GeometryConstruction::GeometryConstruction( /*RunAction* ra,*/ GeometryManager* gm) : G4VUserDetectorConstruction(),
//    fRunAction( ra ), 
    fGeometryManager( gm ) {

    fCheckOverlaps = true;
    fDetectorMessenger = new GeometryConstructionMessenger(this);

    // Set default values for world.
    
    world_x = 1.*m;
    world_y = 1.*m;
    world_z = 1.*m;

    simple_cube = new SimpleCube( gm );
}



GeometryConstruction::~GeometryConstruction(){
    delete fDetectorMessenger;
}



G4VPhysicalVolume* GeometryConstruction::Construct(){

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

    world_lv->SetVisAttributes( G4VisAttributes::Invisible );

    fGeometryManager->Add( world_name, world_lv, world_pv );

    return world_pv;
}


void GeometryConstruction::ConstructUserVolumes(){

    simple_cube->Construct();
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


