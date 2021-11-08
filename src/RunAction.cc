
// $Id: RunAction.cc $
//
/// \file RunAction.cc
/// \brief Implementation of the RunAction class


#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "TFile.h"
#include "TTree.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction( CommandlineArguments* c) : G4UserRunAction(),
    fCmdlArgs( c ){

    // Configure the random engine.
    // The seed is first set by the current time.
    // It will be updated by the commandline parameter if provided.
    //
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    G4long seeds[2];
    time_t systime = time(NULL);
    if( fCmdlArgs->Find("seed") ){
        seeds[0] = stol( fCmdlArgs->Get("seed"));
    }
    else{
        seeds[0] = (long) systime;
    }
    seeds[1] = (long) (systime*G4UniformRand());
    
    G4Random::setTheSeeds(seeds);

    randomSeeds.push_back( seeds[0] );
    randomSeeds.push_back( seeds[1] );

    G4cout << "Seeds for random generator are " << seeds[0] << ", " << seeds[1] << G4endl;


    // Configure output
    //
    outputName = fCmdlArgs->Get( "output" );
    if( outputName=="" ){
        outputName = fCmdlArgs->Get( "o" );
    }

    outputFile = 0;
    dataTree = 0;

    G4RunManager::GetRunManager()->SetPrintProgress( 1 );
}


RunAction::~RunAction(){}


CommandlineArguments* RunAction::GetCommandlineArguments(){
    return fCmdlArgs;
}


void RunAction::SetOutputFileName( G4String newname ){
    outputName = newname;
}


void RunAction::BeginOfRunAction(const G4Run* /*run*/){

    if( outputName!="" ){

        outputFile = new TFile(outputName, "NEW");
        G4cout << "ROOT file " << outputName << " created." << G4endl;

        if( outputFile->IsOpen() ){
            dataTree = new TTree("events", "Track-level info for the run");
            G4cout << "TTree object created." << G4endl;
        }
    }
}



void RunAction::EndOfRunAction(const G4Run* /*run*/){

    if( outputFile!=0 ) {
        for( unsigned int i=0; i<macros.size(); i++){
            TMacro mac( macros[i] );
            mac.Write();    
        }

        std::stringstream ss;
        for( unsigned int i=0; i<randomSeeds.size(); i++)
            ss << randomSeeds[i] << '\t';

        TMacro randm( "randomSeeds");
        randm.AddLine( ss.str().c_str());
        randm.Write();

        outputFile->Write();
        outputFile->Close();
    }
}



TTree* RunAction::GetDataTree(){
    return dataTree;
}
