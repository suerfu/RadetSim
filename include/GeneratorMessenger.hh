
// $Id: GeneratorMessenger.hh $
//
/// \file GeneratorMessenger.hh
/// \brief Definition of the GeneratorMessenger class

#ifndef GeneratorMessenger_h
#define GeneratorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"

class GeneratorAction;
class G4UIdirectory;

class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;

class GeneratorMessenger : public G4UImessenger {

public:

	GeneratorMessenger(GeneratorAction*);
	virtual ~GeneratorMessenger();

	virtual void SetNewValue(G4UIcommand*, G4String);

private:

	GeneratorAction* primaryGenerator;

	G4UIdirectory* primaryGeneratorDir;

	G4UIcmdWithAString* cmdSetSpectrum;

    G4UIcmdWithoutParameter* cmdSample;
    G4UIcmdWithoutParameter* cmdSetPosition;
    G4UIcmdWithoutParameter* cmdSetDir;
    G4UIcmdWithoutParameter* cmdSetEnergy;
    G4UIcmdWithoutParameter* cmdSetWall;

};

#endif
