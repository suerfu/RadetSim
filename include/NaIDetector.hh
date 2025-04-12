/*
    Author:  Suerfu Burkhant
    Date:    April 12, 2025
    Contact: suerfu@post.kek.jp
*/

#ifndef NAIDETECTOR_H
#define NAIDETECTOR_H 1

#include "G4VUserDetectorConstruction.hh"
#include "GeometryManager.hh"

#include "globals.hh"

#include <string>

using std::string;

class GeometryConstructionMessenger;


/// Detector construction class to define materials and geometry.
//
class NaIDetector{

public:

    NaIDetector();

    virtual ~NaIDetector();

    G4LogicalVolume* GetLogicalVolume();

    G4double GetDiameter();

    G4double GetLength();

private:

    string GetClassName(){ return "NaIDetector"; }

    GeometryManager* fGeometryManager;

    bool fCheckOverlaps;

    // dimensions for NaI crystal - PMT assembly
    //
    G4double crystal_d;
    G4double crystal_h;

    G4double glass_th;
        // th for thickness

    G4double pmt_len;
    G4double pmt_outer_d;
    G4double pmt_wall;
    G4double pmt_inner_d;
    
    G4double casing_th;

    G4double assembly_d;
    G4double assembly_h;
};


#endif
