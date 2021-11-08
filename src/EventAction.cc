
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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction( RunAction* input_run_action )
 : G4UserEventAction(),
   run_action(input_run_action),
   stepCollection(),
   data_tree(0),
   eventID(0),
   trackID(0),
   stepID(0),
   parentID(0),
   tmp_particle_name(""),
   tmp_volume_name(""),
   tmp_process_name(""),
   position(0),
   x(0),
   y(0),
   z(0),
   momentum(0),
   px(0),
   py(0),
   pz(0),
   Eki(0),
   Ekf(0),
   edep(0),
   global_time(0)
{
    max_char_len = 15;
    cmdl = run_action->GetCommandlineArguments();
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


EventAction::~EventAction(){}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void EventAction::PrintEventStatistics() const {}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void EventAction::BeginOfEventAction(const G4Event*){
    
    // If pointer to ROOT tree is empty, then ask RunAction to create the ROOT tree
    // and assign address of variables for output.
    if( data_tree==0 ){

        data_tree = run_action->GetDataTree();

        // Proceed only if data output is enabled.
        if( data_tree!=0 ){
            // information about its order in the event/run sequence
            data_tree->Branch("eventID", &eventID, "eventID/I");
            data_tree->Branch("trackID", &trackID, "trackID/I");
            data_tree->Branch("stepID", &stepID, "stepID/I");

            // information about its idenity
            data_tree->Branch("particle", particle_name, "particle[16]/C");
            data_tree->Branch("parentID", &parentID, "parentID/I");

            // geometric information
            data_tree->Branch("volume", volume_name, "volume[16]/C");
            data_tree->Branch("x", &x, "x/D");
            data_tree->Branch("y", &y, "y/D");
            data_tree->Branch("z", &z, "z/D");
            data_tree->Branch("px", &px, "px/D");
            data_tree->Branch("py", &py, "py/D");
            data_tree->Branch("pz", &pz, "pz/D");

            // dynamic information
            data_tree->Branch("t", &global_time, "t/D");
            data_tree->Branch("Eki", &Eki, "Eki/D"); // initial kinetic energy before the step
            data_tree->Branch("Ekf", &Ekf, "Ekf/D"); // final kinetic energy after the step
            data_tree->Branch("Edep", &edep, "Edep/D"); // energy deposit calculated by Geant4
            data_tree->Branch("process", process_name, "process[16]/C");
        }
    }

    StepInfo stepinfo;
    stepinfo.SetProcessName( "newEvent" );
    GetStepCollection().push_back(stepinfo);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
        for( size_t i=0; i < stepCollection.size(); ++i ){
            if( stepCollection[i].GetVolumeName()=="chamber" ){
                record = true;
                break;
            }
        }

        if( record==true ){

            bool NewEvtMarker = false;
            G4String EventType = "";

            for( size_t i=0; i < stepCollection.size(); ++i ){
            
                tmp_process_name = stepCollection[i].GetProcessName();
                if( tmp_process_name=="newEvent" || tmp_process_name=="timeReset"){
                    EventType = tmp_process_name;
                    NewEvtMarker = true;
                    continue;
                }
                if( NewEvtMarker==true ){
                    tmp_process_name = EventType;
                    NewEvtMarker = false;
                }

                tmp_particle_name = stepCollection[i].GetParticleName();
                tmp_volume_name = stepCollection[i].GetVolumeName();

                eventID = stepCollection[i].GetEventID();
                trackID = stepCollection[i].GetTrackID();
                stepID = stepCollection[i].GetStepID();
                parentID = stepCollection[i].GetParentID();

                strncpy( particle_name, tmp_particle_name.c_str(), max_char_len);
                strncpy( process_name, tmp_process_name.c_str(), max_char_len);
                strncpy( volume_name, tmp_volume_name.c_str(), max_char_len);

                Eki = stepCollection[i].GetEki();
                Ekf = stepCollection[i].GetEkf();
                edep = stepCollection[i].GetDepositedEnergy();
            
                position = stepCollection[i].GetPosition();
                x = position.x();
                y = position.y();
                z = position.z();

                momentum = stepCollection[i].GetMomentumDirection();
                px = momentum.x();
                py = momentum.y();
                pz = momentum.z();

                global_time = stepCollection[i].GetGlobalTime();

                data_tree->Fill();
            }
        }
    }

    stepCollection.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

vector<StepInfo>& EventAction::GetStepCollection(){
    return stepCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
