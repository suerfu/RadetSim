//
//
// $Id: StepInfo.cc $
//
/// \file StepInfo.cc
/// \brief Implementation of the StepInfo class

#include "StepInfo.hh"

#include "globals.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StepInfo::StepInfo()
  : eventID(0),
    trackID(0),
    stepID(0),
    parentID(0),
    particle_name(""),
    volume_name(""),
    volume_copy_number(0),
    energy_i(0),
    energy_f(0),
    deposited_energy(0),
    position(0),
    momentum_direction(0),
    global_time(0),
    process_name("")
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StepInfo::StepInfo( const G4Step* step )
  : eventID(0),
    trackID(0),
    stepID(0),
    parentID(0),
    particle_name(""),
    volume_name(""),
    volume_copy_number(0),
    energy_i(0),
    energy_f(0),
    deposited_energy(0),
    position(0),
    momentum_direction(0),
    global_time(0),
    process_name("")
{
    G4StepPoint* postStep = step->GetPostStepPoint();
    G4StepPoint* preStep = step->GetPreStepPoint();
    G4Track* track = step->GetTrack();

    eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    trackID = track->GetTrackID();
    stepID = track->GetCurrentStepNumber();
    parentID = track->GetParentID();

    particle_name = track->GetParticleDefinition()->GetParticleName();

    if(!postStep->GetPhysicalVolume()){
        volume_name = "OutOfWorld";
        volume_copy_number = 0;
    }
    else {
        volume_name = postStep->GetPhysicalVolume()->GetName();
        volume_copy_number = postStep->GetPhysicalVolume()->GetCopyNo();
    }

    energy_i = preStep->GetKineticEnergy();
    energy_f = postStep->GetKineticEnergy();
    /*
    G4double pre_energy;
    if(eventID>=1){
        pre_energy = preStep->GetKineticEnergy();
        deposited_energy = pre_energy - energy;
    }else{
        deposited_energy = 0;
    }*/
    deposited_energy = step->GetTotalEnergyDeposit();

    position = postStep->GetPosition();
    momentum_direction = postStep->GetMomentumDirection();
    global_time = postStep->GetGlobalTime();

    if(!postStep->GetProcessDefinedStep()){
        process_name = "initStep";
    } 
    else {
        process_name = postStep->GetProcessDefinedStep()->GetProcessName();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StepInfo::~StepInfo()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int StepInfo::GetEventID()
{
  return eventID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetEventID( G4int newID )
{
  eventID = newID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int StepInfo::GetTrackID()
{
  return trackID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetTrackID( G4int newID )
{
  trackID = newID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int StepInfo::GetStepID()
{
  return stepID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetStepID( G4int newID )
{
  stepID = newID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int StepInfo::GetParentID()
{
  return parentID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetParentID( G4int newID )
{
  parentID = newID;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String StepInfo::GetParticleName()
{
  return particle_name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetParticleName( G4String new_name )
{
  particle_name = new_name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String StepInfo::GetVolumeName()
{
  return volume_name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetVolumeName( G4String new_name )
{
  volume_name = new_name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int StepInfo::GetVolumeCopyNumber()
{
  return volume_copy_number;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetVolumeCopyNumber( G4int new_copy_number )
{
  volume_copy_number = new_copy_number;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double StepInfo::GetEki()
{
  return energy_i;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetEki( G4double new_energy )
{
  energy_i = new_energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double StepInfo::GetEkf()
{
  return energy_f;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetEkf( G4double new_energy )
{
  energy_f = new_energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double StepInfo::GetDepositedEnergy()
{
  return deposited_energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetDepositedEnergy( G4double new_deposited_energy )
{
  deposited_energy = new_deposited_energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector StepInfo::GetPosition()
{
  return position;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetPosition( G4ThreeVector new_position )
{
  position = new_position;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector StepInfo::GetMomentumDirection()
{
  return momentum_direction;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetMomentumDirection( G4ThreeVector new_momentum_direction )
{
  momentum_direction = new_momentum_direction;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double StepInfo::GetGlobalTime()
{
  return global_time;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetGlobalTime( G4double new_global_time )
{
  global_time = new_global_time;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String StepInfo::GetProcessName()
{
  return process_name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void StepInfo::SetProcessName( G4String new_process_name )
{
  process_name = new_process_name;
}
