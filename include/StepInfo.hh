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


struct StepInfo{

public:

    StepInfo();
    StepInfo( const G4Step* );

    G4int eventID;
    G4int trackID;
    G4int stepID;
    G4int parentID;

    G4String particleName;

    G4String volumeName;
    G4int volumeCopyNumber;

    G4double Eki;
    G4double Ekf;
    G4double Edep;

    G4ThreeVector position;
    double rx;
    double ry;
    double rz;

    G4ThreeVector momentumDir;
    double px;
    double py;
    double pz;

    G4double globalTime;

    G4String processName;
};

#endif
