/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file StepInfo.hh
/// \brief Definition of the StepInfo class

#ifndef STEPINFO_H
#define STEPINFO_H 1

#include "globals.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"

using namespace std;


class StepInfo{

public:

    StepInfo();
    StepInfo( const G4Step* );

    void SetEventID( G4int i){ eventID = i; }
    G4int GetEventID(){ return eventID; }

    void SetTrackID( G4int i){ trackID = i; }
    G4int GetTrackID(){ return trackID; }

    void SetStepID( G4int i){ stepID = i; }
    G4int GetStepID(){ return stepID; }

    void SetParentID( G4int i){ parentID = i; }
    G4int GetParentID(){ return parentID; }

    void SetParticleName( G4String name){ particleName = name; }
    G4String GetParticleName(){ return particleName;}

    void SetVolumeName( G4String name){ volumeName = name; }
    G4String GetVolumeName(){ return volumeName;}

    void SetVolumeCopyNumber( G4int i){ volumeCopyNumber = i; }
    G4int GetVolumeCopyNumber(){ return volumeCopyNumber; }

    void SetNextVolumeName( G4String name){ nextVolumeName = name; }
    G4String GetNextVolumeName(){ return nextVolumeName;}

    void SetEki(G4double a){ Eki = a;}
    G4double GetEki(){ return Eki;}

    void SetEkf(G4double a){ Ekf = a;}
    G4double GetEkf(){ return Ekf;}

    void SetEdep(G4double a){ Edep = a;}
    G4double GetEdep(){ return Edep;}

    void SetPosition(G4ThreeVector a){ position = a;}
    G4ThreeVector GetPosition(){ return position;}

    void SetMomentumDir(G4ThreeVector a){ momentumDir = a.unit();}
    G4ThreeVector GetMomentumDir(){ return momentumDir;}

    void SetGlobalTime(G4double a){ globalTime = a;}
    G4double GetGlobalTime(){ return globalTime;}

    void SetProcessName(G4String s){ processName = s;}
    G4String GetProcessName(){ return processName;}

private:

    G4int eventID;
    G4int trackID;
    G4int stepID;
    G4int parentID;

    G4String particleName;

    G4String volumeName;
    G4int volumeCopyNumber;
    
    G4String nextVolumeName;

    G4double Eki;
    G4double Ekf;
    G4double Edep;

    G4ThreeVector position;
    G4ThreeVector momentumDir;
    G4double globalTime;

    G4String processName;
};

#endif
