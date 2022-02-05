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
    stepInfo.SetParticleName (track->GetParticleDefinition()->GetParticleName() );
    if( fRunAction->ExcludeParticle( stepInfo.GetParticleName() ) ){
        return;
    }
    
    // Next check for exclude volume
    //
    stepInfo.SetVolumeName( track->GetVolume()->GetName() );
    if( fRunAction->ExcludeVolume( stepInfo.GetVolumeName() ) ){
        return;
    }

    stepInfo.SetEventID( G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() );
    stepInfo.SetTrackID( track->GetTrackID() );
    stepInfo.SetStepID( track->GetCurrentStepNumber() );
    stepInfo.SetParentID( track->GetParentID() );

    stepInfo.SetVolumeName( track->GetVolume()->GetName() );
    stepInfo.SetNextVolumeName( track->GetVolume()->GetName() );
    stepInfo.SetVolumeCopyNumber( track->GetVolume()->GetCopyNo() );

    stepInfo.SetPosition( track->GetPosition() );
    stepInfo.SetMomentumDir( track->GetMomentumDirection() );
    stepInfo.SetGlobalTime( track->GetGlobalTime() );

    stepInfo.SetEki( track->GetKineticEnergy() );
    stepInfo.SetEkf( track->GetKineticEnergy() );

    stepInfo.SetProcessName( procName );

    fEventAction->GetStepCollection().push_back(stepInfo);
}

