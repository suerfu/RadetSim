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

//#include "SimpleCube.hh"

class SimpleCube;

class G4UIdirectory;
class G4UIcmdWithAString;

class SimpleCubeMessenger: public G4UImessenger{

public:

    SimpleCubeMessenger( SimpleCube* );

    virtual ~SimpleCubeMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

private:

    SimpleCube* detector;

    G4UIdirectory* directory;
    
    G4UIcmdWithADoubleAndUnit* LinCmd;
    G4UIcmdWithADoubleAndUnit* LoutCmd;
        // Size of the cube.

    G4UIcmdWith3VectorAndUnit* posCmd;
        // Position of the cube

    G4UIcmdWithADoubleAndUnit* angCmd_x;
    G4UIcmdWithADoubleAndUnit* angCmd_y;
    G4UIcmdWithADoubleAndUnit* angCmd_z;
        // Orientation

    G4UIcommand* place_detector;
        // Place the actual object
};

#endif
