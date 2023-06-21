/*
    Author:  Suerfu Burkhant
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EVENTACTION_H
#define EVENTACTION_H 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "StepInfo.hh"
#include "RunAction.hh"


/// EventAction is responsible for processing the events.
/// The process mainly includes iterating over the tracks/steps and write them into a ROOT file.

class EventAction : public G4UserEventAction{

public:

    EventAction( RunAction* runaction );
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void PrintEventStatistics() const;
    
    vector<StepInfo>& GetStepCollection();
        //!< A vector that contains each steps in this event.

private:
    
    RunAction* fRunAction;
        //!< Pointer to RunAction to get output filename, etc.

    CommandlineArguments* cmdl;
        //!< Pointer to commandline arguments so that EventAction can access commandline parameters.

    vector<StepInfo> stepCollection;
    
    TTree* data_tree;
        //!< Pointer to a ROOT TTree object.

    static const int max_char_len1 = 10;
        //!< Max character length to store shorter ones such as particle name.

    static const int max_char_len2 = 20;
        //!< Maximum character length to store in ROOT. Using char[] insteat of string will speed up things.
        //!< Longer version for volume names & process names.

    char particleName[max_char_len1];
    char volumeName[max_char_len2];
    char nextVolumeName[max_char_len2];
    char processName[max_char_len2];

    G4String tmp_particleName;
    G4String tmp_volumeName;
    G4String tmp_nextVolumeName;
    G4String tmp_processName;
    
    int eventID;
    int trackID;
    int parentID;
    int stepID;

    double rx, ry, rz;
    double px, py, pz;
    double Eki, Ekf, Edep;
    double globalTime;

    void SetFillValue( StepInfo& wStep){

        eventID = wStep.GetEventID();
        trackID = wStep.GetTrackID();
        parentID = wStep.GetParentID();
        stepID = wStep.GetStepID();

        tmp_particleName = wStep.GetParticleName();
        tmp_volumeName = wStep.GetVolumeName();
        tmp_nextVolumeName = wStep.GetNextVolumeName();
        tmp_processName = wStep.GetProcessName();

        strncpy( particleName, tmp_particleName.c_str(), max_char_len1);
        strncpy( processName, tmp_processName.c_str(), max_char_len2);
        strncpy( volumeName, tmp_volumeName.c_str(), max_char_len2);
        strncpy( nextVolumeName, tmp_nextVolumeName.c_str(), max_char_len2);
        
        rx = wStep.GetPosition().x()/CLHEP::mm;
        ry = wStep.GetPosition().y()/CLHEP::mm;
        rz = wStep.GetPosition().z()/CLHEP::mm;

        px = wStep.GetMomentumDir().x();
        py = wStep.GetMomentumDir().y();
        pz = wStep.GetMomentumDir().z();

        Eki = wStep.GetEki()/CLHEP::keV;
        Ekf = wStep.GetEkf()/CLHEP::keV;
        Edep = wStep.GetEdep()/CLHEP::keV;

        globalTime = wStep.GetGlobalTime()/CLHEP::ns;
    }
};


#endif
