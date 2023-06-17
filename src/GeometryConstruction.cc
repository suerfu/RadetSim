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
}



GeometryConstruction::~GeometryConstruction(){
    delete fDetectorMessenger;
}



G4VPhysicalVolume* GeometryConstruction::Construct(){

    G4cout << GetClassName() << ": Constructing geometry...\n";

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

    geometryType = GeometryManager::Get()->GetGeometryType();
    G4cout << GetClassName() << ": The type of geometry is " << geometryType << G4endl;
    
    // if type is within 100 and 200, then it is a rock-related geometry
    //
    if( geometryType >= 100 && geometryType <=199 ){

        G4Material* rockMaterial = GeometryManager::Get()->GetMaterial( "Rock" );

        G4LogicalVolume* world_lv = GeometryManager::Get()->GetLogicalVolume("world");
    
        // Rock extending half of the world
        //
        G4Box* rock_solid = new G4Box( "rock_solid", world_x/2.0, world_y/2.0, world_z/4.0);
        G4LogicalVolume* rock_lv = new G4LogicalVolume( rock_solid, rockMaterial, "rock_lv");
        new G4PVPlacement( 0, G4ThreeVector(0,0,-world_z/4), rock_lv, "Rock", world_lv, false, 0, fCheckOverlaps);

        // 100 ==> rock + virtual detector
        //
        if( geometryType==100 ){

            G4Material* virtualDetMaterial = GeometryManager::Get()->GetMaterial( "G4_Galactic" );

            G4Box* det_solid = new G4Box( "virtualDetector_solid", world_x/2.0, world_y/2.0, world_z/4.0);

            G4LogicalVolume* det_lv = new G4LogicalVolume( det_solid, virtualDetMaterial, "virtualDetector_lv");

            new G4PVPlacement( 0, G4ThreeVector(0,0,world_z/4), det_lv, "virtualDetector", world_lv, false, 0, fCheckOverlaps);
        }

        // 101 ==> rock + NaI inside Pb shielding
        //
        if( geometryType==101 ){
            // Lead shielding
            //
            G4double Pb_x = 30 * cm;
            G4double Pb_y = 30 * cm;
            G4double Pb_z = 50 * cm;
            G4Box* Pb_solid = new G4Box( "Pb_solid", Pb_x/2.0, Pb_y/2.0, Pb_z/2.0);

            // Shielding inner space
            //
            G4double Air_x = 10 * cm;
            G4double Air_y = 10 * cm;
            G4double Air_z = 30 * cm;
            G4Box* air_solid = new G4Box( "air_solid", Air_x/2.0, Air_y/2.0, Air_z/2.0);

            G4VSolid* shielding_solid = new G4SubtractionSolid( "shielding_solid", Pb_solid, air_solid, 0, G4ThreeVector(0,0,-Pb_z/2+Air_z/2));

            G4Material* shieldingMaterial = GeometryManager::Get()->GetMaterial( "G4_Pb" );
            G4LogicalVolume* shielding_lv = new G4LogicalVolume( shielding_solid, shieldingMaterial, "shielding_lv");
            new G4PVPlacement( 0, G4ThreeVector(0,0,Pb_z/2+3*cm), shielding_lv, "Shielding", world_lv, false, 0, fCheckOverlaps);

            // NaI detector
            //
            G4double diameter = 1.5 * 2.54 * cm;
            G4double height = diameter;
            G4Tubs* NaI_solid = new G4Tubs( "NaI_solid", 0, diameter/2, height/2, 0, CLHEP::twopi);
            G4Material* NaIMaterial = GeometryManager::Get()->GetMaterial( "NaI" );
            G4LogicalVolume* NaI_lv = new G4LogicalVolume( NaI_solid, NaIMaterial, "NaI_lv" );
            new G4PVPlacement( 0, G4ThreeVector(0,0,height/2+3*cm+5*cm), NaI_lv, "NaIDetector", world_lv, false, 0, fCheckOverlaps );
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
