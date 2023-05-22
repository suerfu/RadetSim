
#include "MCPulse.h"
#include <cstring>

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


int MCPulse::GetEdepIndex( string name ){

    if( name=="e-" || name=="e+" || name=="gamma" ){
        return 0;
    }
    else if( IsNuclearRecoil( name ) ){
        return 1;
    }
    else{
        return 2;
    }
}


bool MCPulse::IsNuclearRecoil( string name ){
    
    // first check if starts with capital letter
    // if so, it is Geant4's isotope notation
    if( isupper(name[0]) )
        return true;
    
    // next check for a few exceptions
    // including proton, neutron, deuteron, alpha, triton
    if( name.find("proton")!=string::npos )
        return true;
    if( name.find("neutron")!=string::npos )
        return true;
    if( name.find("deuteron")!=string::npos )
        return true;
    if( name.find("triton")!=string::npos )
        return true;
    if( name.find("alpha")!=string::npos)
        return true;

    return false;
}
