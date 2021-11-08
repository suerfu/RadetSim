//
// $Id: StepInfo.hh $
//
/// \file StepInfo.hh
/// \brief Definition of the StepInfoclass

#ifndef StepInfo_h
#define StepInfo_h 1

#include "globals.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"

using namespace std;

class StepInfo
{
  public:
    StepInfo();
    StepInfo( const G4Step* );
    virtual ~StepInfo();

    G4int GetEventID();
    void SetEventID( G4int );

    G4int GetTrackID();
    void SetTrackID( G4int );

    G4int GetStepID();
    void SetStepID( G4int );

    G4int GetParentID();
    void SetParentID( G4int );

    G4String GetParticleName();
    void SetParticleName( G4String );

    G4String GetVolumeName();
    void SetVolumeName( G4String );

    G4int GetVolumeCopyNumber();
    void SetVolumeCopyNumber( G4int );

    G4double GetEki();
    void SetEki( G4double );

    G4double GetEkf();
    void SetEkf( G4double );

     G4double GetDepositedEnergy();
    void SetDepositedEnergy( G4double );

    G4ThreeVector GetPosition();
    void SetPosition( G4ThreeVector );

    G4ThreeVector GetMomentumDirection();
    void SetMomentumDirection( G4ThreeVector );

    G4double GetGlobalTime();
    void SetGlobalTime( G4double );

    G4String GetProcessName();
    void SetProcessName( G4String );

  private:

    G4int eventID;
    G4int trackID;
    G4int stepID;
    G4int parentID;

    G4String particle_name;

    G4String volume_name;
    G4int volume_copy_number;

    G4double energy_i;
    G4double energy_f;
    G4double deposited_energy;

    G4ThreeVector position;
    G4ThreeVector momentum_direction;

    G4double global_time;

    G4String process_name;

};

#endif
