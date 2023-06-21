
#include "TrackReader.h"

#include "TTree.h"
#include "TFile.h"
#include "TObjString.h"
//#include "TKey.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>

#include "MacroHandler.h"

// history:
// 2022-05-06 Suerfu adding functions to calculate simulation duration automatically.
//      Note: at this stage if run/beamOn, confine or wall_ appears in comments, it may affect result.
//      Note2: surface mode supports only full surface. It does not support particular surfaces.

using namespace std;


void TrackReader::ConfigureTTree( TTree* tree ){

    // Set global-level information
    //
    stringstream ss;
    ss << "file[" << MAX_FILENAME_LEN << "]/C";
    tree->Branch( "file", &inputFileNameChar, ss.str().c_str() );

    tree->Branch( "ID", &ID, "ID/I");
    tree->Branch( "eventID", &eventID, "eventID/I");
    tree->Branch( "clusterIndex", &clusterIndex, "clusterIndex/I");

    timeStamp = -1;
    tree->Branch( "timeStamp", &timeStamp, "timeStamp/D" );
        // Time stamp is by default -1.

    // Set Parent info if enabled
    //
    if( GetParentInfo()==true ){
        tree->Branch( "parentParticle", &parentParticle, "parentParticle[16]/C" );
        tree->Branch( "parentVolume", &parentVolume, "parentVolume[16]/C" );
        tree->Branch( "parentRx", &parentPosition[0], "parentRx/D" );
        tree->Branch( "parentRy", &parentPosition[1], "parentRy/D" );
        tree->Branch( "parentRz", &parentPosition[2], "parentRz/D" );
        tree->Branch( "parentEki", &parentEki, "parentEki/D" );
    }

    // Generate energy deposition array for active volumes.
    //
    for( vector<string>::iterator itr1 = arrayAV.begin(); itr1!=arrayAV.end(); itr1++ ){

        string volumeName = *itr1;
        cout << "Creating pulse array for Active Volume " << volumeName << endl;

        string edepName = string("edep_") + *itr1;

		stringstream ss_tmp;
		ss_tmp << edepName << "[" << MCPulse::GetNType() << "]/D";
        string rootName = ss_tmp.str();
            // rootName is name + ROOT data format.

        pulseArrayAV[ volumeName ] = MCPulseArray();

		energyDeposit[ volumeName ] = new double[MCPulse::GetNType()];
        for( unsigned int i=0; i<MCPulse::GetNType(); i++){
            // cout << &energyDeposit[ volumeName ][i] << endl;
            energyDeposit[ volumeName ][i] = 0;
        }
        //cout << &energyDeposit[ volumeName ] << endl;
            // in principle this has to be deleted at the end, but I am too busy...
        tree->Branch( edepName.c_str(), energyDeposit[volumeName], rootName.c_str() );
            // Set address for ROOT tree branch

        // If a volume name also exists in volume-of-interest, remove them all.
        //
        vector<string>::iterator tmp = find(arrayVOI.begin(), arrayVOI.end(), *itr1);
        while( tmp!=arrayVOI.end() ){
            arrayVOI.erase( tmp );
            tmp = find(arrayVOI.begin(), arrayVOI.end(), *itr1);
        }
    }

    // Generate energy deposition array for volumes of interest.
    //
    for( vector<string>::iterator itr1 = arrayVOI.begin(); itr1!=arrayVOI.end(); itr1++ ){

        string volumeName = *itr1;
        cout << "Creating pulse array for VOI " << volumeName << endl;

        string edepName = string("edep_") + *itr1;

		stringstream ss_tmp;
		ss_tmp << edepName << "[" << MCPulse::GetNType() << "]/D";
        string rootName = ss_tmp.str();
            // rootName is name + ROOT data format.

        pulseArrayVOI[ volumeName ] = MCPulseArray();

		energyDeposit[ volumeName ] = new double[MCPulse::GetNType()];
        tree->Branch( edepName.c_str(), energyDeposit[volumeName], rootName.c_str() );
            // Set address for ROOT tree branch
    }
}


bool TrackReader::NewEvent( char* name ){
    if (strncmp( name, "newEvent", 8)==0 )
        return true;
    else if( strncmp( name, "timeReset", 9)==0 )
        return true;
    return false;
}



