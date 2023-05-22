#ifndef MCPULSE_H
#define MCPULSE_H 1

#include <string>
#include <vector>
#include <ostream>

using namespace std;

class MCPulse{

public:

	static unsigned int GetNType(){ return nType; }

	MCPulse();

	MCPulse( double t, vector<double> a);

	MCPulse( const MCPulse& a);

	~MCPulse();

	MCPulse& operator=( const MCPulse& a);

	bool operator<( const MCPulse& a ) const { return time < a.time; }

	MCPulse& operator+( const MCPulse& a);

	void operator+=( const MCPulse& a);

	void SetTime( double a ){ time = a; }
	
	double GetTime() const{ return time; }
	
	void SetEnergy( int i, double a ){ if(i<nType) energy[i] = a; }
	
	void SetEnergy( vector<double> a ){
		if(a.size()==nType)
			energy = a;
	}

	vector<double> GetEnergy(){ return energy; }
		// returns the actual energy vector

	double GetEnergy( int i ){ return energy[i]; }
		// returns a double based on the actual index

	friend ostream& operator<<( ostream& os, const MCPulse& a);
	
    static int GetEdepIndex( string particle_name );
		// This function returns the index for the type of interaction based on the interacting particle.
		// 0 - for ER (all electron, gamma and positron induced).
		// 1 - everything else (should be different types of ion interactions)
		//		as long as one is not simulating super-high-energy interactions or some weird primary particles.
		//		will work on a better categorizing method.
		// 2 - mu- and mu+

    static bool IsNuclearRecoil( string name );

private:

	static const unsigned int nType = 3;

	double time;

	vector<double> energy;

};


#endif
