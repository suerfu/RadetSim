/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
// $Id: GeometryConstructionMessenger.cc $
//
/// \file GeometryConstructionMessenger.cc
/// \brief Definition of the GeometryConstructionMessenger class

#include "GeometryConstructionMessenger.hh"
#include "GeometryConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"

GeometryConstructionMessenger::GeometryConstructionMessenger( GeometryConstruction* placement) : G4UImessenger(), detector( placement ){
/*
    directory = new G4UIdirectory( "/placement/" );
    directory->SetGuidance( "Far-side/backing detector placement." );

    posCmd = new G4UIcmdWith3VectorAndUnit( "/placement/pos", this);
    posCmd->SetGuidance( "Set the position of the farside detector to be placed.\nUnit is in cm by default.");
    posCmd->SetParameterName( "x", "y", "z", false );
    posCmd->AvailableForStates( G4State_Idle );
    posCmd->SetDefaultValue( G4ThreeVector(0,100*CLHEP::cm,0) );
    posCmd->SetDefaultUnit( "cm" );

    angCmd_x = new G4UIcmdWithADoubleAndUnit( "/placement/rotateX", this );
    angCmd_y = new G4UIcmdWithADoubleAndUnit( "/placement/rotateY", this );
    angCmd_z = new G4UIcmdWithADoubleAndUnit( "/placement/rotateZ", this );

    angCmd_x->SetGuidance( "Set the angle of rotation about X-axis of the farside detector to be placed.\nUnit is in degree by default." );
    angCmd_y->SetGuidance( "Set the angle of rotation about Y-axis of the farside detector to be placed.\nUnit is in degree by default." );
    angCmd_z->SetGuidance( "Set the angle of rotation about Z-axis of the farside detector to be placed.\nUnit is in degree by default." );

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

    place_detector = new G4UIcommand( "/placement/placeDetector", this );
    place_detector->SetGuidance( "Place a far-side detector based on the previously specified positions and angles." );
    place_detector->AvailableForStates( G4State_Idle );
*/
}



GeometryConstructionMessenger::~GeometryConstructionMessenger(){
//    delete FSDistanceCmd;
}


void GeometryConstructionMessenger::SetNewValue( G4UIcommand* command, G4String newValue) {
/*
    if( command==posCmd ){
        detector->SetFarSidePosition( posCmd->GetNew3VectorValue( newValue ) );
    }
    else if( command==angCmd_x ){
        detector->FarSideRotateX( angCmd_x->GetNewDoubleValue( newValue) );
    }
    else if( command==angCmd_y ){
        detector->FarSideRotateY( angCmd_y->GetNewDoubleValue( newValue) );
    }
    else if( command==angCmd_z ){
        detector->FarSideRotateZ( angCmd_z->GetNewDoubleValue( newValue) );
    }
    else if( command==place_detector ){
        detector->PlaceFarSideDetector();
    }
    return;
*/
}
