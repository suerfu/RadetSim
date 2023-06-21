#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>

#include "TrackReader.h"
#include "CommandlineHandler.h"

#include "TTree.h"
#include "TFile.h"

using namespace std;


// Print usage information of this program.
//
void PrintUsage( char** argv);


int main( int argc, char* argv[] ){ 

    // Parse commandline arguments
    //
    CommandlineHandler cmdl( argc, argv);

    if( cmdl.Size()==1 || cmdl.Find("-h") || cmdl.Find("--help") ){
        PrintUsage( argv );
        return 0;
    }

    // Check if necessary parameters have been specified.
    //
    if( cmdl.Find("--input")==false ){
        cerr << "No input file found. Specify it with --input option..\n";
        return -1;
    }
    if( cmdl.Find("--output")==false ){
        cerr << "No output file name found. Specify it with --output option.\n";
        return -1;
    }
    if( cmdl.Find("--active")==false ){
        cerr << "No active volume found. Specify it with --active option.\n";
        return -1;
    }

    // Define a number of variables
    //
    vector<string> inputFiles = cmdl.Get("--input");
        // input files to be processed.

    string outputFile = cmdl.Get("--output")[0];
        // output ROOT file for writing.

    vector<string> activeVolumes = cmdl.Get("--active");
        // active volume is used to find and group events.
        // edep within DAQ window is grouped into an interaction.

    vector<string> voi = cmdl.Get("--voi");
        // edep in volumes of interest will be recorded.

    // Print the above parameters so that user knows if the program is executing with the correct parameters.
    //
    cout << "Energy deposits in";
    for( vector<string>::iterator itr=activeVolumes.begin(); itr!=activeVolumes.end(); itr++ )
        cout << " " << *itr;
    cout << " will be used to find and cluster events.\n";

    if( voi.empty()==false ){
        cout << "Energy deposits in";
        for( vector<string>::iterator itr=voi.begin(); itr!=voi.end(); itr++ )
            cout << " " << *itr;
        cout << " will be recorded.\n";
    }

    // Initialize the reader object to read and process the tracks and generate output file with event-level info..
    TrackReader reader;

    reader.SetActiveVolume( activeVolumes );
    reader.SetVoI( voi );

    if( cmdl.Find("--daq")==true ){
        reader.SetDAQWindow( stof(cmdl.Get("--daq")[0]) );
    }
    else{
        reader.SetDAQWindow( 1000 );
    }

    if( cmdl.Find("--coin")==true ){
        reader.SetCoinWindow( stof(cmdl.Get("--coin")[0]) );
    }
    else{
        reader.SetCoinWindow( 100 );
    }

    if( cmdl.Find("--no-parent")==true ){
        reader.SetParentInfo( false );
    }

    if( cmdl.Find("--ancestor")==true ){
        reader.SetAncestorInfo( true );
    }


    if( cmdl.Find("--force-create")==true || cmdl.Find("-f")==true ){
        reader.SetForceCreate( true );
    }

    reader.Process( outputFile, inputFiles );

    reader.Reset();

    return 0;
}


void PrintUsage( char* argv[] ){
    cout << "\nusage: " << argv[0] << " [--option param1 param2 ...] --input f0.root f1.root ... --output foo.root\n\n";
    cout << "Options:\n";
    cout << "\n--active: " << "Specifies the active volume(s). This is an necessary option." << endl;
    cout << "          " << "Energy deposits in the specified active volume(s) will be used to construct events." << endl;
    cout << "\n--voi:    " << "Specifies volume(s) of interest." << endl;
    cout << "          " << "Energy deposits in the volumes of interest will be summed and recorded for each event in active volume(s)." << endl;
    cout << "          " << "If not specified, all volumes involved in the simulation will be taken as volumes of interest, which may take longer." << endl;
    cout << "\n--daq:    " << "Specifies acquisition window in ns. Default 1000 ns." << endl;
    cout << "          " << "Energy deposits within this window will be summed and reported as the energy deposit of the event." << endl;
    cout << "\n--coin:   " << "Specifies coincidence window in ns for more than one active volumes/other volumes of interest. Default 100 ns." << endl;
    cout << "          " << "Once a leading energy deposit in the active volume is identified, if there are energy deposits within this window in other active volumes and volumes of interest." << endl;
    cout << "          " << "Energy deposits within acquisition window in these volumes will be summed and reported as coincident events in the corresponding volumes." << endl;
    cout << "\n--no-parent:       " << "If specified, the information of the original/initial parent particle information causing the event is NOT recorded (default to record)." << endl;
    cout << "\n--ancestor:        " << "If specified, the information of the very original parent particle informatiof the event IS recorded (default not to record)." << endl;
    cout << "\n-f,--force-create: " << "Output file will be overwritten when the same file already exists. By default, no overwriting.\n" << endl;
}
