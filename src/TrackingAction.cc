/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file /src/TrackingAction.cc
/// \brief Implementation of the TrackingAction class
//
//

#include "TrackingAction.hh"
#include "EventAction.hh"

#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include "StepInfo.hh"


TrackingAction::TrackingAction(RunAction* runAction, EventAction* eventAction)
  : G4UserTrackingAction(),
    fRunAction( runAction ),
    fEventAction( eventAction ){
}


void TrackingAction::PreUserTrackingAction(const G4Track* track){
    
    G4String procName = "initStep";

    // Set up the initStep by hand
    //
    StepInfo stepInfo;


    // First check if the particle should be excluded.
    //
    stepInfo.particleName = track->GetParticleDefinition()->GetParticleName();
    if( fRunAction->ExcludeParticle( stepInfo.particleName) ){
        return;
    }
    
    // Next check for exclude volume
    //
    stepInfo.volumeName = track->GetVolume()->GetName();
    if( fRunAction->ExcludeVolume( stepInfo.volumeName) ){
        return;
    }

    stepInfo.eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    stepInfo.trackID = track->GetTrackID();
    stepInfo.stepID =  track->GetCurrentStepNumber();
    stepInfo.parentID = track->GetParentID();

    stepInfo.volumeCopyNumber = track->GetVolume()->GetCopyNo();

    stepInfo.position = track->GetPosition();
    stepInfo.momentumDir = track->GetMomentumDirection();
    stepInfo.globalTime = track->GetGlobalTime();

    stepInfo.Eki = track->GetKineticEnergy();
    stepInfo.Ekf = track->GetKineticEnergy();

    stepInfo.processName = procName;

    fEventAction->GetStepCollection().push_back(stepInfo);
}

