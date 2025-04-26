/*
    Author:  Suerfu Burkhant
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file StackingAction.cc
/// \brief Implementation of the StackingAction class

#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4StackManager.hh"

#include "G4SystemOfUnits.hh"


StackingAction::StackingAction( RunAction* runAction, EventAction* eventAction) : 
    fRunAction( runAction ),
    fEventAction( eventAction ){ }


StackingAction::~StackingAction(){ }


G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track){

    // By default, ignore or do not track neutrinos
    //
    G4String particleName = track->GetDefinition()->GetParticleName();
    if( particleName=="anti_nu_e" || particleName=="nu_e" ){
        return fKill;
    }

    // If global time is within the window, return urgent regardless of the process:
    //
    G4double window = 1*CLHEP::ms;
        // 1-ms window

    G4double time = track->GetGlobalTime();
    if( time < window ){
        return fUrgent;
    }

    // Check if it's a radioactive decay that happened years later.
    //
    G4String motherProcess = "";
    if( track->GetCreatorProcess()!=0 ){
        motherProcess = track->GetCreatorProcess()->GetProcessName();
    }

    //G4cout << "Name: " << track->GetParticleDefinition()->GetParticleName() << "\tt: " << track->GetGlobalTime() << "\tCreatorProc: " << motherProcess << G4endl;

    if( motherProcess.find("RadioactiveDecay")==std::string::npos ){
        return fUrgent;
    }
    else{
        //G4cout << "Radioactivity out of window detected!" << G4endl;
        const_cast<G4Track*>(track)->SetGlobalTime(0);
        return fWaiting;
    }
}


void StackingAction::NewStage(){
    StepInfo stepinfo;
    stepinfo.SetProcessName( "timeReset" );
    fEventAction->GetStepCollection().push_back(stepinfo);
}
