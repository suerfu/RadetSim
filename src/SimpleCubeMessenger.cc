/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
#include "SimpleCubeMessenger.hh"
#include "SimpleCube.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"

SimpleCubeMessenger::SimpleCubeMessenger( SimpleCube* placement) : G4UImessenger(), detector( placement ){

    name = "target";
    dirname = G4String("/")+name+"/";

    directory = new G4UIdirectory( dirname.c_str() );
    directory->SetGuidance( "Commands to control the scattering target." );

    posCmd = new G4UIcmdWith3VectorAndUnit( (dirname+"pos").c_str(), this);
    posCmd->SetGuidance( "Set the position of the target to be placed.\nUnit is cm by default.");
    posCmd->SetParameterName( "x", "y", "z", false );
    posCmd->SetDefaultValue( G4ThreeVector( 0, 100*CLHEP::cm, 0) );
    posCmd->SetDefaultUnit( "cm" );
    posCmd->AvailableForStates( G4State_Idle );

    angCmd_x = new G4UIcmdWithADoubleAndUnit( (dirname+"rotateX").c_str(), this );
    angCmd_y = new G4UIcmdWithADoubleAndUnit( (dirname+"rotateY").c_str(), this );
    angCmd_z = new G4UIcmdWithADoubleAndUnit( (dirname+"rotateZ").c_str(), this );

    angCmd_x->SetGuidance( "Set the angle of rotation about X-axis.\nUnit is degree by default." );
    angCmd_y->SetGuidance( "Set the angle of rotation about Y-axis.\nUnit is degree by default." );
    angCmd_z->SetGuidance( "Set the angle of rotation about Z-axis.\nUnit is degree by default." );

    angCmd_x->SetParameterName( "Xangle", false );
    angCmd_y->SetParameterName( "Yangle", false );
    angCmd_z->SetParameterName( "Zangle", false );

    angCmd_x->SetDefaultValue( 0. );
    angCmd_y->SetDefaultValue( 0. );
    angCmd_z->SetDefaultValue( 0. );

    angCmd_x->SetDefaultUnit( "deg" );
    angCmd_y->SetDefaultUnit( "deg" );
    angCmd_z->SetDefaultUnit( "deg" );

    angCmd_x->AvailableForStates( G4State_Idle );
    angCmd_y->AvailableForStates( G4State_Idle );
    angCmd_z->AvailableForStates( G4State_Idle );

    materialCmd = new G4UIcmdWithAString( (dirname+"material").c_str(), this);
    materialCmd->SetGuidance( "Set the target material" );
    materialCmd->SetParameterName( "name", false);

    G4String matList;
    const G4MaterialTable* matTbl = G4Material::GetMaterialTable();
    for(size_t i=0;i<G4Material::GetNumberOfMaterials();i++){
        matList += (*matTbl)[i]->GetName();
        matList += " ";
    }

    materialCmd->SetCandidates(matList);
    materialCmd->AvailableForStates(G4State_Idle);

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

SimpleCubeMessenger::~SimpleCubeMessenger(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void SimpleCubeMessenger::SetNewValue( G4UIcommand* command, G4String newValue) {

    if( command==posCmd){
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
    }
    else if( command==materialCmd ){
        detector->SetMaterial( newValue );
    }
    return;

}
