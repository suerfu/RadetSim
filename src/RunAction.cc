/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file RunAction.cc
/// \brief Implementation of the RunAction class


#include "RunAction.hh"
#include "RunActionMessenger.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "TFile.h"
#include "TTree.h"


RunAction::RunAction( CommandlineArguments* c) : G4UserRunAction(),
     fRunActionMessenger(0), fCmdlArgs( c ){

    fRunActionMessenger = new RunActionMessenger( this );

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
    seeds[1] = (long) (seeds[0]*G4UniformRand());
    
    G4Random::setTheSeeds(seeds);

    // Aldo keep a record of randomSeeds for later output
    //
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

    // If output name is specified, create a ROOT file and a TTree.
    //
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


void RunAction::AddRecordWhenHit( G4String a){ recordWhenHit.insert(a); }


bool RunAction::RecordWhenHit( G4String s ){
    if( recordWhenHit.empty()==true ){
        return true;
            // If no volume is specified, record everything.
    }
    else{
        return recordWhenHit.find(s)!=recordWhenHit.end();
    }
}


void RunAction::AddKillWhenHit( G4String a){ killWhenHit.insert(a); }

bool RunAction::KillWhenHit( G4String s ){
    return killWhenHit.find(s)!=killWhenHit.end();
}


void RunAction::AddExcludeParticle( G4String a){ excludeParticle.insert(a); }

bool RunAction::ExcludeParticle( G4String s){
    return excludeParticle.find(s)!=excludeParticle.end();
}


void RunAction::AddExcludeVolume( G4String a){ excludeVolume.insert(a); }

bool RunAction::ExcludeVolume( G4String s){
    return excludeVolume.find(s)!=excludeVolume.end();
}


void RunAction::AddExcludeProcess( G4String a){ excludeProcess.insert(a); }

bool RunAction::ExcludeProcess( G4String s){
    return excludeProcess.find(s)!=excludeProcess.end();
}



