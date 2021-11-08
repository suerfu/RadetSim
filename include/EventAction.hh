
// $Id: EventAction.hh $
//
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "StepInfo.hh"
#include "RunAction.hh"

class EventAction : public G4UserEventAction{

public:

    EventAction( RunAction* input_run_action );
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void PrintEventStatistics() const;
    
    vector<StepInfo>& GetStepCollection();

private:
    
    RunAction* run_action;

    CommandlineArguments* cmdl;

    vector<StepInfo> stepCollection;
    
    TTree* data_tree;

    int eventID;
    int trackID;
    int stepID;
    int parentID;

    int max_char_len;
    char particle_name[16];
    char volume_name[16];
    char process_name[16];

    G4String tmp_particle_name;
    G4String tmp_volume_name;
    G4String tmp_process_name;
    
    G4ThreeVector position;
    double x;
    double y;
    double z;
    double r;
    double rphi;

    G4ThreeVector momentum;
    double px;
    double py;
    double pz;
    double theta;
    double phi;

    double Eki;
    double Ekf;
    double edep;
    
    double global_time;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
