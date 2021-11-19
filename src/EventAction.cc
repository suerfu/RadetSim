/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "RunAction.hh"

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
    eventID = 0;
    stepID = 0;
    trackID = 0;
    parentID = 0;

    tmp_particleName = "";
    tmp_volumeName = "";
    tmp_processName = "";

    position = G4ThreeVector(0,0,0);
    momentum = G4ThreeVector(0,0,0);
    kineticInfo = KineticInfo();

    max_char_len = 15;
    cmdl = fRunAction->GetCommandlineArguments();
}


EventAction::~EventAction(){}


void EventAction::PrintEventStatistics() const {}


void EventAction::BeginOfEventAction(const G4Event*){
    
    // If pointer to ROOT tree is empty, then ask RunAction to create the ROOT tree
    // and assign address of variables for output.
    if( data_tree==0 ){

        data_tree = fRunAction->GetDataTree();

        // Proceed only if data output is enabled.
        if( data_tree!=0 ){
            // information about its order in the event/run sequence
            data_tree->Branch("eventID", &eventID, "eventID/I");
            data_tree->Branch("trackID", &trackID, "trackID/I");

            // information about its idenity
            data_tree->Branch("particle", particleName, "particle[16]/C");
            data_tree->Branch("parentID", &parentID, "parentID/I");
            data_tree->Branch("stepID", &stepID, "stepID/I");

            // geometric information
            data_tree->Branch("volume", volumeName, "volume[16]/C");
            data_tree->Branch("rx", &kineticInfo.rx, "rx/D");
            data_tree->Branch("ry", &kineticInfo.ry, "ry/D");
            data_tree->Branch("rz", &kineticInfo.rz, "rz/D");
            data_tree->Branch("px", &kineticInfo.px, "px/D");
            data_tree->Branch("py", &kineticInfo.py, "py/D");
            data_tree->Branch("pz", &kineticInfo.pz, "pz/D");

            // dynamic information
            data_tree->Branch("t", &kineticInfo.time, "t/D");
            data_tree->Branch("Eki", &kineticInfo.Eki, "Eki/D"); // initial kinetic energy before the step
            data_tree->Branch("Ekf", &kineticInfo.Ekf, "Ekf/D"); // final kinetic energy after the step
            data_tree->Branch("Edep", &kineticInfo.Edep, "Edep/D"); // energy deposit calculated by Geant4
            data_tree->Branch("process", processName, "process[16]/C");
        }
    }

    //At the beginning of the event, insert a special flag.
    StepInfo stepinfo;
    stepinfo.SetProcessName( "newEvent" );
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
        bool record = true;
        /*
        // Some filter criteria goes here.
        for( size_t i=0; i < stepCollection.size(); ++i ){
            if( stepCollection[i].GetVolumeName()=="chamber" ){
                record = true;
                break;
            }
        }
        */

        if( record==true ){

            bool NewEvtMarker = false;
            G4String EventType = "";

            for( size_t i=0; i < stepCollection.size(); ++i ){
            
                tmp_processName = stepCollection[i].GetProcessName();
                if( tmp_processName=="newEvent" || tmp_processName=="timeReset"){
                    EventType = tmp_processName;
                    NewEvtMarker = true;
                    continue;
                }
                if( NewEvtMarker==true ){
                    tmp_processName = EventType;
                    NewEvtMarker = false;
                }

                tmp_particleName = stepCollection[i].GetParticleName();
                tmp_volumeName = stepCollection[i].GetVolumeName();

                eventID = stepCollection[i].GetEventID();
                trackID = stepCollection[i].GetTrackID();
                parentID = stepCollection[i].GetParentID();
                stepID = stepCollection[i].GetStepID();

                strncpy( particleName, tmp_particleName.c_str(), max_char_len);
                strncpy( processName, tmp_processName.c_str(), max_char_len);
                strncpy( volumeName, tmp_volumeName.c_str(), max_char_len);

                kineticInfo.Eki = stepCollection[i].GetEki()/keV;
                kineticInfo.Ekf = stepCollection[i].GetEkf()/keV;
                kineticInfo.Edep = stepCollection[i].GetDepositedEnergy()/keV;
            
                position = stepCollection[i].GetPosition();
                kineticInfo.rx = position.x()/mm;
                kineticInfo.ry = position.y()/mm;
                kineticInfo.rz = position.z()/mm;

                momentum = stepCollection[i].GetMomentumDirection();
                kineticInfo.px = momentum.x();
                kineticInfo.py = momentum.y();
                kineticInfo.pz = momentum.z();

                kineticInfo.time = stepCollection[i].GetGlobalTime()/ns;

                data_tree->Fill();
            }
        }
    }

    stepCollection.clear();
}



vector<StepInfo>& EventAction::GetStepCollection(){
    return stepCollection;
}

