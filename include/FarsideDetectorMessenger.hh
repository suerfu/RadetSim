/// \file FarsideDetectorMessenger.hh
/// \brief Definition of the FarsideDetectorMessenger class

#ifndef FARSIDEDETECTORMESSENGER_H
#define FARSIDEDETECTORMESSENGER_H 1

#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

#include "GeometryManager.hh"

#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

class FarsideDetectorMessenger: public G4UImessenger{

public:

    FarsideDetectorMessenger( GeometryManager* gm );

    ~FarsideDetectorMessenger();

    void SetNewValue(G4UIcommand*, G4String);

private:

    GeometryManager* fGeometryManager;

    G4UIdirectory* directory;
    
    G4UIcmdWith3VectorAndUnit* posCmd;
        // Position of the farside detector

    G4UIcmdWithADoubleAndUnit* angCmd_x;
    G4UIcmdWithADoubleAndUnit* angCmd_y;
    G4UIcmdWithADoubleAndUnit* angCmd_z;
        // Orientation of the farside detector

    G4ThreeVector pos;
    G4RotationMatrix* rot;

    G4UIcmdWithAString* placeCmd;
        // Place the actual object
    G4String name;
    G4String dirname;

};

#endif
