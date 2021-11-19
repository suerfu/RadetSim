/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file StackingAction.hh
/// \brief Definition of the StackingAction class

#ifndef StackingAction_h
#define StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "EventAction.hh"
#include "utility.hh"
#include "globals.hh"

/// Stacking action class : manage the newly generated particles

class StackingAction : public G4UserStackingAction{

public:

    StackingAction( RunAction* runAction, EventAction* eventAction );

    virtual ~StackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);

    virtual void NewStage();
        //!< This function is called to insert a special event marker for resetting time.

private:

    RunAction* fRunAction;

    EventAction* fEventAction;
        //!< Pointer to EventAction class.
        //!< When a radioactive decay happens with long timescale, this pointer is used to insert a special marker in the step collections to reset time.
};


#endif

