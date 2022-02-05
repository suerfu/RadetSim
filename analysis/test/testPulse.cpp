#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "MCPulse.h"

using namespace std;

int main(){

	srand(time(0));

	double t1 = rand()%1000/10000.0;
	vector<double> e1 = { 1.0*(rand()%100), 1.0*(rand()%100), 1.0*(rand()%100) };

	double t2 = rand()%1000/10000.0;
	vector<double> e2 = { 1.0*(rand()%100), 1.0*(rand()%100), 1.0*(rand()%100) };

	MCPulse a( t1, e1 );
	cout << a << endl;

	MCPulse b( t2, e2 );
	cout << b << endl;

	MCPulse c = a+b;
	cout << c << endl;

	return 0;
}
