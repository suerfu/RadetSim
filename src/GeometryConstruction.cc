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

    detector_assembly = NaIDetector();
}



GeometryConstruction::~GeometryConstruction(){
    delete fDetectorMessenger;
}



G4VPhysicalVolume* GeometryConstruction::Construct(){
    
    G4cout << GetClassName() << ": Constructing geometry...\n";

    // Obtain the type of geometry from configuration parser
    //
    G4String GeometryType = GeometryManager::Get()->GetConfigParser()->GetString( "type", "KamiokaGamma-2025" );
    G4cout << GetClassName() << ": type of geometry is " << GeometryType << G4endl;


    //===================================================================
    // Get parameters for World
    // Construction is left for each classes
    //===================================================================
    
    
    world_x = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/x", 100 ) * cm;
    world_y = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/y", 100 ) * cm;
    world_z = GeometryManager::Get()->GetConfigParser()->GetDouble( "world/z", 100 ) * cm;
    
    /*
    G4String world_name = "World";
    G4Material* world_material  = fGeometryManager->GetMaterial("G4_AIR");

    G4Box* world_solid          = new G4Box( world_name+"_sld", world_x/2.0, world_y/2.0, world_z/2.0 );
    G4LogicalVolume* logicWorld   = new G4LogicalVolume( world_solid, world_material, world_name+"_lv" );
    G4VPhysicalVolume* physWorld = new G4PVPlacement( 0, G4ThreeVector(0,0,0), logicWorld, world_name, 0, false, 0, fCheckOverlaps );
    */

    //===================================================================
    
    G4VPhysicalVolume* physWorld = 0;

    switch( GetGeometryCode( GeometryType ) ){

        case 0:
            physWorld = ConstructRock();
            break;

        case 100:
            physWorld = ConstructKamiokaGamma2023();
            break;
        
        case 101:
            {
                int direction = GeometryManager::Get()->GetConfigParser()->GetInt( "direction", 0 );
                physWorld = ConstructKamiokaGamma2025( direction % 7 );
                    // direction code:
                    // 0 - internal
                    // 1 - bottom
                    // 2 - top
                    // 3 - east
                    // 4 - south
                    // 5 - west
                    // 6 - north
            }
            break;

        default:
            ConstructRock();
            break;
    }

    // set the visual aspects of logic world
    // !!! This assumes that user has created World volume named World in the ConstrctXXX code !!!
    //
    /*
    G4LogicalVolume* logicWorld = GeometryManager::Get()->GetLogicalVolume("World");

    auto visWorld = new G4VisAttributes();
    visWorld->SetForceWireframe(true);
    visWorld->SetVisibility(true);
    
    logicWorld->SetVisAttributes(visWorld);
    */
    
    return physWorld;
}


int GeometryConstruction::GetGeometryCode( G4String input ){

    if( input == "Rock" ){
        return 0;
    }
    if( input == "KamiokaGamma-2023" ){
        return 100;
    }
    if( input == "KamiokaGamma-2025" ){
        return 101;
    }
    return 0;
}



G4VPhysicalVolume* GeometryConstruction::ConstructRock(){
 
    G4String name = "World";

    G4Material* world_material  = fGeometryManager->GetMaterial("G4_AIR");

    auto solidWorld = new G4Box( name, world_x/2.0, world_y/2.0, world_z/2.0 );
    auto logicWorld = new G4LogicalVolume( solidWorld, world_material, name );
    auto physWorld  = new G4PVPlacement( 0, G4ThreeVector(0,0,0), logicWorld, name, 0, false, 0, fCheckOverlaps );

    // G4LogicalVolume* logicWorld = GeometryManager::Get()->GetLogicalVolume("World");
    
    // Rock extending half of the world
    //
    name = "Rock";

    G4Material* rock = GeometryManager::Get()->GetMaterial( "Rock" );

    G4Box* rock_solid        = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0 );
    G4LogicalVolume* rock_lv = new G4LogicalVolume( rock_solid, rock, name+"_lv" );

    new G4PVPlacement( 0, G4ThreeVector(0,0,-world_z/4), rock_lv, name, logicWorld, false, 0, fCheckOverlaps);

    // virtual detector on top of the rock
    //
    name = "virtualDetector";

    G4Material* virtualDetMaterial = GeometryManager::Get()->GetMaterial( "G4_Galactic" );

    G4Box* det_solid        = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0);
    G4LogicalVolume* det_lv = new G4LogicalVolume( det_solid, virtualDetMaterial, name+"_lv");

    new G4PVPlacement( 0, G4ThreeVector(0,0,world_z/4), det_lv, name, logicWorld, false, 0, fCheckOverlaps);
    
    rock_lv->SetVisAttributes(G4VisAttributes(G4Colour(0.3, 0.3, 0.3)));      // Dark Gray

    return physWorld;
}


