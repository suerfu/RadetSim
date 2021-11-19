/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TMacro.h"

#include <vector>
#include <sstream>

#include "utility.hh"

class G4Run;
class RunActionMessenger;

class RunAction : public G4UserRunAction {

public:

    RunAction( CommandlineArguments* c);

    virtual ~RunAction();

    virtual void SetOutputFileName( G4String newname );

    virtual void BeginOfRunAction( const G4Run* );
    virtual void EndOfRunAction( const G4Run* );

    void AddMacro( G4String s){
        macros.push_back( s );
    }

    /*
    void AddRandomSeeds( long seeds[], int len){
        for( int i=0; i<len; i++)
            randomSeeds.push_back( seeds[i]);
    }
    */

    TTree* GetDataTree();

    CommandlineArguments* GetCommandlineArguments();

private:

    CommandlineArguments* fCmdlArgs;

    G4String outputName;
    
    TFile* outputFile;
    TTree* dataTree;

    std::vector< G4String > macros;
    std::vector< long > randomSeeds;

};


#endif
