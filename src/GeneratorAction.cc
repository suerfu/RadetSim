//
/// \file GeneratorAction.cc
/// \brief Implementation of the GeneratorAction class

#include "GeneratorAction.hh"
#include "GeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4ThreeVector.hh"
#include "G4RandomDirection.hh"
#include "G4IonTable.hh"

#include <cmath>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


GeneratorAction::GeneratorAction( RunAction* runAction) : G4VUserPrimaryGeneratorAction(),
    fRunAction( runAction) {
    
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


void GeneratorAction::SetSpectrum( string str ){

    G4cout << "Setting spectrum to " << str << G4endl;


    if( file!=0 && file->IsOpen() ){
        file->Close();
    }

    G4cout << "Opening " << str << G4endl;

    file = new TFile( str.c_str() );
        // open in read mode (default)
    if( !file->IsOpen() ){
        G4cerr << "Error opening " << s << G4endl;
        return;
    }

    tree = (TTree*)file->Get("events");

    if( tree!=0 ){
        nentries = tree->GetEntries();
        tree->SetBranchAddress( "particle", &particle );
        tree->SetBranchAddress( "nParticle", &nparticle );
        tree->SetBranchAddress( "x", &x );
        tree->SetBranchAddress( "y", &y );
        tree->SetBranchAddress( "z", &z );
        tree->SetBranchAddress( "px", &px );
        tree->SetBranchAddress( "py", &py );
        tree->SetBranchAddress( "pz", &pz );
        tree->SetBranchAddress( "Eki", &E );
        if( tree->GetListOfBranches()->FindObject("theta") )
            tree->SetBranchAddress( "theta", &theta );
        if( tree->GetListOfBranches()->FindObject("theta") )
            tree->SetBranchAddress( "phi", &phi );            
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


void GeneratorAction::ConfineOnWall(){
    
    if( world==0 ){
        world = new G4Box( "world", wall_x/2, wall_y/2, wall_z/2);
    } 
    onwall = true;
}


void GeneratorAction::GeneratePrimaries(G4Event* anEvent){
    
    if( sample==true ){
        tree->GetEntry( index%nentries );
        int counter = 0;
        do{
            fgun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( particle ) );
            // If particles to be generated on the world wall, confine to wall and generate theta/phi w.r.t. normal.
            if( onwall ==true ){
                // Set particle position
                G4ThreeVector pos = world->GetPointOnSurface();
                fgun->SetParticlePosition( pos );

                // Set particle momentum direction
                G4ThreeVector original = -world->SurfaceNormal( pos );

                G4ThreeVector rotate1 = original.orthogonal().unit();
                    // A vector perpendicular to normal. Rotation w.r.t. this axis generates the polar angle.
                G4ThreeVector rotate2 = original.unit();
                    // This is the original normal direction. Phi rotation will be w.r.t. this axis.
                original.rotate( theta, rotate1/rotate1.mag() );
                original.rotate( phi, rotate2/rotate2.mag() );

                fgun->SetParticleMomentumDirection( original );
            }
            else{
                fgun->SetParticlePosition( G4ThreeVector(x,y,z) );
                fgun->SetParticleMomentumDirection( G4ThreeVector(px,py,pz) );
            }

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

