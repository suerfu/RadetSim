
// $Id: SimpleCubeMessenger.cc $
//
/// \file SimpleCubeMessenger.cc
/// \brief Definition of the SimpleCubeMessenger class

#include "SimpleCubeMessenger.hh"
#include "SimpleCube.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"

SimpleCubeMessenger::SimpleCubeMessenger( SimpleCube* placement) : G4UImessenger(), detector( placement ){

    G4String name = "simplecube";
    G4String dirname = G4String("/")+simplecube+"/";

    directory = new G4UIdirectory( dirname );
    directory->SetGuidance( "A simple cube." );

    LinCmd = new G4UIcmdWithADoubleAndUnit( dirname+"Lin", this );
    LoutCmd = new G4UIcmdWithADoubleAndUnit( dirname+"Lout", this );

    LinCmd->SetGuidance( "Set the length of inner side.\nUnit is cm by default." );
    LoutCmd->SetGuidance( "Set the length of outer side.\nUnit is cm by default." );

    LinCmd->AvailableForStates( G4State_Idle );
    LoutCmd->AvailableForStates( G4State_Idle );

    LinCmd->SetParameterName( "Lin", false );
    LoutCmd->SetParameterName( "Lout", false );

    LinCmd->SetDefaultValue( 5. );
    LoutCmd->SetDefaultValue( 10. );

    LinCmd->SetDefaultUnit( "cm" );
    LoutCmd->SetDefaultUnit( "cm" );

    posCmd = new G4UIcmdWith3VectorAndUnit( dirname+"pos", this);
    posCmd->SetGuidance( "Set the position of the object to be placed.\nUnit is cm by default.");
    posCmd->SetParameterName( "x", "y", "z", false );
    posCmd->AvailableForStates( G4State_Idle );
    posCmd->SetDefaultValue( G4ThreeVector( 0, 100*CLHEP::cm, 0) );
    posCmd->SetDefaultUnit( "cm" );

    angCmd_x = new G4UIcmdWithADoubleAndUnit( dirname+"rotateX", this );
    angCmd_y = new G4UIcmdWithADoubleAndUnit( dirname+"rotateY", this );
    angCmd_z = new G4UIcmdWithADoubleAndUnit( dirname+"rotateZ", this );

    angCmd_x->SetGuidance( "Set the angle of rotation about X-axis.\nUnit is degree by default." );
    angCmd_y->SetGuidance( "Set the angle of rotation about Y-axis.\nUnit is degree by default." );
    angCmd_z->SetGuidance( "Set the angle of rotation about Z-axis.\nUnit is degree by default." );

    angCmd_x->AvailableForStates( G4State_Idle );
    angCmd_y->AvailableForStates( G4State_Idle );
    angCmd_z->AvailableForStates( G4State_Idle );

    angCmd_x->SetParameterName( "Xangle", false );
    angCmd_y->SetParameterName( "Yangle", false );
    angCmd_z->SetParameterName( "Zangle", false );

    angCmd_x->SetDefaultValue( 0. );
    angCmd_y->SetDefaultValue( 0. );
    angCmd_z->SetDefaultValue( 0. );

    angCmd_x->SetDefaultUnit( "deg" );
    angCmd_y->SetDefaultUnit( "deg" );
    angCmd_z->SetDefaultUnit( "deg" );

    place_detector = new G4UIcommand( dirname+"place", this );
    place_detector->SetGuidance( "Place a far-side detector based on the previously specified positions and angles." );
    place_detector->AvailableForStates( G4State_Idle );

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

SimpleCubeMessenger::~SimpleCubeMessenger(){
//    delete FSDistanceCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void SimpleCubeMessenger::SetNewValue( G4UIcommand* command, G4String newValue) {

    if( command==LinCmd ){
        detector->SetInnerLength( LinCmd->GetNewDoubleValue( newValue) );
    }
    else if( command==LoutCmd ){
        detector->SetOuterLength( LoutCmd->GetNewDoubleValue( newValue) );
    }
    else if( command==posCmd){
        detector->SetPosition( posCmd->GetNew3VectorValue( newValue ) );
    }
    else if( command==angCmd_x ){
        detector->RotateX( angCmd_x->GetNewDoubleValue( newValue) );
    }
    else if( command==angCmd_y ){
        detector->RotateY( angCmd_y->GetNewDoubleValue( newValue) );
    }
    else if( command==angCmd_z ){
        detector->RotateZ( angCmd_z->GetNewDoubleValue( newValue) );
    }/*
    else if( command==place_detector ){
        detector->PlaceFarSideDetector();
    }*/
    return;

}
