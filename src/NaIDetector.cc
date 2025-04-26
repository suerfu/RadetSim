/*
    Author:  Suerfu Burkhant
    Date:    April 12, 2025
    Contact: suerfu@post.kek.jp
*/

#include "NaIDetector.hh"

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

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>



NaIDetector::NaIDetector(){
    fCheckOverlaps = true;

    // Common dimensions for NaI crystal - PMT assembly
    //
    crystal_d = 76.0 * mm;
    crystal_h = 76.0 * mm;

    glass_th = 3.0 * mm;   // th for thickness

    pmt_len = 134.0 * mm;
    pmt_outer_d = crystal_d;
    pmt_wall = 2.0 * mm;
    pmt_inner_d = pmt_outer_d - 2 * pmt_wall;
    
    casing_th = 1.0 * mm;

    assembly_d = pmt_outer_d + 2 * casing_th;
    assembly_h = crystal_h + glass_th + pmt_len + 2 * casing_th;
}



NaIDetector::~NaIDetector(){}


G4double NaIDetector::GetDiameter(){ return assembly_d;}


G4double NaIDetector::GetLength(){ return assembly_h;}


G4LogicalVolume* NaIDetector::GetLogicalVolume(){

    G4NistManager* nist = G4NistManager::Instance();

    auto vacuum = nist->FindOrBuildMaterial("G4_GALACTIC");
    auto air = nist->FindOrBuildMaterial("G4_AIR");
    auto NaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    auto borosilicate = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
    auto aluminum = nist->FindOrBuildMaterial("G4_Al");

    // ========================================
    // Create NaI crystal - PMT assembly object
    // ========================================
    
    // NaIDetectorAssembly logical volume (aluminum-filled cylinder)
    //
    G4String name = "NaIDetector";
    auto solidAssembly = new G4Tubs(name,
                                    0,
                                    assembly_d / 2,
                                    assembly_h / 2,
                                    0, 360 * deg);
    auto logicAssembly = new G4LogicalVolume(solidAssembly, air, name);

    name = "Enclosure";
    auto solidEnclosureInner = new G4Tubs( name+"1",
                                    0,
                                    assembly_d / 2 - casing_th,
                                    assembly_h / 2 - casing_th,
                                    0, 360 * deg);
    auto solidEnclosure = new G4SubtractionSolid( name, solidAssembly, solidEnclosureInner, 0, G4ThreeVector(0,0,0));
    auto logicEnclosure = new G4LogicalVolume(solidAssembly, aluminum, name);

    // --- Inside the enclosure ---
    // NaI Crystal
    //
    name = "NaICrystal";
    auto solidCrystal = new G4Tubs( name, 0, crystal_d / 2, crystal_h / 2, 0, 360 * deg);
    auto logicCrystal = new G4LogicalVolume( solidCrystal, NaI, name);
    G4ThreeVector posCrystal( 0, 0, -(pmt_len+glass_th)/2 );

    new G4PVPlacement( 0, posCrystal, logicCrystal, name, logicAssembly, false, 0);

    // Glass Window
    //
    name = "GlassWindow";
    auto solidGlass = new G4Tubs(name, 0, crystal_d / 2, glass_th / 2, 0, 360 * deg);
    auto logicGlass = new G4LogicalVolume(solidGlass, borosilicate, name);
    G4ThreeVector posGlass = posCrystal + G4ThreeVector(0, 0, crystal_h / 2 + glass_th / 2 );
    
    new G4PVPlacement(0, posGlass, logicGlass, name, logicAssembly, false, 0);

    // PMT Tube
    //
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
    //
    auto invis = new G4VisAttributes( G4Color(0,0,0,0.1) );
    //invis->SetVisibility(false);

    logicEnclosure->SetVisAttributes(G4VisAttributes(G4Colour(0.8, 0.8, 0.8, 0.2)));
        // Light Gray
    logicCrystal->SetVisAttributes(G4VisAttributes(G4Colour(.25, .88, .82, 0.95)));
        // Blue
    logicGlass->SetVisAttributes(G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.8)));
        // Light Gray
    logicPMT->SetVisAttributes(G4VisAttributes(G4Colour(0.3, 0.3, 0.3, 0.4)));      // Dark Gray
    //logicAssembly->SetVisAttributes(G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.4)));     // Metallic

    logicAssembly->SetVisAttributes(invis);

    return logicAssembly;
}
