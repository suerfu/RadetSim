/*
    Author:  Suerfu Burkhant
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
// $Id: GeneratorMessenger.cc $
//
/// \file GeneratorMessenger.cc
/// \brief Definition of the GeneratorMessenger class

#include "GeneratorMessenger.hh"

#include "GeneratorAction.hh"

GeneratorMessenger::GeneratorMessenger( GeneratorAction* generator )  : G4UImessenger(), primaryGenerator(generator), primaryGeneratorDir(0) {

	primaryGeneratorDir = new G4UIdirectory("/generator/");
	primaryGeneratorDir->SetGuidance("Generator position, momentum and energy spectrum control.");

    cmdGPSInMaterial = new G4UIcmdWithAString( "/generator/setMaterial", this);
    cmdGPSInMaterial->SetGuidance( "Set the name of the material to generate particles.");
    cmdGPSInMaterial->SetParameterName( "world", false);
    cmdGPSInMaterial->AvailableForStates( G4State_PreInit, G4State_Idle);

    cmdSetSpectrum = new G4UIcmdWithAString( "/generator/spectrum", this);
    cmdSetSpectrum->SetGuidance( "Set the ROOT file containing polar angle and energy.");
    cmdSetSpectrum->SetParameterName( "foo.root", false);
    cmdSetSpectrum->AvailableForStates( G4State_PreInit, G4State_Idle);

	cmdSetParticle = new G4UIcmdWithAString( "/generator/particle", this);
	cmdSetParticle->SetGuidance( "Set the name of the particle.");
	cmdSetParticle->SetParameterName( "gamma", false);
	cmdSetParticle->AvailableForStates( G4State_PreInit, G4State_Idle);
    
}


GeneratorMessenger::~GeneratorMessenger(){
    delete cmdGPSInMaterial;
    delete cmdSetSpectrum;
    delete cmdSetParticle;
}


void GeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue){

	if( command == cmdSetSpectrum )	{
		primaryGenerator->SetSpectrum( newValue );
	}
    else if( command == cmdSetParticle ){
        primaryGenerator->SetParticleName( newValue );
    }
	else if( command == cmdGPSInMaterial ){
        primaryGenerator->GPSInMaterialSetMaterial( newValue );
    }

}

