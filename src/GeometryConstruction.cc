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
    world_x = 200.;
    world_y = 200.;
    world_z = 200.;
}



GeometryConstruction::~GeometryConstruction(){
    delete fDetectorMessenger;
}



G4VPhysicalVolume* GeometryConstruction::Construct(){
    
    G4cout << GetClassName() << ": Constructing geometry...\n";

    world_x = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/x", world_x ) * cm;
    world_y = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/y", world_y ) * cm;
    world_z = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/z", world_z ) * cm;

    G4VPhysicalVolume* world_pv = ConstructWorld();

    /* Other user-defined geometries goes here */
    ConstructUserVolumes();

    return world_pv;
}



G4VPhysicalVolume* GeometryConstruction::ConstructWorld(){

    G4String world_name = "World";
    G4Material* world_material = fGeometryManager->GetMaterial("G4_AIR");

    G4Box* world_solid = new G4Box( world_name+"_sld", world_x/2.0, world_y/2.0, world_z/2.0 );
    G4LogicalVolume* world_lv = new G4LogicalVolume( world_solid, world_material, world_name+"_lv" );
    G4VPhysicalVolume* world_pv = new G4PVPlacement( 0, G4ThreeVector(0,0,0), world_lv, world_name, 0, false, 0, fCheckOverlaps );

    //world_lv->SetVisAttributes( G4VisAttributes::GetInvisible() );

    return world_pv;
}


void GeometryConstruction::ConstructUserVolumes(){

    geometryType = GeometryManager::Get()->GetGeometryType();
    G4cout << GetClassName() << ": The type of geometry is " << geometryType << G4endl;
    
    G4LogicalVolume* world_lv = GeometryManager::Get()->GetLogicalVolume("World");
    
    // if type is within 100 and 200, then it is a rock-related geometry
    //
    if( geometryType >= 100 && geometryType <=199 ){

        // Rock extending half of the world
        //
        G4String name = "Rock";

        G4Material* rockMaterial = GeometryManager::Get()->GetMaterial( "Rock" );

        G4Box* rock_solid = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0 );
        G4LogicalVolume* rock_lv = new G4LogicalVolume( rock_solid, rockMaterial, name+"_lv" );
        new G4PVPlacement( 0, G4ThreeVector(0,0,-world_z/4), rock_lv, name, world_lv, false, 0, fCheckOverlaps);

        // 100 ==> rock + virtual detector
        //
        if( geometryType==100 ){

            name = "virtualDetector";

            G4Material* virtualDetMaterial = GeometryManager::Get()->GetMaterial( "G4_Galactic" );

            G4Box* det_solid = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0);
            G4LogicalVolume* det_lv = new G4LogicalVolume( det_solid, virtualDetMaterial, name+"_lv");
            new G4PVPlacement( 0, G4ThreeVector(0,0,world_z/4), det_lv, name, world_lv, false, 0, fCheckOverlaps);
        }

        // 101 ==> rock + NaI inside Pb shielding
        //
        if( geometryType==101 ){

            // Lead shielding
            //
            name = "Shielding";

            G4double Pb_x = 30;
            G4double Pb_y = 30;
            G4double Pb_z = 50;
            
            Pb_x = GeometryManager::Get()->GetConfigParser()->GetDouble( "shield/x", Pb_x ) * cm;
            Pb_y = GeometryManager::Get()->GetConfigParser()->GetDouble( "shield/y", Pb_y ) * cm;
            Pb_z = GeometryManager::Get()->GetConfigParser()->GetDouble( "shield/z", Pb_z ) * cm;
            
            G4Box* Pb_solid = new G4Box( "Pb_solid", Pb_x/2.0, Pb_y/2.0, Pb_z/2.0);

            // Shielding inner space
            //
            G4double Air_x = 10;
            G4double Air_y = 10;
            G4double Air_z = 30;
            
            Air_x = GeometryManager::Get()->GetConfigParser()->GetDouble( "shield/inner_x", Air_x ) * cm;
            Air_y = GeometryManager::Get()->GetConfigParser()->GetDouble( "shield/inner_y", Air_y ) * cm;
            Air_z = GeometryManager::Get()->GetConfigParser()->GetDouble( "shield/inner_z", Air_z ) * cm;
            
            G4Box* air_solid = new G4Box( "air_solid", Air_x/2.0, Air_y/2.0, Air_z/2.0);
            
            G4Material* shieldingMaterial = GeometryManager::Get()->GetMaterial( "G4_Pb" );

            G4VSolid* shielding_solid = new G4SubtractionSolid( name+"_solid", Pb_solid, air_solid, 0, G4ThreeVector(0,0,-Pb_z/2+Air_z/2));
            G4LogicalVolume* shielding_lv = new G4LogicalVolume( shielding_solid, shieldingMaterial, name+"_lv");
            new G4PVPlacement( 0, G4ThreeVector( 0, 0, Pb_z/2+3*cm ), shielding_lv, name, world_lv, false, 0, fCheckOverlaps);

            // NaI detector
            //
            name = "NaI";

            G4double NaI_diameter = 1.5 * 2.54 * cm;
            G4double NaI_height = NaI_diameter;
            G4double space = 0;

            NaI_diameter = GeometryManager::Get()->GetConfigParser()->GetDouble( "NaI/diameter", NaI_diameter ) * cm;
            NaI_height = GeometryManager::Get()->GetConfigParser()->GetDouble( "NaI/height", NaI_height ) * cm;
            space = GeometryManager::Get()->GetConfigParser()->GetDouble( "NaI/space", space ) * cm;

            G4Material* NaIMaterial = GeometryManager::Get()->GetMaterial( "NaI" );

            G4Tubs* NaI_solid = new G4Tubs( name+"_solid", 0, NaI_diameter/2, NaI_height/2, 0, CLHEP::twopi);
            G4LogicalVolume* NaI_lv = new G4LogicalVolume( NaI_solid, NaIMaterial, name+"_lv" );
            new G4PVPlacement( 0, G4ThreeVector( 0, 0, NaI_height/2+space ), NaI_lv, name+"Detector", world_lv, false, 0, fCheckOverlaps );
        }
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
