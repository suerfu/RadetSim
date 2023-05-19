/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file GeometryConstructionMessenger.hh
/// \brief Definition of the GeometryConstructionMessenger class

#ifndef GEOMETRYCONSTRUCTIONMESSENGER_H
#define GEOMETRYCONSTRUCTIONMESSENGER_H 1

#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
//#include "G4UIcmdWith3VectorAndUnit.hh"
//#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "GeometryConstruction.hh"


class GeometryConstruction;

class G4UIdirectory;

class GeometryConstructionMessenger: public G4UImessenger{

public:

    GeometryConstructionMessenger( GeometryConstruction* );
    
    virtual ~GeometryConstructionMessenger();

    virtual void SetNewValue( G4UIcommand*, G4String );

private:

    GeometryConstruction* detector;

    G4UIdirectory* directory;
	
    G4UIcmdWithAnInteger* fTypeCmd;
	
    G4UIcmdWithAString*   fConfigCmd;

};

#endif
