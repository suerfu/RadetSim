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
}



GeometryConstruction::~GeometryConstruction(){
    delete fDetectorMessenger;
}



G4VPhysicalVolume* GeometryConstruction::Construct(){
    
    G4cout << GetClassName() << ": Constructing geometry...\n";

    // Obtain the type of geometry from configuration parser
    //
    G4String GeometryType = GeometryManager::Get()->GetConfigParser()->GetString( "type");
    G4cout << GetClassName() << ": type of geometry is " << GeometryType << G4endl;


    //===================================================================
    // Construct World
    // world is needed in all cases
    //===================================================================
    
    world_x = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/x", 100 ) * cm;
    world_y = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/y", 100 ) * cm;
    world_z = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/z", 100 ) * cm;


    G4String world_name = "World";
    G4Material* world_material  = fGeometryManager->GetMaterial("G4_AIR");

    G4Box* world_solid          = new G4Box( world_name+"_sld", world_x/2.0, world_y/2.0, world_z/2.0 );
    G4LogicalVolume* logicWorld   = new G4LogicalVolume( world_solid, world_material, world_name+"_lv" );
    G4VPhysicalVolume* physWorld = new G4PVPlacement( 0, G4ThreeVector(0,0,0), logicWorld, world_name, 0, false, 0, fCheckOverlaps );

    //===================================================================

    switch( GetGeometryCode( GeometryType ) ){

        case 0:
            ConstructRock();
            break;

        case 101:
            ConstructKamiokaNaI( 0 );
            break;
        
        case 102:
            ConstructKamiokaNaI( 1 );
            break;

        default:
            ConstructRock();
            break;
    }

    //logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
    auto visWorld = new G4VisAttributes();
    visWorld->SetForceWireframe(true);
    visWorld->SetVisibility(true);
    
    logicWorld->SetVisAttributes(visWorld);
    
    return physWorld;
}


int GeometryConstruction::GetGeometryCode( G4String input ){

    if( input == "Rock" ){
        return 0;
    }
    if( input == "KamiokaNaI-2304" ){
        return 101;
    }
    if( input == "KamiokaNaI-2504" ){
        return 102;
    }

    return 0;
}



void GeometryConstruction::ConstructRock(){
 
    G4LogicalVolume* logicWorld = GeometryManager::Get()->GetLogicalVolume("World");
    
    // Rock extending half of the world
    //
    G4String name = "Rock";

    G4Material* rockMaterial = GeometryManager::Get()->GetMaterial( "Rock" );

    G4Box* rock_solid        = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0 );
    G4LogicalVolume* rock_lv = new G4LogicalVolume( rock_solid, rockMaterial, name+"_lv" );

    new G4PVPlacement( 0, G4ThreeVector(0,0,-world_z/4), rock_lv, name, logicWorld, false, 0, fCheckOverlaps);

    // virtual detector on top of the rock
    //
    name = "virtualDetector";

    G4Material* virtualDetMaterial = GeometryManager::Get()->GetMaterial( "G4_Galactic" );

    G4Box* det_solid        = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0);
    G4LogicalVolume* det_lv = new G4LogicalVolume( det_solid, virtualDetMaterial, name+"_lv");

    new G4PVPlacement( 0, G4ThreeVector(0,0,world_z/4), det_lv, name, logicWorld, false, 0, fCheckOverlaps);
    
    rock_lv->SetVisAttributes(G4VisAttributes(G4Colour(0.3, 0.3, 0.3)));      // Dark Gray

}


