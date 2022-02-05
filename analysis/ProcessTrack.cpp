#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>

#include "TrackReader.h"

#include "TTree.h"
#include "TFile.h"

using namespace std;

bool IsOpt( string tmp ){
    if( tmp.size()>1 ){
        if( tmp[0]=='-'){
            if(tmp[1]=='-' || isalpha(tmp[1])>0 )
                return true;
        }
    }
    return false;
}

typedef vector<string> Parameter;
typedef map<string, Parameter> ParameterMap;

class Commandline{

public:

    Commandline( int argc, char* argv[]){
        Parse( argc, argv);
    }

    ~Commandline(){}

    void Parse( int argc, char* argv[]){

        map.clear();

        string key;
        string tmp;

        for( unsigned int i=0; i<argc; i++){

            tmp = string( argv[i] );
    
            if( IsOpt(tmp) || i==0 ){
                key = tmp;
                map[key] = Parameter();
            }
            else{
                map[key].push_back(tmp);
            }
        }
    }

    bool Find( string key ){ return map.find(key)!=map.end(); }
    
    vector<string> Get( string key ){ return map[key]; }

    void Print(){
        ParameterMap::iterator itr;
        for( itr=map.begin(); itr!=map.end(); itr++ ){
            cout << itr->first << ":";
            for( Parameter::iterator itr2 = itr->second.begin(); itr2!=itr->second.end(); itr2++ ){
                cout << " " << *itr2;
            }
            cout << endl;
        }
    }

    int Size(){ return map.size(); }

private:

    ParameterMap map;

};


int main( int argc, char* argv[]){ 

    // Parse commandline arguments
    //
    Commandline cmdl( argc, argv);

    if( cmdl.Size()==1 || cmdl.Find("-h") || cmdl.Find("--help") ){
        cout << "\nusage: " << argv[0] << "[--active x y z] --voi u v w --input f0.root f1.root ... --output foo.root\n\n";
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
    if( cmdl.Find("--voi")==false ){
        cerr << "No volume-of-interest found. Specify it with --voi option.\n";
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
    cout << "Creating " << outputFile << " from";
    for( vector<string>::iterator itr=inputFiles.begin(); itr!=inputFiles.end(); itr++ )
        cout << " " << *itr;
    cout << endl;

    cout << "Energy deposits in";
    for( vector<string>::iterator itr=activeVolumes.begin(); itr!=activeVolumes.end(); itr++ )
        cout << " " << *itr;
    cout << " will be used to find and cluster events.\n";

    cout << "Energy deposits in";
    for( vector<string>::iterator itr=voi.begin(); itr!=voi.end(); itr++ )
        cout << " " << *itr;
    cout << " will be recorded.\n";

    // Initialize the reader object to read and process the tracks and generate output file with event-level info..
    TrackReader reader;

    reader.SetActiveVolume( activeVolumes );
    reader.SetVoI( voi );

    if( cmdl.Find("--daq")==true )
        reader.SetDAQWindow( stof(cmdl.Get("--daq")[0]) ); 
    else
        reader.SetDAQWindow( 1000 );

    if( cmdl.Find("--coin")==true )
        reader.SetCoinWindow( stof(cmdl.Get("--coin")[0]) ); 
    else
        reader.SetCoinWindow( 1 );

    reader.Process( outputFile, inputFiles );

    reader.Reset();

    return 0;
}


