/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file GeneratorAction.cc
/// \brief Implementation of the GeneratorAction class

#include "GeneratorAction.hh"
#include "GeneratorMessenger.hh"

#include "G4RunManager.hh"
//#include "G4Event.hh"
#include "G4ParticleGun.hh"
//#include "G4ParticleTable.hh"
//#include "G4ParticleDefinition.hh"
#include "G4GeneralParticleSource.hh"
//#include "G4SystemOfUnits.hh"
//#include "Randomize.hh"
//#include "G4ThreeVector.hh"
//#include "G4RandomDirection.hh"
//#include "G4IonTable.hh"



GeneratorAction::GeneratorAction( RunAction* runAction, GeometryManager* gManager) : G4VUserPrimaryGeneratorAction(),
    fRunAction( runAction),
    fGeometryManager( gManager) {
    
    fCmdlArgs = fRunAction->GetCommandlineArguments();

    fgun = new G4ParticleGun();
    fgps = new G4GeneralParticleSource();

    //Npostponed = 0;
    //NDelayed = 0;

    file = 0;
    tree = 0;
    index = 0;

    primaryGeneratorMessenger = new GeneratorMessenger(this);

    onwall = false;
    sample = false;

    wall_x = 11*m;
    wall_y = 12*m;
    wall_z = 3.2*m;

    world = 0;
}


GeneratorAction::~GeneratorAction(){
    
    if( !file==0 ){
        file->Close();
        delete file;
    }

    delete primaryGeneratorMessenger;
    delete world;
}


void GeneratorAction::SetSpectrum( G4String str ){

    G4cout << "Setting generator spectrum to " << str << G4endl;

    // If a file is already opened, close it first.
    if( file!=0 && file->IsOpen() ){
        file->Close();
    }

    G4cout << "Opening " << str << G4endl;

    // open in read mode (default)
    file = new TFile( str.c_str() );
    if( !file->IsOpen() ){
        G4cerr << "Error opening " << s << G4endl;
        return;
    }

    tree = (TTree*)file->Get("events");

    if( tree!=0 ){
        nentries = tree->GetEntries();
        tree->SetBranchAddress( "nParticle", &nparticle );
        tree->SetBranchAddress( "particle", &particle );
        tree->SetBranchAddress( "x", &x );
        tree->SetBranchAddress( "y", &y );
        tree->SetBranchAddress( "z", &z );
        tree->SetBranchAddress( "px", &px );
        tree->SetBranchAddress( "py", &py );
        tree->SetBranchAddress( "pz", &pz );
        tree->SetBranchAddress( "Eki", &E );
        /*
        if( tree->GetListOfBranches()->FindObject("theta") )
            tree->SetBranchAddress( "theta", &theta );
        if( tree->GetListOfBranches()->FindObject("theta") )
            tree->SetBranchAddress( "phi", &phi );
        */
    }

    sample = true;

}


void GeneratorAction::Sample( int n ){
    if( n>0 && tree!=0 ){
        index = n;
    }
}


void GeneratorAction::SetPosition(){
    fgun->SetParticlePosition( G4ThreeVector(x,y,z) );
}


void GeneratorAction::SetDirection(){
    fgun->SetParticleMomentumDirection( G4ThreeVector(px,py,pz) );
}


void GeneratorAction::SetEnergy(){
    fgun->SetParticleEnergy( E );
}



void GeneratorAction::GeneratePrimaries(G4Event* anEvent){
    
    if( sample==true ){
        tree->GetEntry( index%nentries );
        int counter = 0;
        do{
            fgun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( particle ) );
            fgun->SetParticlePosition( G4ThreeVector(x,y,z) );
            fgun->SetParticleMomentumDirection( G4ThreeVector(px,py,pz) );
            fgun->SetParticleEnergy( E );
            fgun->GeneratePrimaryVertex(anEvent);

            if( nparticle>1){
                index++;
                tree->GetEntry( index%nentries );
            }
            else{
                break;
            }
            counter++;
        }while( counter<nparticle);

        index++;
    }
    else{
        fgps->GeneratePrimaryVertex(anEvent);
    }
}

