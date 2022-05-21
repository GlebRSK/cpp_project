//============================================================================
// Name        : HookJews.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

double function (double x, double y) {
	//return 4*(x-5)*(x-5) + (y-6)*(y-6);
	return 3*x*x + 3*x*y + 5*y*y - 18*x;
}

int main() {
	double error = 0.1, x0 = 1, y0 = 0.5, delta1 = 1, delta2 = 2, d1 = 1, d2 = 1, a = 2, l = 1;
	int step = 0;
	double x = x0, y = y0;
	do {

		cout << "step: " << step << " " << function(x, y) <<  endl;
		cout << function(x + delta1*d1, y) << " " << function(x - delta1*d1, y) << endl;
		if (function(x + delta1*d1, y) < function(x, y)) {
			x += delta1*d1;
			cout << "\tsearch +x" << " x: " << x << " y: " << y << " function: " << function(x, y) << endl;
		}
		else if(function(x - delta1*d1, y) < function(x, y)) {
			x -= delta1*d1;
			cout << "\tsearch -x" << " x: " << x << " y: " << y << " function: " << function(x, y) << endl;
		}
		cout << function(x, y + delta2*d2) << " " << function(x, y - delta2*d2) << endl;
		if (function(x, y + delta2*d2) < function(x, y)) {
			y += delta2*d2;
			cout << "\tsearch +y" << " x: " << x << " y: " << y << " function: " << function(x, y) << endl;
		}
		else if(function(x, y - delta2*d2) < function(x, y)) {
			y -= delta2*d2;
			cout << "\tsearch -y: " << " x: " << x << " y: " << y << " function: " << function(x, y) << endl;
		}

		if (function(x, y) < function(x0, y0)) {
			double x1 = x0,  y1 = y0;
			x0 = x; y0 = y;
			x += l*(x - x1);
			y += l*(y - y1);
			cout << "\tstep on the model: " << x << " " << y << " " << function(x, y) << endl;
		}
		else {
			if (delta1 >= error) delta1 /= a;
			if (delta2 >= error) delta2 /= a;
			cout << "\tchange delta:\n\t\tdelta1: " << delta1 << " delta2: " << delta2 << endl;
		}

		step++;
	} while (((delta1 >= error) && (delta2 >= error)));
	cout << x << "  " << y << function(x, y) << endl;
	return 0;
}
