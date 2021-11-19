/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include "StepInfo.hh"
#include "G4ThreeVector.hh"

#include "TTree.h"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


EventAction::EventAction( RunAction* runaction ) : G4UserEventAction(), fRunAction(runaction) {

    data_tree = 0;

    wStep = StepInfo();

    tmp_particleName = "";
    tmp_volumeName = "";
    tmp_processName = "";

    max_char_len = 15;
    cmdl = fRunAction->GetCommandlineArguments();
}


EventAction::~EventAction(){}


void EventAction::PrintEventStatistics() const {}


void EventAction::BeginOfEventAction(const G4Event*){
    
    // If pointer to ROOT tree is empty, then ask RunAction to create the ROOT tree
    // and assign address of variables for output.
    // 
    if( data_tree==0 ){

        data_tree = fRunAction->GetDataTree();

        // Proceed only if data output is enabled.
        if( data_tree!=0 ){

            // information about its order in the event/run sequence
            //
            data_tree->Branch("eventID", &wStep.eventID, "eventID/I");
            data_tree->Branch("trackID", &wStep.trackID, "trackID/I");

            // information about its idenity
            //
            data_tree->Branch("particle", particleName, "particle[16]/C");
                // Strings must be handled indirectly since it should not be filled directly.
                // If done directly, it becomes a vector of variable length and subsequent reading speed becomes very low.
            data_tree->Branch("parentID", &wStep.parentID, "parentID/I");
            data_tree->Branch("stepID", &wStep.stepID, "stepID/I");

            // geometric information
            //
            data_tree->Branch("volume", volumeName, "volume[16]/C");
            data_tree->Branch("rx", &wStep.rx, "rx/D");
            data_tree->Branch("ry", &wStep.ry, "ry/D");
            data_tree->Branch("rz", &wStep.rz, "rz/D");
            data_tree->Branch("px", &wStep.px, "px/D");
            data_tree->Branch("py", &wStep.py, "py/D");
            data_tree->Branch("pz", &wStep.pz, "pz/D");

            // dynamic information
            //
            data_tree->Branch("t", &wStep.globalTime, "t/D");
            data_tree->Branch("Eki", &wStep.Eki, "Eki/D"); // initial kinetic energy before the step
            data_tree->Branch("Ekf", &wStep.Ekf, "Ekf/D"); // final kinetic energy after the step
            data_tree->Branch("Edep", &wStep.Edep, "Edep/D"); // energy deposit calculated by Geant4

            data_tree->Branch("process", processName, "process[16]/C");
        }
    }

    //At the beginning of the event, insert a special flag.
    StepInfo stepinfo;
    stepinfo.processName = "newEvent";
    GetStepCollection().push_back(stepinfo);
}



void EventAction::EndOfEventAction(const G4Event* event){

    // Print per event (modulo n)
    G4int evtID = event->GetEventID();
    G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( ( printModulo > 0 ) && ( evtID % printModulo == 0 ) ) {
        G4cout << "---> End of event: " << evtID << G4endl;
    }

    if( data_tree!=0 ){

        // Filter for event recording. 
        // For different application, this should be changed.
        bool record = false;
        
        // Some filter criteria goes here.
        for( size_t i=0; i < stepCollection.size(); ++i ){
            if( fRunAction->RecordWhenHit( stepCollection[i].volumeName ) ){
                record = true;
                break;
            }
        }
        

        if( record==true ){

            for( size_t i=0; i < stepCollection.size(); ++i ){
                
                // If process name is newEvent or timeReset, it means the end of previous event and one should write all the output.
                //
                wStep = stepCollection[i];

                tmp_particleName = stepCollection[i].particleName;
                tmp_volumeName = stepCollection[i].volumeName;
                tmp_processName = stepCollection[i].processName;

                strncpy( particleName, tmp_particleName.c_str(), max_char_len);
                strncpy( processName, tmp_processName.c_str(), max_char_len);
                strncpy( volumeName, tmp_volumeName.c_str(), max_char_len);

                data_tree->Fill();
            }
        }
    }

    stepCollection.clear();
}



vector<StepInfo>& EventAction::GetStepCollection(){
    return stepCollection;
}

