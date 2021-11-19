/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file SimpleCubeMessenger.hh
/// \brief Definition of the SimpleCubeMessenger class

#ifndef SimpleCubeMessenger_h
#define SimpleCubeMessenger_h 1

#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

class SimpleCube;


class SimpleCubeMessenger: public G4UImessenger{

public:

    SimpleCubeMessenger( SimpleCube* );

    virtual ~SimpleCubeMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

private:

    SimpleCube* detector;

    G4UIdirectory* directory;
    
    G4UIcmdWithAString* materialCmd;
        // Material of the target.

    G4UIcmdWith3VectorAndUnit* posCmd;
        // Position of the target

    G4UIcmdWithADoubleAndUnit* angCmd_x;
    G4UIcmdWithADoubleAndUnit* angCmd_y;
    G4UIcmdWithADoubleAndUnit* angCmd_z;
        // Orientation of the target

    //G4UIcommand* place_detector;
        // Place the actual object

    G4String name = "target";
    G4String dirname = G4String("/")+name+"/";
};

#endif
