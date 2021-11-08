/// \file GeometryConstructionMessenger.hh
/// \brief Definition of the GeometryConstructionMessenger class

#ifndef GeometryConstructionMessenger_h
#define GeometryConstructionMessenger_h 1

#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

class GeometryConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;

class GeometryConstructionMessenger: public G4UImessenger{

public:
    GeometryConstructionMessenger( GeometryConstruction* );
    virtual ~GeometryConstructionMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

private:
    GeometryConstruction* detector;

    G4UIdirectory* directory;
    G4UIcmdWith3VectorAndUnit* posCmd;
        // Command to specify position of the farside detector
    G4UIcmdWithADoubleAndUnit* angCmd_x;
    G4UIcmdWithADoubleAndUnit* angCmd_y;
    G4UIcmdWithADoubleAndUnit* angCmd_z;
        // Command to specify angle of rotation the farside detector
    G4UIcommand* place_detector;
};

#endif