void TrackReader::Process( string outputStr, vector<string> inputStr ){

    // ==================================================
    // Prepare VOI and PSD for writing trees
    // ==================================================
    //
    // If no volume-of-interest is specified, iterate over the file to get all the volumes involved.
    if( arrayVOI.empty()==true ){
        cout << "Volume-of-Interest is empty. All volumes treated as VOI.\n";
        arrayVOI = GetVOIFromFile( inputStr );
    }

    // ==================================================
    // Create ROOT file
    // ==================================================
    //
    TFile* outputFile = new TFile( outputStr.c_str(), "NEW" );

    TMacro gTab;
        // Keep one copy of geometry table in the output.
        // By default, it will be the first occurance.
        // By default, the geometry is assumed to be identical in all the runs in the same batch.

    if( !outputFile ){
        cerr << "Error creating output file " << outputStr << endl;
        return;
    }

    // ==================================================
    // Create ROOT TTree and initialize branches.
    // ==================================================
    //
    TTree* tree = new TTree("events", "Event-level information for MC simulated tracks.");
    ConfigureTTree( tree );
        // Set the addresses of global-level variables.

    ID = 0;

    //Nsimulated = 0;
    Tsimulated = 0;

    for( vector<string>::iterator itr = inputStr.begin(); itr!=inputStr.end(); itr++ ){

        ProcessFile( tree, *itr );

        TMacro mac1 = GetMacro( *itr, "runMacro" );
        TMacro mac2 = GetMacro( *itr, "geometryTable" );
            // feature since ver. 0.1.1

        if( itr==inputStr.begin() ){
            gTab = mac2;
        }

        Tsimulated += GetTimeSimulated( mac1, mac2 );
    }

    outputFile->cd();

    tree->Write();
    gTab.Write();

    stringstream ss;
    ss << Tsimulated;

    TMacro mac("duration");
    mac.AddLine( ss.str().c_str() );
    mac.Write();

    outputFile->Write();
    outputFile->Close();

    cout << "Output file " << outputStr << " written.\n";

}



void TrackReader::ProcessFile( TTree* tree, string input ){


    // ==================================================
    // Open the ROOT file to read and set branch variables
    // ==================================================
    //
    TFile* inputFile = TFile::Open( input.c_str(), "READ");
    if( !inputFile ){
        cerr << "ERROR reading file " << input << ". Skipping..." << endl;
        return;
    }

    cout << "Processing " << input << endl;

    strncpy( inputFileNameChar, input.c_str(), MAX_FILENAME_LEN-1 );

    // ==================================================
    // Retrieve the tree and set branch address of variables for readout
    // ==================================================
    //
    TTree* inputTree = (TTree*)inputFile->Get("events");

    StepInfo rdata;

    inputTree -> SetBranchAddress( "eventID",  &rdata.eventID);
    inputTree -> SetBranchAddress( "trackID",  &rdata.trackID);
    inputTree -> SetBranchAddress( "parentID", &rdata.parentID);
    inputTree -> SetBranchAddress( "particle", &rdata.particleName);
    inputTree -> SetBranchAddress( "volume",   &rdata.volumeName);
    inputTree -> SetBranchAddress( "rx",       &rdata.position[0]);
    inputTree -> SetBranchAddress( "ry",       &rdata.position[1]);
    inputTree -> SetBranchAddress( "rz",       &rdata.position[2]);
    inputTree -> SetBranchAddress( "Eki",      &rdata.Eki);
    inputTree -> SetBranchAddress( "Ekf",      &rdata.Ekf);
    inputTree -> SetBranchAddress( "Edep",     &rdata.Edep);
    inputTree -> SetBranchAddress( "t",        &rdata.time);
    inputTree -> SetBranchAddress( "process",  &rdata.processName);

    // ==================================================
    // Loop over the tree and process the events.
    // ==================================================
    //
    unsigned int nEntries = inputTree->GetEntries();

    // Here initial index is starting with 1 since in any valid file, first entry is newEvent.
    // If file is empty, the for-loop condition is not satisfied, thus won't be executed.
    //
    for( unsigned int n = 1; n<nEntries; n++ ){

        inputTree->GetEntry(n);

        // If current event is a new event or the last event, fill the previous event and initialize.
        //
        if( NewEvent( rdata.processName )==true || n==nEntries-1 ){

            //cout << "Processing event " << eventID << " at entry " << n <<endl;
            ProcessPulseArray( tree );
                // filling is done at this step.

            map<string, MCPulseArray>::iterator clr;
            for( clr=pulseArrayAV.begin(); clr!=pulseArrayAV.end(); clr++ ){
                (clr->second).Clear();
            }
            for( clr=pulseArrayVOI.begin(); clr!=pulseArrayVOI.end(); clr++ ){
                (clr->second).Clear();
            }
        }

        // If not new event flag, then it's a regular step.
        // Check if it falls in one of the active volumes.
        // If yes, then put it in the energy deposit array to be processed later.
        //
        else{

            eventID = rdata.eventID;

            // Suerfu on June 20, 2023: set parent information if not set yet.
            //
            if( parentID < 0 ){
                parentID = rdata.trackID;
                //parentParticle = rdata.particleName;
                //parentVolume = rdata.volumeName;
                parentPosition[0] = rdata.position[0];
                parentPosition[1] = rdata.position[1];
                parentPosition[2] = rdata.position[2];
                parentEki = rdata.Eki;
            }

            if( rdata.Edep>1e-9 ){

                string name = string( rdata.volumeName );

                if( find( arrayAV.begin(), arrayAV.end(), name ) != arrayAV.end() ){
                    pulseArrayAV[name].PushBack( ConvertToMCPulse(rdata ) );
                }

                else if( find( arrayVOI.begin(), arrayVOI.end(), name ) != arrayVOI.end() ){
                    pulseArrayVOI[name].PushBack( ConvertToMCPulse( rdata ) );
                }
            }
        }
    }
}



