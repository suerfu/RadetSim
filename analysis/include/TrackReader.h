#ifndef TRACKREADER_H
#define TRACKREADER_H 1

#include <string>
#include <vector>
#include <map>

#include "MCPulseArray.h"
#include "StepInfo.h"

#include "TFile.h"
#include "TTree.h"
#include "TMacro.h"

using namespace std;

class TrackReader{

public:

    // Constructor
    TrackReader(){

        // Initialize with default values
        //
        daqWindow = 1000;   // DAQ window in ns
        coinWindow = 100;   // 100 ns coincidence window

        parentInfo = true;
        ancestorInfo = false;

        forceCreate = false;
    }

    // Destructor
    //
    ~TrackReader(){;}

    void Reset(){}

    // Add an active volume
    //
    void SetActiveVolume( vector<string> name){ arrayAV = name;}
        // Active volume is by which event clustering will be performed

    // Add a volume of interest.
    //
    void SetVoI( vector<string> name ){ arrayVOI = name; }
        // A volume of interest is by definition volume which energy deposit will be summed and recorded

    // Sets the (hypothetical) DAQ window in microsecond.
    //
    void SetDAQWindow( float win = 1){ daqWindow = win;}
        // Events ocurring within this window is regarded as the same event.

    float GetDAQWindow(){ return daqWindow; }

    // Sets the coincidence window in microseconds
    //
    void SetCoinWindow( float win = 1){ coinWindow = win;}
        // Events ocurring within this window from an event in active volume will be recorded.
    
    float GetCoinWindow(){ return coinWindow; }

    // Process the tracks in input and write output as a ROOT TTree in output.
    //
    void Process( string output, vector<string> inputs );

    unsigned int GetMaxFileLength( vector<string> inputs );

    string GetStrippedString( string a ){
        std::size_t found = a.rfind('/');
        if( found!=std::string::npos )
            return a.substr( a.rfind("/")+1, a.size() );
        return a;
    }

    // Get duration of simulation in seconds
    //
    double GetTimeSimulated( TMacro run, TMacro geo );

    // Return whether parent info should be recorded in the output
    //
    bool GetParentInfo(){ return parentInfo; }

    void SetParentInfo( bool a ){ parentInfo = a;}

    // Return whether ancestor/top of chain info should be recorded in the output
    //
    bool GetAncestorInfo(){ return ancestorInfo; }

    void SetAncestorInfo( bool a ){ ancestorInfo = a;}

    // Returns whether output file should be force recreated if existing.
    //
    bool GetForceCreate(){ return forceCreate; }

    void SetForceCreate( bool f ){ forceCreate=f; }

private:

    static const unsigned int MAX_FILENAME_LEN = 128;

    double daqWindow;
    double coinWindow;

    bool parentInfo;
        // this is the isotope whose decay is directly responsible for the energy deposit.
    bool ancestorInfo;
        // this is the top of the decay chain

    bool forceCreate;

    vector<string> arrayAV;
        // This array holds the active volumes.
    vector<string> arrayVOI;
        // This array holds the volumes of interest.
        // If it's zero, will treat all volumes as volume of interest.

    vector<string> GetVOIFromFile( vector<string> inputName);
        // Iterate through given files to generate a list of volumes involved.


    void ConfigureTTree( TTree* tree);

    // Set branch variables to the default values.
    void DefaultBranchVariables(){
        //map<string, double>::iterator itr;
        for( auto itr = energyDeposit.begin(); itr!=energyDeposit.end(); itr++ ){
            double* dptr = itr->second;
            for( unsigned int i=0; i<MCPulse::GetNType(); i++){
                dptr[i] = 0;
            }
        }
        timeStamp = -1;
    }

    char inputFileNameChar[ MAX_FILENAME_LEN ];
        // name of the file to which this event belongs to
    unsigned int max_file_len;

    unsigned int ID;
        // global ID of the event processed
    unsigned int eventID;
        // event ID in its own run.
    unsigned int clusterIndex;
        // index of the event cluster when multiple event occurred in the DAQ window.
    double timeStamp;
        // timeStamp of the interaction.

    unsigned int parentID;
        // This is the immediate mother of the decay product causing the edep

    unsigned int ancestorID;
        // This is the very original radioactive isotope.

    static const int max_particle_len = 16;
        // max length of characters for particle name
    static const int max_volume_len = 20;
        // max length of characters for volume and process names

    char parentParticle[ max_particle_len ];
    char ancestorParticle[ max_particle_len ];

    char parentVolume[ max_volume_len ];
    char ancestorVolume[max_volume_len ];

    double parentPosition[3];
    double ancestorPosition[3];

    double parentEki;
    double ancestorEki;

    bool IsRadioactiveDecay( char* proc ){ return strncmp( proc, "RadioactiveDeca", 15)==0; }

    double Nsimulated;
        // Total No. of particles simulated in this run

    double Tsimulated;
        // Total days simulated.
        // Assuming 1 Bq/kg for internal radioactivity
        // or 1 /m2/s for external gamma/neutron
        // Internal:
        //  If there is material/geometry table, use the correct mass.
        //  If not, assume mass is 1 kg.

    map<string, double*> energyDeposit;

    void ProcessFile( TTree* tree, string s);

    void ProcessPulseArray( TTree* tree );

    bool NewEvent( char* name );
        // Returns true if current event is the beginning of a new event

    bool NewEventByTimeReset( char* name );
        // Returns true if current event is due to time reset of radioactive decay

    StepInfo rdata;

    map<string, MCPulseArray> pulseArrayAV;
    //map<string, double> edepAVWrite;

    map<string, MCPulseArray> pulseArrayVOI;
    //map<string, double> edepVOIWrite;

    MCPulse ConvertToMCPulse( StepInfo );

    void FindEventTime( string& s, double& t);
};


#endif
