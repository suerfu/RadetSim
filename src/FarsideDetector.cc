/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
#include "FarsideDetector.hh"

#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

FarsideDetector::FarsideDetector( GeometryManager* gm ) : fGeometryManager( gm ){
    radius = 5 * cm;
    height = 10 * cm;
}

void FarsideDetector::PlaceDetector( G4String name, G4ThreeVector pos, G4RotationMatrix* rot){

    G4LogicalVolume* mother = fGeometryManager->GetLogicalVolume("world");
    G4Material* mat = fGeometryManager->GetMaterial( "G4_Pb" );

    if( mother!=0 && mat!=0 ){
        G4cout << "Building farside detector " << name << G4endl;
        G4Tubs* solid = new G4Tubs( name+"_solid", 0, radius, height/2, 0, CLHEP::twopi);
        G4LogicalVolume* lv = new G4LogicalVolume( solid, mat, name+"_lv" );
        new G4PVPlacement( rot, pos, lv, name, mother, false, 0, false );
    }
    else{
        if( mother==0 ){
            G4cerr << "Mother volume world is not defined" << G4endl;
        }
        else{
            G4cerr << "Cannot find the material of farside detector" << G4endl;
        }
    }

}
