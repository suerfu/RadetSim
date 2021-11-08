// $Id: GeneratorMessenger.cc $
//
/// \file GeneratorMessenger.cc
/// \brief Definition of the GeneratorMessenger class

#include "GeneratorMessenger.hh"

#include "GeneratorAction.hh"

GeneratorMessenger::GeneratorMessenger( GeneratorAction* generator )  : G4UImessenger(), primaryGenerator(generator), primaryGeneratorDir(0) {

	primaryGeneratorDir = new G4UIdirectory("/generator/");
	primaryGeneratorDir->SetGuidance("Generator position, momentum and energy spectrum control.");

    cmdSetSpectrum = new G4UIcmdWithAString("/generator/spectrum", this);
    cmdSetSpectrum->SetGuidance("Set the ROOT file from which to sample position, direction and energy.");
    cmdSetSpectrum->SetParameterName("foo.root", false);
    cmdSetSpectrum->AvailableForStates(G4State_PreInit, G4State_Idle);

    cmdSample = new G4UIcmdWithoutParameter("/generator/sample", this);
    cmdSample->SetGuidance("Increment the sample event index of the ROOT tree of energy spectrum.");
    cmdSample->AvailableForStates(G4State_PreInit, G4State_Idle);
    
    cmdSetPosition = new G4UIcmdWithoutParameter("/generator/setPos", this);
    cmdSetPosition->SetGuidance("Set the position of next primary vertex from ROOT file.");
    cmdSetPosition->AvailableForStates(G4State_PreInit, G4State_Idle);

    cmdSetDir = new G4UIcmdWithoutParameter("/generator/setDir", this);
    cmdSetDir->SetGuidance("Set the direction of next primary vertex from ROOT file.");
    cmdSetDir->AvailableForStates(G4State_PreInit, G4State_Idle);

    cmdSetEnergy = new G4UIcmdWithoutParameter("/generator/setEnergy", this);
    cmdSetEnergy->SetGuidance("Set the energy of next primary vertex from ROOT file.");
    cmdSetEnergy->AvailableForStates(G4State_PreInit, G4State_Idle);

    cmdSetWall = new G4UIcmdWithoutParameter("/generator/confineOnWall", this);
    cmdSetWall->SetGuidance("Assumes the source is on the wall. x,y,z in the ROOT file will be irrelevant and theta and phi will be w.r.t. the normal direction of the surface.");
    cmdSetWall->AvailableForStates(G4State_PreInit, G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

GeneratorMessenger::~GeneratorMessenger(){

    delete cmdSetSpectrum;
    delete cmdSample;
    delete cmdSetPosition;
    delete cmdSetDir;
    delete cmdSetEnergy;
    delete cmdSetWall;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void GeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue){

	if( command == cmdSetSpectrum )	{
		primaryGenerator->SetSpectrum( newValue);
	}

    else if( command == cmdSample ){
        primaryGenerator->Sample();
    }

    else if( command == cmdSetPosition ){
        primaryGenerator->SetPosition();
    }

    else if( command == cmdSetDir ){
        primaryGenerator->SetDirection();
    }

    else if( command == cmdSetWall ){
        primaryGenerator->ConfineOnWall();
    }

    else if( command == cmdSetEnergy ){
        primaryGenerator->SetEnergy();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
