/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file /include/TrackingAction.hh
/// \brief Definition of the TrackingAction class
//

#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "RunAction.hh"


class EventAction;


/// This class is mainly implemented to insert a special step at the beginning of a track.
///
class TrackingAction : public G4UserTrackingAction {

public:

    TrackingAction( RunAction* , EventAction* );

    virtual ~TrackingAction() {};

    virtual void PreUserTrackingAction(const G4Track*);
        //!< This method inserts a special step at the beginning of the track.

private:

    RunAction* fRunAction;

    EventAction* fEventAction;
};


#endif
