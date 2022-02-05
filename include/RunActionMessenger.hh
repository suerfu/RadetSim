/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

#ifndef RUNACTIONMESSENGER_H
#define RUNACTIONMESSENGER_H 1

#include "G4UImessenger.hh"
//#include "G4UIcommand.hh"
//#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "globals.hh"

class RunAction;

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithAString;

class RunActionMessenger: public G4UImessenger{

public:

    RunActionMessenger( RunAction* );

    ~RunActionMessenger();

    virtual void SetNewValue( G4UIcommand*, G4String );

private:

    RunAction* fRunAction;

    G4UIdirectory* fDir;

    G4UIcmdWithAString* fCmdIncludeWhenHit;
    G4UIcmdWithAString* fCmdKillWhenHit;

    G4UIcmdWithAString* fCmdExcludeParticle;
    G4UIcmdWithAString* fCmdExcludeVolume;
    G4UIcmdWithAString* fCmdExcludeProcess;

};

#endif

