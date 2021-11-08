
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

struct KineticInfo{

    double rx;
    double ry;
    double rz;

    double px;
    double py;
    double pz;

    double Eki;
    double Ekf;
    double Edep;
    
    double time;
};

class EventAction : public G4UserEventAction{

public:

    EventAction( RunAction* runaction );
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void PrintEventStatistics() const;
    
    vector<StepInfo>& GetStepCollection();

private:
    
    RunAction* fRunAction;

    CommandlineArguments* cmdl;

    vector<StepInfo> stepCollection;
    
    TTree* data_tree;

    int eventID;
    int trackID;
    int parentID;
    int stepID;

    int max_char_len;
    char particleName[16];
    char volumeName[16];
    char processName[16];

    G4String tmp_particleName;
    G4String tmp_volumeName;
    G4String tmp_processName;
    
    G4ThreeVector position;
    G4ThreeVector momentum;

    KineticInfo kineticInfo;
};


#endif
