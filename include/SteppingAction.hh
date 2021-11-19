/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file SteppingAction.hh
/// \brief Definition of the SteppingAction class

#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

#include "RunAction.hh"

class GeometryConstruction;
class EventAction;

/// Stepping action class.
///
/// In UserSteppingAction() there are collected the energy deposit and track
/// lengths of charged particles in Absober and Gap layers and
/// updated in EventAction.

class SteppingAction : public G4UserSteppingAction{

public:

    SteppingAction( RunAction* runAction, EventAction* eventAction, const GeometryConstruction* detectorConstruction );

    virtual ~SteppingAction();

    virtual void UserSteppingAction( const G4Step* step );

private:

    RunAction* fRunAction;

    EventAction* fEventAction;

    const GeometryConstruction* fDetectorConstruction;
};


#endif
