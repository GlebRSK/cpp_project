/*
 * otherMethods.h
 *
 *  Created on: 5 ����. 2018 �.
 *      Author: Gleb
 */

#ifndef OTHERMETHODS_H_
#define OTHERMETHODS_H_

#include <vector>

using namespace std;

vector <vector <double> > transpose (const vector <vector <double> >&);
vector <vector <double> > multiplication (const vector <vector <double> >&, const vector <vector <double> >&);
vector <vector <double> > derivateInPoint (vector <vector <double> >&, double, double);
vector <vector <double> > multOnNumber (vector <vector <double> >&, double);
double function (double, double);
double lFunction (double);
vector <vector <double> > svennsAlghorithm (vector <vector <double> >&, double);
void cMatrix (double, double, double, double);
double golden_section_method ();
vector <vector <double> > addMatrix (vector <vector <double> >&, vector <vector <double> >&, bool);
vector <vector <double> > coeffA (vector <vector <double> >&, vector <vector <double> >&);
vector <vector <double> > coeffB (vector <vector <double> >&, vector <vector <double> >&);
#endif /* OTHERMETHODS_H_ */
