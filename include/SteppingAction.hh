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

#include <set>

class GeometryConstruction;
class EventAction;


/// Stepping action class.
///
/// In UserSteppingAction() there are collected the energy deposit and track
/// The steps will be processed in EventAction.

class SteppingAction : public G4UserSteppingAction{

public:

    SteppingAction( RunAction* runAction, EventAction* eventAction );

    virtual ~SteppingAction();

    virtual void UserSteppingAction( const G4Step* step );
        //!< In this method, G4Steps are used to form StepInfo classes and pushed into EventAction's step collector.

private:

    RunAction* fRunAction;

    EventAction* fEventAction;

};


#endif
