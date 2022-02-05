#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "MCPulseArray.h"

using namespace std;

int main(){
	
	srand(time(0));

	MCPulseArray array1;
	MCPulseArray array2;

	unsigned int n = 10;
	
	int daqWindow = 10;
	int coinWindow = 3;

	cout << "DAQ Window: " << daqWindow << endl;
	cout << "Coin window: " << coinWindow << endl;

	for(unsigned int i=0; i< n; i++){

		double t1 = rand()%100;
		vector<double> e1 = { 1.0*(rand()%100), 1.0*(rand()%100), 1.0*(rand()%100) };
		MCPulse p(t1, e1);

		array1.PushBack(p);

		t1 = rand()%100;
		e1 = { 1.0*(rand()%100), 1.0*(rand()%100), 1.0*(rand()%100) };
		p = MCPulse(t1, e1);

		array2.PushBack(p);
	}

	//cout << "Original: " << endl;
	//cout << array << endl;

	array1.Sort();
	array2.Sort();

	cout << "Sorted: " << endl;
	cout << array1 << endl;
	cout << array2 << endl;

	array1.ResetIterator();
	array2.ResetIterator();

	double t = array1.GetNextInteractionTime();

	while( t>=0 ){

		MCPulse p = array1.FindPrimaryInteraction( t, daqWindow );

		cout << "Event at t = " << t << endl;
		cout << p << endl;

		p = array2.FindCoincidentInteraction( t, coinWindow, daqWindow );
		if( p.GetTime()>0 ){
			cout << "Coincident event at t = " << p.GetTime() << endl;
			cout << p << endl;
		}
	
		cout << endl;
		
		t = array1.GetNextInteractionTime();
	}

/*



	double t2 = rand()%1000/10000.0;
	vector<double> e2 = { 1.0*(rand()%100), 1.0*(rand()%100), 1.0*(rand()%100) };

	MCPulse a( t1, e1 );
	cout << a << endl;

	MCPulse b( t2, e2 );
	cout << b << endl;

	MCPulse c = a+b;
	cout << c << endl;
*/
	return 0;
}
