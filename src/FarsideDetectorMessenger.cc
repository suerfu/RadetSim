
#include "FarsideDetectorMessenger.hh"
#include "FarsideDetector.hh"


FarsideDetectorMessenger::FarsideDetectorMessenger( GeometryManager* gm ) : G4UImessenger(), fGeometryManager( gm ){

    name = "farside";
    dirname = G4String("/")+name+"/";


    directory = new G4UIdirectory( dirname.c_str() );
    directory->SetGuidance( "Farside detector." );


    posCmd = new G4UIcmdWith3VectorAndUnit( (dirname+"pos").c_str(), this);
    posCmd->SetGuidance( "Set the position of the farside detector to be placed.\nUnit is cm by default.");
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

    placeCmd = new G4UIcmdWithAString( (dirname+"place").c_str(), this );
    placeCmd->SetGuidance( "Place a far-side detector with the previously specified positions and angles." );
    placeCmd->SetParameterName( "name", false );
    placeCmd->AvailableForStates( G4State_Idle );

}



FarsideDetectorMessenger::~FarsideDetectorMessenger(){
}


void FarsideDetectorMessenger::SetNewValue( G4UIcommand* command, G4String newValue) {

    if( command==posCmd ){
        pos = posCmd->GetNew3VectorValue( newValue );
    }
    else if( command==angCmd_x ){
        if( rot==0 ){
            rot = new G4RotationMatrix();
        }
        rot->rotateX( angCmd_x->GetNewDoubleValue( newValue ) );
    }
    else if( command==angCmd_y ){
        if( rot==0 ){
            rot = new G4RotationMatrix();
        }
        rot->rotateY( angCmd_y->GetNewDoubleValue( newValue ) );
    }
    else if( command==angCmd_z ){
        if( rot==0 ){
            rot = new G4RotationMatrix();
        }
        rot->rotateZ( angCmd_z->GetNewDoubleValue( newValue ) );
    }
    else if( command==placeCmd ){
        if( fGeometryManager->GetPhysicalVolume(newValue)==0 ){
            FarsideDetector* det = new FarsideDetector( fGeometryManager );
            det->PlaceDetector( newValue, pos, rot );
            pos = G4ThreeVector(0,0,0);
            rot = 0;
            fGeometryManager->GeometryHasBeenModified();
        }
        else{
            G4cerr << "Detector with name " << newValue << " already exists!" << G4endl;
            pos = G4ThreeVector(0,0,0);
            delete rot;
            rot = 0;
        }
    }
    return;

}
