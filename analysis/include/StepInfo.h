#ifndef STEPINFO_H
#define STEPINFO_H 1


// TrackInfor is used to hold information read from the input ROOT file.
// This is a trajectory/track level information.
//
struct StepInfo{

    int eventID;
    int trackID;
    int parentID;

    char particleName[16];
    char volumeName[16];
    char processName[16];
    
    double position[3];

    double Eki;
    double Ekf;
    double Edep;
    double time;
};

#endif

/*
    inputTree -> SetBranchAddress( "eventID",  &rdata.eventID);
    inputTree -> SetBranchAddress( "trackID",  &rdata.trackID);
    inputTree -> SetBranchAddress( "particle", &rdata.particleName);
    inputTree -> SetBranchAddress( "volume",   &rdata.volumeName);
    inputTree -> SetBranchAddress( "Eki",      &rdata.Eki);
    inputTree -> SetBranchAddress( "Ekf",      &rdata.Ekf);
    inputTree -> SetBranchAddress( "Edep",     &rdata.Edep);
    inputTree -> SetBranchAddress( "t",        &rdata.time);
    inputTree -> SetBranchAddress( "process",  &rdata.processName);

    if( GetParentInfo()==true ){
        inputTree -> SetBranchAddress( "parentID", &rdata.parentID);
        inputTree -> SetBranchAddress( "parentEki", &rdata.parentEki);
        inputTree -> SetBranchAddress( "parentParticle", &rdata.parentParticleName);
        inputTree -> SetBranchAddress( "parentvolume", &rdata.parentVolumeName);
        inputTree -> SetBranchAddress( "parentRx", &rdata.parentPosition[0]);
        inputTree -> SetBranchAddress( "parentRx", &rdata.parentPosition[1]);
        inputTree -> SetBranchAddress( "parentRx", &rdata.parentPosition[2]);
    }
*/
