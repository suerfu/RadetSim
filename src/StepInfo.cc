/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

#include "StepInfo.hh"

#include "globals.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

using namespace std;

using CLHEP::mm;
using CLHEP::ns;
using CLHEP::keV;

StepInfo::StepInfo()
  : eventID(-1),
    trackID(-1),
    stepID(-1),
    parentID(-1),
    particleName(""),
    volumeName(""),
    volumeCopyNumber(0),
    Eki(0),
    Ekf(0),
    Edep(0),
    position(0),
    rx(0),
    ry(0),
    rz(0),
    momentumDir(0),
    px(0),
    py(0),
    pz(0),
    globalTime(0),
    processName("")
{}


StepInfo::StepInfo( const G4Step* step )
  : eventID(-1),
    trackID(-1),
    stepID(-1),
    parentID(-1),
    particleName(""),
    volumeName(""),
    volumeCopyNumber(0),
    Eki(0),
    Ekf(0),
    Edep(0),
    position(0),
    rx(0),
    ry(0),
    rz(0),
    momentumDir(0),
    px(0),
    py(0),
    pz(0),
    globalTime(0),
    processName("")
{

    // From the input step, get necessary pointers to steps and tracks.
    //
    G4StepPoint* postStep = step->GetPostStepPoint();
    G4StepPoint* preStep = step->GetPreStepPoint();
    G4Track* track = step->GetTrack();

    // Set various IDs
    //
    eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    trackID = track->GetTrackID();
    stepID = track->GetCurrentStepNumber();
    parentID = track->GetParentID();

    particleName = track->GetParticleDefinition()->GetParticleName();

    // If postStep is not pointing to any physical volume, set a special flag.
    if(!postStep->GetPhysicalVolume()){
        volumeName = "OutOfWorld";
        volumeCopyNumber = 0;
    }
    else {
        volumeName = postStep->GetPhysicalVolume()->GetName();
        volumeCopyNumber = postStep->GetPhysicalVolume()->GetCopyNo();
    }

    // Retrieve kinematic information.
    //
    position = postStep->GetPosition();
    rx = position.x()/mm;
    ry = position.y()/mm;
    rz = position.z()/mm;

    momentumDir = postStep->GetMomentumDirection();
    px = position.x();
    py = position.y();
    pz = position.z();

    globalTime = postStep->GetGlobalTime()/ns;

    Eki = preStep->GetKineticEnergy()/keV;
    Ekf = postStep->GetKineticEnergy()/keV;
    Edep = step->GetTotalEnergyDeposit()/keV;

    // If this is a first step in the series, set process name to be a special flag.
    //
    if(!postStep->GetProcessDefinedStep()){
        processName = "initStep";
    } 
    else {
        processName = postStep->GetProcessDefinedStep()->GetProcessName();
    }
}

