
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

    output_file = 0;
    data_tree = 0;
    output_name = "";

    G4RunManager::GetRunManager()->SetPrintProgress(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction(){}

CommandlineArguments* RunAction::GetCommandlineArguments(){
    return fCmdlArgs;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::SetOutputFileName(G4String newname){
    output_name = newname;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* /*run*/){
    if( output_name!="" ){
        output_file = new TFile(output_name, "NEW");
        G4cout << "Output ROOT file " << output_name << " created." << G4endl;

        if( output_file->IsOpen() ){
            data_tree = new TTree("events", "Track-level info for the run");
            G4cout << "Output TTree object created." << G4endl;
        }
    }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* /*run*/){

    if( output_file!=0 ) {
        for( unsigned int i=0; i<macros.size(); i++){
            TMacro mac( macros[i] );
            mac.Write();    
        }

        std::stringstream ss;
        for( unsigned int i=0; i<random_seeds.size(); i++)
            ss << random_seeds[i] << '\t';

        TMacro randm( "rand_seeds");
        randm.AddLine( ss.str().c_str());
        randm.Write();

        output_file->Write();
        output_file->Close();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TTree* RunAction::GetDataTree(){
    return data_tree;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