G4VPhysicalVolume* GeometryConstruction::ConstructKamiokaGamma2023( ){
    
    // World
    //
    G4String name = "World";
    
    G4Material* world_material  = fGeometryManager->GetMaterial("G4_AIR");

    auto solidWorld = new G4Box( name, world_x/2.0, world_y/2.0, world_z/2.0 );
    auto logicWorld = new G4LogicalVolume( solidWorld, world_material, name );
    auto physWorld  = new G4PVPlacement( 0, G4ThreeVector(0,0,0), logicWorld, name, 0, false, 0, fCheckOverlaps );

    // Rock
    //
    name = "Rock";

    G4Material* rock = GeometryManager::Get()->GetMaterial( "Rock" );

    G4Box* rock_solid        = new G4Box( name+"_solid", world_x/2.0, world_y/2.0, world_z/4.0 );
    G4LogicalVolume* rock_lv = new G4LogicalVolume( rock_solid, rock, name );

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

    return physWorld;
}



G4VPhysicalVolume* GeometryConstruction::ConstructKamiokaGamma2025( int option ){
    
    G4cout << "Constructing geometries for Kamioka miniland gamma measurement in March, 2025..." << G4endl;

    // =======================================
    // Get materials used for this measurement
    // =======================================
    //
    G4NistManager* nist = G4NistManager::Instance();

    auto vacuum = nist->FindOrBuildMaterial("G4_GALACTIC");
    auto air = nist->FindOrBuildMaterial("G4_AIR");
    auto rock = GeometryManager::Get()->GetMaterial( "Rock" );
    auto aluminum = nist->FindOrBuildMaterial("G4_Al");
    auto steel = G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    auto lead = GeometryManager::Get()->GetMaterial( "G4_Pb" );

    // ====================================================
    // Information on the room and the rock to be simulated
    // ====================================================

    // Experimental room
    //
    G4double roomX = 8 * m;
    G4double roomY = 4 * m;
    G4double roomZ = 4 * m;

    // Effective rock
    // on top of the room, add this much of rock in simulation
    //
    G4double rockZ = 30 * cm;

    // Steel plate on which detector and the rails are set up
    //
    G4double steelBaseX = 1828 * mm;
        // x is defined as east-west direction
    G4double steelBaseY = 3 * 914 * mm;
        // y is defined as north-south direction
    G4double steelBaseZ = 16 * mm;
        // z is defined as upward direction

    // Lead shielding dimensitions
    //
    G4double shieldingX = 50 * cm;
    G4double shieldingY = 50 * cm;
    G4double shieldingZ = 80 * cm;

    G4double innerShieldingX = 10 * cm;
    G4double innerShieldingY = 10 * cm;
    G4double innerShieldingZ = 40 * cm;

    G4double windowX = 10 * cm;
    G4double windowY = 20 * cm; // recess / distance from lead surface to NaI detector
    G4double windowZ = 20 * cm; // height of the opening window for radiation measurement

    // ====================================
    // Geometries common to all simulations
    // ====================================

    G4String name = "World";

    G4double worldX = roomX+2*rockZ;
    G4double worldY = roomY+2*rockZ;
    G4double worldZ = roomZ+2*rockZ;

    auto solidWorld = new G4Box( name, worldX/2, worldY/2, worldZ/2 );
    auto logicWorld = new G4LogicalVolume( solidWorld, rock, name );
    auto physWorld  = new G4PVPlacement( 0, G4ThreeVector(0,0,0), logicWorld, name, 0, false, 0, fCheckOverlaps );

    name = "Room";

    auto solidRoom = new G4Box( name, roomX/2.0, roomY/2.0, roomZ/2.0 );
    auto logicRoom = new G4LogicalVolume( solidRoom, air, name );
    auto physRoom  = new G4PVPlacement( 0, G4ThreeVector(0,0,0), logicRoom, name, logicWorld, false, 0, fCheckOverlaps );

    name = "SteelBase";

    G4double steelPosX = -roomX/2 + 2 * m;
    G4double steelPosY = -roomY/2 + 2 * m;
    G4double steelPosZ = -roomZ/2 + steelBaseZ/2;

    auto solidSteelBase = new G4Box( name, steelBaseX/2.0, steelBaseY/2.0, steelBaseZ/2.0 );
    auto logicSteelBase = new G4LogicalVolume( solidSteelBase, steel, name );
    new G4PVPlacement( 0, G4ThreeVector(steelPosX,steelPosY,steelPosZ), logicSteelBase, name, logicRoom, false, 0, fCheckOverlaps);

    name = "PbShield";

    G4double shieldPosZ = steelPosZ + steelBaseZ/2 + shieldingZ/2;

    auto solidShielding = new G4Box( name, shieldingX/2.0, shieldingY/2.0, shieldingZ/2.0 );
    auto logicShielding  = new G4LogicalVolume( solidShielding, lead, name);
    new G4PVPlacement( 0, G4ThreeVector(steelPosX,steelPosY,shieldPosZ), logicShielding, name, logicRoom, false, 0, fCheckOverlaps);
    
    auto logicNaIDetector = detector_assembly.GetLogicalVolume();

    
    // Internal background measurements
    // Configuration is lead shielding + inner space + NaI detector assembly
    // World is as large as the 4-pi lead shielding.
    //
    if ( option == 0 ){

        name = "ShieldingInside";
        auto solidShieldingInside = new G4Box( name, innerShieldingX/2.0, innerShieldingY/2.0, innerShieldingZ/2.0 );

        G4double zOffset = 20 * cm; // thickness of lead shielding in the bottom direction
        G4double shieldingInsideZ = -shieldingZ/2 + innerShieldingZ/2 + zOffset;
    
        auto logicShieldingInside  = new G4LogicalVolume( solidShieldingInside, air, name);
        new G4PVPlacement( 0, G4ThreeVector(0,0,shieldingInsideZ), logicShieldingInside, name, logicShielding, false, 0, fCheckOverlaps);

        // place the detector assembly
        //
        G4double zGap = 10 * cm;
            // distance from NaI detector surface to inner lead surface in the upward direction
        G4double posZ = innerShieldingZ/2 - detector_assembly.GetLength()/2 - zGap;

        auto rotation = new G4RotationMatrix();
        rotation->rotateX(180.0 * deg);
            // in the internal background measurement, the detector is oriented upward, so apply rotation

        new G4PVPlacement( rotation, G4ThreeVector(0,0,posZ), logicNaIDetector, name, logicShieldingInside, false, 0);
    }

    // Bottom measurements
    // Bottom configuration is Rock and Steel plate extending half of world
    // NaI detector sits on top of the Steel plate
    //
    else if( option == 1 ){

        name = "ShieldingInside";
        auto solidShieldingInside = new G4Box( name, innerShieldingX/2.0, innerShieldingY/2.0, innerShieldingZ/2.0 );

        G4double shieldingInsideZ = -shieldingZ/2 + innerShieldingZ/2;
    
        auto logicShieldingInside  = new G4LogicalVolume( solidShieldingInside, air, name);
        new G4PVPlacement( 0, G4ThreeVector(0,0,shieldingInsideZ), logicShieldingInside, name, logicShielding, false, 0, fCheckOverlaps);

        // NaI detector
        //
        G4double detPosZ = -innerShieldingZ/2 + detector_assembly.GetLength()/2;
        new G4PVPlacement( 0, G4ThreeVector(0,0,detPosZ), logicNaIDetector, name, logicShieldingInside, false, 0);

        // Rock on the floor
        //
        name = "Rock";

        G4double rockX = 1 * m;
        G4double rockY = 1 * m;
        G4double rockPosZ = -worldZ/2 + rockZ/2;

        auto solidRock = new G4Box( name, rockX/2.0, rockY/2.0, rockZ/2.0 );
        auto logicRock = new G4LogicalVolume( solidRock, rock, name );
        new G4PVPlacement( 0, G4ThreeVector(steelPosX,steelPosY,rockPosZ), logicRock, name, logicWorld, false, 0, fCheckOverlaps);
        
    }



    logicShielding->SetVisAttributes(G4VisAttributes(G4Colour(0.2, 0.2, 0.2, 0.5)));      // Dark Gray

    return physWorld;
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

