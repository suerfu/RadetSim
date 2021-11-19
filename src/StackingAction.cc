/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file StackingAction.cc
/// \brief Implementation of the StackingAction class

#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"

#include "G4SystemOfUnits.hh"


StackingAction::StackingAction( RunAction* runAction, EventAction* eventAction) : 
    fRunAction( runAction ),
    fEventAction( eventAction ){ }


StackingAction::~StackingAction(){ }


G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track){

    G4double window = 1.e6*CLHEP::ns;

    G4String particleName = track->GetDefinition()->GetParticleName();

    // By default, ignore or do not track neutrinos
    //
    if( particleName=="anti_nu_e" || particleName=="nu_e" ){
        return fKill;
    }

    G4String motherProcess = "";
    if( track->GetCreatorProcess()!=0 ){
        motherProcess = track->GetCreatorProcess()->GetProcessName();
    }

    if( motherProcess!="RadioactiveDecay" ){
        return fUrgent;
    }
    else{
        const_cast<G4Track*>(track)->SetGlobalTime(0);
        return fWaiting;
    }
}


void StackingAction::NewStage(){
    StepInfo stepinfo;
    stepinfo.processName = "timeReset";
    fEventAction->GetStepCollection().push_back(stepinfo);
}