void GeometryConstruction::ConstructKamiokaNaI( int option ){
    
    G4LogicalVolume* logicWorld = GeometryManager::Get()->GetLogicalVolume("World");
    
    if( option == 0){

        G4String name = "Rock";

        G4Material* rockMaterial = GeometryManager::Get()->GetMaterial( "Rock" );

        G4Box* rock_solid        = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0 );
        G4LogicalVolume* rock_lv = new G4LogicalVolume( rock_solid, rockMaterial, name );

        new G4PVPlacement( 0, G4ThreeVector(0,0,-world_z/4), rock_lv, name, logicWorld, false, 0, fCheckOverlaps);
        
        // Lead shielding
        //
        name = "Shielding";

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
        
        G4Material* shieldingMaterial = GeometryManager::Get()->GetMaterial( "G4_Pb" );

        G4VSolid* shielding_solid = new G4SubtractionSolid( name+"_solid", Pb_solid, air_solid, 0, G4ThreeVector(0,0,-Pb_z/2+Air_z/2));
        G4LogicalVolume* shielding_lv = new G4LogicalVolume( shielding_solid, shieldingMaterial, name+"_lv");
        new G4PVPlacement( 0, G4ThreeVector( 0, 0, Pb_z/2 ), shielding_lv, name, logicWorld, false, 0, fCheckOverlaps);

        // NaI detector
        //
        name = "NaI";

        G4double NaI_diameter = 1.5 * 2.54 * cm;
        G4double NaI_height = NaI_diameter;
        G4double space = 8 * mm;

        G4Material* NaIMaterial = GeometryManager::Get()->GetMaterial( "NaI" );

        G4Tubs* NaI_solid = new G4Tubs( name+"_solid", 0, NaI_diameter/2, NaI_height/2, 0, CLHEP::twopi);
        G4LogicalVolume* NaI_lv = new G4LogicalVolume( NaI_solid, NaIMaterial, name+"_lv" );
        new G4PVPlacement( 0, G4ThreeVector( 0, 0, NaI_height/2 + space ), NaI_lv, name+"Detector", logicWorld, false, 0, fCheckOverlaps );
    }

    else if ( option == 1 ){
        
        // Materials
        G4NistManager* nist = G4NistManager::Instance();
        auto vacuum = nist->FindOrBuildMaterial("G4_GALACTIC");
        auto air = nist->FindOrBuildMaterial("G4_AIR");
        auto NaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
        auto borosilicate = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
        auto aluminum = nist->FindOrBuildMaterial("G4_Al");

        // Dimensions
        G4double crystal_d = 76.0 * mm;
        G4double crystal_h = 76.0 * mm;

        G4double glass_th = 3.0 * mm;   // th for thickness

        G4double pmt_len = 134.0 * mm;
        G4double pmt_outer_d = crystal_d;
        G4double pmt_wall = 2.0 * mm;
        G4double pmt_inner_d = pmt_outer_d - 2 * pmt_wall;
        
        G4double casing_th = 1.0 * mm;

        G4double assembly_d = pmt_outer_d + 2 * casing_th;
        G4double assembly_h = crystal_h + glass_th + pmt_len + 2 * casing_th;

        // NaIDetectorAssembly logical volume (aluminum-filled cylinder)
        G4String name = "NaIDetector";
        auto solidAssembly = new G4Tubs(name,
                                        0,
                                        assembly_d / 2,
                                        assembly_h / 2,
                                        0, 360 * deg);
        auto logicAssembly = new G4LogicalVolume(solidAssembly, air, name);

        // Place the assembly at center of world
        new G4PVPlacement(0, {}, logicAssembly, name, logicWorld, false, 0);

        name = "Enclosure";
        auto solidEnclosureInner = new G4Tubs( name+"1",
                                        0,
                                        assembly_d / 2 - casing_th,
                                        assembly_h / 2 - casing_th,
                                        0, 360 * deg);
        auto solidEnclosure = new G4SubtractionSolid( name, solidAssembly, solidEnclosureInner, 0, G4ThreeVector(0,0,0));
        auto logicEnclosure = new G4LogicalVolume(solidAssembly, aluminum, name);

        new G4PVPlacement(0, {}, logicEnclosure, name, logicWorld, false, 0);


        // --- Inside the enclosure ---

        // NaI Crystal
        name = "NaICrystal";
        auto solidCrystal = new G4Tubs( name, 0, crystal_d / 2, crystal_h / 2, 0, 360 * deg);
        auto logicCrystal = new G4LogicalVolume( solidCrystal, NaI, name);
        G4ThreeVector posCrystal( 0, 0, -(pmt_len+glass_th)/2 );

        new G4PVPlacement( 0, posCrystal, logicCrystal, name, logicAssembly, false, 0);

        // Glass Window
        name = "GlassWindow";
        auto solidGlass = new G4Tubs(name, 0, crystal_d / 2, glass_th / 2, 0, 360 * deg);
        auto logicGlass = new G4LogicalVolume(solidGlass, borosilicate, name);
        G4ThreeVector posGlass = posCrystal + G4ThreeVector(0, 0, crystal_h / 2 + glass_th / 2 );
        
        new G4PVPlacement(0, posGlass, logicGlass, name, logicAssembly, false, 0);

        // PMT Tube
        name = "PMT1";
        auto solidPMT1 = new G4Tubs(name,
                                   0,
                                   pmt_outer_d / 2,
                                   pmt_len / 2,
                                   0, 360 * deg);
        name = "PMT2";
        auto solidPMT2 = new G4Tubs(name,
                                   0,
                                   pmt_inner_d / 2,
                                   pmt_len / 2 - pmt_wall,
                                   0, 360 * deg);
        name = "PMT";
        auto solidPMT = new G4SubtractionSolid( name, solidPMT1, solidPMT2, 0, G4ThreeVector(0,0,0));

        auto logicPMT = new G4LogicalVolume(solidPMT, borosilicate, name);
        G4ThreeVector posPMT = posGlass + G4ThreeVector(0, 0, glass_th / 2 + pmt_len / 2 );
        
        new G4PVPlacement(0, posPMT, logicPMT, name, logicAssembly, false, 0);

        // Visuals
        auto invis = new G4VisAttributes( G4Color(0,0,0,0.1) );
        //invis->SetVisibility(false);

        logicEnclosure->SetVisAttributes(G4VisAttributes(G4Colour(0.8, 0.8, 0.8, 0.2)));
            // Light Gray
        logicCrystal->SetVisAttributes(G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.8)));
            // Blue
        logicGlass->SetVisAttributes(G4VisAttributes(G4Colour(0.8, 0.8, 0, 0.8)));
            // Light Gray
        logicPMT->SetVisAttributes(G4VisAttributes(G4Colour(0.3, 0.3, 0.3, 0.4)));      // Dark Gray
        //logicAssembly->SetVisAttributes(G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.4)));     // Metallic

        logicAssembly->SetVisAttributes(invis);

    }
}


/*
G4VIStore* GeometryConstruction::CreateImportanceStore(){
    
    //===============  Importance Sampling to speed up simulation ==============//
    
    G4IStore *istore = G4IStore::GetInstance();

    istore->AddImportanceGeometryCell( 1, *physWorld);
    for( unsigned int i=0; i<list.size(); i++ ){
        istore->AddImportanceGeometryCell( list[i].bias, *list[i].phy);
    }
    
    return istore;
}
*/
