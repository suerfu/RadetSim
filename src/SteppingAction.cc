/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "GeometryConstruction.hh"

#include "G4Neutron.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "StepInfo.hh"


SteppingAction::SteppingAction( RunAction* runAction, EventAction* eventAction, const GeometryConstruction* detectorConstruction ) : G4UserSteppingAction(),
        fRunAction( runAction ),
        fEventAction( eventAction ),
        fDetectorConstruction( detectorConstruction ){
}


SteppingAction::~SteppingAction(){ }


void SteppingAction::UserSteppingAction( const G4Step* step){
    
    /*
    G4Track* track = step->GetTrack();
    G4int parentID = track->GetParentID();
    G4int stepNo =  track->GetCurrentStepNumber();
    
    G4StepPoint* preStep = step->GetPreStepPoint();
    G4StepPoint* postStep = step->GetPostStepPoint();

    G4String particle = track->GetParticleDefinition()->GetParticleName();
    G4String particleType = track->GetParticleDefinition()->GetParticleType();
    */

    G4String procName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    // Collect energy and number of scatters step by step
    // Don't save the out of world step
    // if(!postStep->GetPhysicalVolume()) return;

    if( procName=="Transportation" && step->GetTotalEnergyDeposit()<1.e-6*CLHEP::eV )
        return;

    /*
    // Record only EM interactions.
    if( particle=="gamma" || particle=="e+" || particle=="e-"){
        fEventAction->GetStepCollection().push_back(StepInfo(step));
    }
    */
    
    fEventAction->GetStepCollection().push_back(StepInfo(step));

}