void TrackReader::ProcessPulseArray( TTree* tree ){

    clusterIndex = 0;

    // Sort the hits in active volumes and voi by time order.
    //
    map<string, MCPulseArray>::iterator itr;
    for( itr=pulseArrayAV.begin(); itr!=pulseArrayAV.end(); itr++ ){
        (itr->second).Sort();
    }
    for( itr=pulseArrayVOI.begin(); itr!=pulseArrayVOI.end(); itr++ ){
        (itr->second).Sort();
    }

    // Find the first interaction event.
    // If the interaction time is positive, then enter the loop.
    // This variable is used as a state variable:
    //      if -1 is passed, it will always return the first interaction time.
    // Suerfu on 2022-5-18: using -1 as default will create problems when passing it to the FindEventTime
    // function when two active volumes are involved.
    //
    double eventTime = -1;
    string volName;
    FindEventTime( volName, eventTime );

    while( eventTime > 0 ){

        // Now eventTime is the time of interaction.
        // volName is the name of the volume where this event happened.
        // Next, find coincidence.
        //
        for( itr=pulseArrayAV.begin(); itr!=pulseArrayAV.end(); itr++){
            if( itr->first==volName ){
                MCPulse p = (itr->second).FindPrimaryInteraction( eventTime, daqWindow );
				for( unsigned int i=0; i<MCPulse::GetNType(); i++){
                	//cout << energyDeposit[ itr->first ] << endl;;// = p.GetEnergy( i );
                    energyDeposit[ itr->first ][i] = p.GetEnergy( i );
                	//cout << "Primary " << itr->first << " " << energyDeposit[itr->first][i] << endl;
				}
            }
            else{
                MCPulse p = (itr->second).FindCoincidentInteraction( eventTime, coinWindow, daqWindow );
				for( unsigned int i=0; i<MCPulse::GetNType(); i++){
                	energyDeposit[ itr->first ][i] = p.GetEnergy( i );
                	//cout << "Act Coin " << itr->first << " " << energyDeposit[itr->first][i] << endl;
            	}
			}
        }

        for( itr=pulseArrayVOI.begin(); itr!=pulseArrayVOI.end(); itr++){
            MCPulse p = (itr->second).FindCoincidentInteraction( eventTime, coinWindow, daqWindow );
			for( unsigned int i=0; i<MCPulse::GetNType(); i++){
            	energyDeposit[ itr->first ][i] = p.GetEnergy( i );
            	//cout << "N/A Coin " << itr->first << " " << energyDeposit[itr->first][i] << endl;
        	}
		}

        timeStamp = eventTime;
        tree->Fill();
        ID++;
            // Since the tree may have to be filled multiple times, it has to be done in this function.

        DefaultBranchVariables();
        clusterIndex++;

        FindEventTime( volName, eventTime );
    }
}



MCPulse TrackReader::ConvertToMCPulse( StepInfo step ){

    MCPulse val;

	int index = MCPulse::GetEdepIndex( step.particleName );
	val.SetEnergy( index, step.Edep );
    val.SetTime( step.time );

	return val;
}




void TrackReader::FindEventTime( string& volName, double& eventTime ){

    map<string, MCPulseArray>::iterator itr;

    for( itr=pulseArrayAV.begin(); itr!=pulseArrayAV.end(); itr++){

        // Needed to properly initialize when there are multiple active volumes.
        // in which case GetNextInteractionTime could return -1.
        //
        if( eventTime < 0 || itr==pulseArrayAV.begin() ){
            volName = itr->first;
            eventTime = (itr->second).GetNextInteractionTime();
        }
        else{
            double intTime = (itr->second).GetNextInteractionTime();

            // One should check if interaction time is greater than -1
            //
            if( eventTime > intTime && intTime > 0 ){
                volName = itr->first;
                eventTime = (itr->second).GetNextInteractionTime();
            }
        }
    }
    //cout << "\nEvent time is " << eventTime << " in " << volName << endl;
}



