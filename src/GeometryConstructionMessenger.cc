/*
    Author:  Suerfu Burkhant
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
#include "G4UIcmdWithoutParameter.hh"

#include "GeometryManager.hh"


GeometryConstructionMessenger::GeometryConstructionMessenger( GeometryConstruction* placement) : G4UImessenger(){

    detector = placement;

    // geometry directory to place the user-defined commands
    //
    directory = new G4UIdirectory( "/geometry/" );
    directory->SetGuidance( "Set geometry type and configuration files." );

    // specify the type of the simulation geometry
    // different types may correspond to different geometries
    //
	fTypeCmd = new G4UIcmdWithAnInteger( "/geometry/type", this );
	fTypeCmd->SetGuidance( "Set the geometry type." );
        // Note: some detailed explanation of geometry types goes here.
	fTypeCmd->SetParameterName( "type", true );
	fTypeCmd->SetDefaultValue( 0 );

    // load the geometry configuration file.
    //
   	fConfigCmd = new G4UIcmdWithAString( "/geometry/loadconfig", this );
	fConfigCmd->SetGuidance( "Load the specified configuration file, which includes necessary parameters as name-value pairs." );
 
}


GeometryConstructionMessenger::~GeometryConstructionMessenger(){}


void GeometryConstructionMessenger::SetNewValue( G4UIcommand* command, G4String newValue) {

	G4cout << GetClassName() <<": SetNewValue " << newValue << G4endl;

	if( command == fTypeCmd ){
		GeometryManager::Get()->SetGeometryType( fTypeCmd->ConvertToInt(newValue) );
		G4cout << GetClassName() <<": The type of simulation geometry is " << GeometryManager::Get()->GetGeometryType() << G4endl;
	}
	else if( command == fConfigCmd ){
		GeometryManager::Get()->LoadFile( newValue );
	}
}
