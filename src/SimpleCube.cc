/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
#include "SimpleCube.hh"
#include "SimpleCubeMessenger.hh"

#include "G4Box.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

SimpleCube::SimpleCube( GeometryManager* g) : gman(g){

    thisName = "target";

    L_in = 5 * cm;
    L_out = 10 * cm;

    fCheckOverlaps = false;

    fSimpleCubeMessenger = new SimpleCubeMessenger( this );

}


SimpleCube::~SimpleCube(){
    delete fSimpleCubeMessenger;
}


void SimpleCube::Construct(){
    
    G4LogicalVolume* world_lv = gman->GetLogicalVolume("world");
    if( world_lv==0 ){
        G4cerr << "Cannot find the logical volume of world." << G4endl;
        return;
    }

    G4Box* box1_solid = new G4Box( thisName+"1_solid", L_out/2, L_out/2, L_out/2);
    G4Box* box2_solid = new G4Box( thisName+"2_solid", L_in/2, L_in/2, L_in/2);
    G4VSolid* box_solid = new G4SubtractionSolid("cube_solid", box1_solid, box2_solid);

    G4Material* tungsten = gman->GetMaterial("G4_W");
    G4LogicalVolume* box_lv = new G4LogicalVolume( box_solid, tungsten, thisName+"_lv");

    new G4PVPlacement( 0, G4ThreeVector(0,0,0), box_lv, thisName, world_lv, false, 0, fCheckOverlaps);

}


void SimpleCube::SetMaterial( G4String material_name ){
    G4Material* material = gman->GetMaterial( material_name );
    if( material!=0 ){
        G4LogicalVolume* cube_lv = gman->GetLogicalVolume( thisName );
        if( cube_lv ){
            cube_lv->SetMaterial( material );
            gman->GeometryHasBeenModified();
        }
    }
}


void SimpleCube::SetPosition( G4ThreeVector s){
    
    G4VPhysicalVolume* cube_pv = gman->GetPhysicalVolume( thisName );
    if( cube_pv ){
        cube_pv->SetTranslation(s);
        gman->GeometryHasBeenModified();
    }
    
}


void SimpleCube::RotateX( G4double a){
    
    G4VPhysicalVolume* cube_pv = gman->GetPhysicalVolume( thisName );

    if( cube_pv ){
        G4RotationMatrix* rot = cube_pv->GetRotation();
        if( !rot ){
            rot = new G4RotationMatrix;
        }

        rot->rotateX(a);
        cube_pv->SetRotation( rot );

        gman->GeometryHasBeenModified();
    }    
}


void SimpleCube::RotateY( G4double a){
    
    G4VPhysicalVolume* cube_pv = gman->GetPhysicalVolume( thisName );

    if( cube_pv ){
        G4RotationMatrix* rot = cube_pv->GetRotation();
        if( !rot ){
            rot = new G4RotationMatrix;
        }

        rot->rotateY(a);
        cube_pv->SetRotation( rot );

        gman->GeometryHasBeenModified();
    }
}


void SimpleCube::RotateZ( G4double a){

    
    G4VPhysicalVolume* cube_pv = gman->GetPhysicalVolume( thisName );

    if( cube_pv ){
        G4RotationMatrix* rot = cube_pv->GetRotation();
        if( !rot ){
            rot = new G4RotationMatrix;
        }

        rot->rotateZ(a);
        cube_pv->SetRotation( rot );

        gman->GeometryHasBeenModified();
    }
}