vector<string> TrackReader::GetVOIFromFile( vector<string> inputName){

    //cout << "Reading over the input files to check all volumes involved..." << endl;

    set<string> container;
    vector<string> return_val;

    for( vector<string>::iterator itr = inputName.begin(); itr!=inputName.end(); itr++ ){

        cout << "Checking " << *itr << endl;

        // Open the ROOT file to read and set branch variables
        //
        TFile* inputFile = TFile::Open( itr->c_str(), "READ");
        if( !inputFile ){
            cerr << "ERROR reading file " << *itr << endl;
            cerr << "Skipping...\n";
            continue;
        }

        // Retrieve the tree and set branch address of variables for readout
        //
        TTree* inputTree = (TTree*)inputFile->Get("events");

        StepInfo rdata;
        inputTree -> SetBranchAddress( "volume",   &rdata.volumeName);

        // Loop over the tree and process the events.
        //
        unsigned int nEntries = inputTree->GetEntries();

        for( unsigned int n = 1; n<nEntries; n++ ){

            inputTree->GetEntry(n);

            string name = rdata.volumeName;
            if( name!="" ){
                container.insert( rdata.volumeName);
            }
        }
        inputFile->Close();
    }

    cout << "Adding ";
    for( auto itr = container.begin(); itr!=container.end(); itr++ ){
        return_val.push_back( *itr );
        cout << *itr << ' ';
    }

    cout << "as volumes of interest." << endl;

    return return_val;
}


double TrackReader::GetTimeSimulated( TMacro runMacro, TMacro geoMacro ){

    double NbParticle = GetNbParticleSimulated( runMacro );
    if( NbParticle<0 ){
        return -1;
    }
    cout << NbParticle << " particles simulated" << endl;

    // First check if this is a surface type simulation or bulk type simulation.
    string keyword1( "/generator/wall" );
    string keyword2( "/confine" );
    string keyword3( "/generator/setMaterial" );

    if( runMacro.GetLineWith( keyword1.c_str() ) != 0 ){

        string target( runMacro.GetLineWith( keyword1.c_str() )->String().Data() );

        string foo;
        double xLen, yLen, zLen;
        string xUnit, yUnit, zUnit;

        stringstream ss(  runMacro.GetLineWith( (keyword1+"_x").c_str() )->String().Data() );
        ss >> foo >> xLen >> xUnit;
        if( xUnit == "cm" ){
            xLen *= 0.01;
        }

        ss.str(  runMacro.GetLineWith( (keyword1+"_y").c_str() )->String().Data() );
        ss.clear();
        ss >> foo >> yLen >> yUnit;
        if( yUnit == "cm" ){
            yLen *= 0.01;
        }

        ss.str(  runMacro.GetLineWith( (keyword1+"_z").c_str() )->String().Data() );
        ss.clear();
        ss >> foo >> zLen >> zUnit;
        if( zUnit == "cm" ){
            zLen *= 0.01;
        }

        double surfArea = 2 * ( xLen*yLen + xLen*zLen + yLen*zLen );
        cout << "Total surface area is " << surfArea << " m2" << endl;

        return ( NbParticle / surfArea );
            // Assuming flux is 1 /m2/s, calculate Tsim
    }
    // This simulation is for bulk radioactivity
    // But one needs to check if there is confine because it may also be a simple monoenergetic beam with point source.
    else if( runMacro.GetLineWith( keyword2.c_str() ) != 0 ){

        string foo;
        string vol;
        double mass = -1;

        stringstream ss(  runMacro.GetLineWith( keyword2.c_str() )->String().Data() );
        ss >> foo >> vol;
        cout << "Source-confining volume is " << vol << endl;

        if( geoMacro.GetLineWith( vol.c_str() ) == 0 ){
            cout << "Warning: confine volume " << vol << " not found in geometry table. Using 1 kg." << endl;
            mass = 1;
        }
        else{
            ss.str(  geoMacro.GetLineWith( vol.c_str() )->String().Data() );
            ss.clear();
            ss >> foo >> mass;
            cout << "Mass of source volume is " << mass << " kg" << endl;
        }
        return ( NbParticle ) / mass;
    }
    // Bulk radioactivity by total material.
    //
    else if( runMacro.GetLineWith( keyword3.c_str() ) != 0 ){

        string foo;
        string mat_name;
        double mass = -1;

        stringstream ss(  runMacro.GetLineWith( keyword3.c_str() )->String().Data() );
        ss >> foo >> mat_name;

        mass = GetMassByMaterial( geoMacro, mat_name );
        cout << "Source-confining material is " << mat_name << " " << mass << " kg" << endl;

        return ( NbParticle ) / mass;
    }
    else{
        return -1;
    }
}


