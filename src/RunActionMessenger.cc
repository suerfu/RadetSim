/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file RunActionMessenger.cc
/// \brief Implementation of the RunActionMessenger class

#include "RunActionMessenger.hh"
#include "RunAction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"


RunActionMessenger::RunActionMessenger( RunAction* EvAct ) : G4UImessenger(),fRunAction(EvAct){
    
    G4String dir = "/filter/";

    fDir = new G4UIdirectory("/filter/");
    fDir->SetGuidance("Filter steps based on particle/volume.");

    fCmdIncludeWhenHit = new G4UIcmdWithAString( (dir+"recordWhenHit").c_str(), this );
    fCmdIncludeWhenHit->SetGuidance( "Record the track when a particle hits a particular volume." );
    fCmdIncludeWhenHit->SetParameterName( "SensitiveVolume", false );
    fCmdIncludeWhenHit->AvailableForStates(G4State_Idle);

    fCmdKillWhenHit = new G4UIcmdWithAString( (dir+"killWhenHit").c_str(), this );
    fCmdKillWhenHit->SetGuidance( "Kill the particle trajectory when it hits the specified volume." );
    fCmdKillWhenHit->SetParameterName( "VolumeName", false );
    fCmdKillWhenHit->AvailableForStates(G4State_Idle);

    fCmdExcludeParticle = new G4UIcmdWithAString( (dir+"excludeParticle").c_str(), this );
    fCmdExcludeParticle->SetGuidance( "Exclude certain particles from track recording." );
    fCmdExcludeParticle->SetParameterName( "ParticleName", false );
    fCmdExcludeParticle->AvailableForStates(G4State_Idle);

    fCmdExcludeVolume = new G4UIcmdWithAString( (dir+"excludeVolume").c_str(), this );
    fCmdExcludeVolume->SetGuidance( "Record the track when a particle hits a particular volume." );
    fCmdExcludeVolume->SetParameterName( "SensitiveVolume", false );
    fCmdExcludeVolume->AvailableForStates(G4State_Idle);

    fCmdExcludeProcess = new G4UIcmdWithAString( (dir+"excludeProcess").c_str(), this );
    fCmdExcludeProcess->SetGuidance( "Ignore steps defined by the process." );
    fCmdExcludeProcess->SetParameterName( "ProcessName", false );
    fCmdExcludeProcess->AvailableForStates(G4State_Idle);
}


RunActionMessenger::~RunActionMessenger(){

  delete fCmdIncludeWhenHit;
  delete fCmdKillWhenHit;

  delete fCmdExcludeParticle;
  delete fCmdExcludeVolume;
  delete fCmdExcludeProcess;
}


void RunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue){ 

    if( command==fCmdIncludeWhenHit ){
        fRunAction->AddRecordWhenHit( newValue);
    }
    else if( command==fCmdKillWhenHit ){
        fRunAction->AddKillWhenHit( newValue);
    }
    else if( command==fCmdExcludeParticle ){
        fRunAction->AddExcludeParticle( newValue );
    }
    else if( command==fCmdExcludeVolume ){
        fRunAction->AddExcludeVolume( newValue );
    }
    else if( command==fCmdExcludeProcess ){
        fRunAction->AddExcludeProcess( newValue );
    }
}

