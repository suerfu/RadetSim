#ifndef STEPINFO_H
#define STEPINFO_H 1


// TrackInfor is used to hold information read from the input ROOT file.
// This is a trajectory/track level information.
//
struct StepInfo{

    int eventID;
    int trackID;
    int parentID;

    char particle_name[16];
    char volume_name[16];
    char proc_name[16];

    double Eki;
    double Ekf;
    double Edep;
    double time;
};

#endif
