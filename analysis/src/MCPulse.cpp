
#include "MCPulse.h"


MCPulse::MCPulse(){
    time = -1;
	for(unsigned int i=0; i<nType; i++){
		energy.push_back(0);
	}
}


MCPulse::MCPulse( double t, vector<double> a){
    time = t;

    unsigned int m = nType<a.size()?nType:a.size();
    for(unsigned int i=0; i<m; i++)
        energy.push_back( a[i] );
    for(unsigned int i=m+1; i<nType; i++)
        energy.push_back( 0 );
    
}


MCPulse::MCPulse( const MCPulse& a){
    time = a.time;
    energy = a.energy;
}


MCPulse::~MCPulse(){}


MCPulse& MCPulse::operator=( const MCPulse& a){
    time = a.time;
    energy = a.energy;
    return *this;
}


MCPulse& MCPulse::operator+( const MCPulse& a){

    if( time<0 ){
        time = a.time;
    }
    else{
        time = time<a.time?time:a.time;
    }

    for(unsigned int i=0; i<nType; i++){
        energy[i] += a.energy[i];
    }

    return (*this);
}


void MCPulse::operator+=( const MCPulse& a){
    if( time<0 )
        time = a.time;
    else
        time = time<a.time ? time : a.time;

    for(unsigned int i=0; i<nType; i++){
        energy[i] += a.energy[i];
    }
}


ostream& operator<<( ostream& os, const MCPulse& a){

	os << a.time;

	vector<double> en = a.energy;
	for( unsigned int i=0; i<en.size(); i++ )
		os << ' ' << en[i];

	return os;
}

