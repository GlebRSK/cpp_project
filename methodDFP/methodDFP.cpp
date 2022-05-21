//============================================================================
// Name        : methodDFP.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Quasi-Newton methods are methods used to either find zeroes or local maxima and minima of functions, as an alternative to Newton's method. 
//They can be used if the Jacobian or Hessian is unavailable or is too expensive to compute at every iteration. 
//The "full" Newton's method requires the Jacobian in order to search for zeros, or the Hessian for finding extrema.
//============================================================================

#include <iostream>
#include <vector>
#include <math.h>
#include "otherMethods.h"

using namespace std;

#define numVar 2 // number of variables
#define error 0.000001 // error

int main() {
    int step = 0; // number of step
    double lyambda = 0;
    vector < vector <double> > points (numVar, vector <double>(1)); // search points vector
    points[0][0] = 0, points[1][0] = 3; // start point
    vector < vector <double> > points1 (numVar, vector <double>(1)); // additional point vector
    vector < vector <double> > gradient (numVar, vector <double> (1)); // antigradient
    vector < vector <double> > dGradient (numVar, vector <double> (1)); //dg = gradient(step + 1) - gradient (step)
    vector < vector <double> > invHess (numVar, vector <double> (numVar)); // Hess inverse matrix
    /*coefficients A, B for approximation of the inverse Hess matrix*/
    vector < vector <double> > A (numVar, vector <double> (numVar));
    vector < vector <double> > B (numVar, vector <double> (numVar));
    // nGrad = invHess*gradient
    vector < vector <double> > nGrad (invHess.size(), vector <double> (gradient[0].size()));

    for (int i = 0; i < numVar; i++) { //Starting Hess inverse matrix = unit matrix
        for (int j = 0; j < numVar; j++) {
            if (i == j) invHess[i][j] = 1;
            else invHess[i][j] = 0;
        }
    }

    /*********************************DFP Alghorithm: 0 step**************************************************/

    // 0.0 search for a new gradient as a derivate at points. dg = gradient, ngrad = invHess * gradient
    gradient = derivateInPoint (gradient, points[0][0], points[1][0]);
    dGradient = gradient;
    nGrad = multiplication (invHess, gradient);

    // 0.1 recording function coefficients for one-dimensional search
    cMatrix (points[0][0], nGrad[0][0], points[1][0], nGrad[1][0]);
    lyambda = golden_section_method();

    // 0.2 ngrad = lyambda * invHess * gradient
    nGrad = multOnNumber (nGrad, lyambda);

    // 0.3 output of the results at the zero step
    cout << "\t\t\tstep" << step << endl;
    cout << "\npoints:\t" << points[0][0] << " " << points[1][0] << endl;
    cout << "gradient:\t" << gradient[0][0] << " " << gradient[1][0] << endl;

    // 0.4 search new points, new gradients in this points and delta gradinet
    points = addMatrix (points, nGrad, false);
    gradient = derivateInPoint (gradient, points[0][0], points[1][0]);
    dGradient = addMatrix (dGradient, gradient, false);

    // 0.5 output of the results at the zero step
    cout << "function:\t" << function(points[0][0], points[1][0]) << endl;
    cout << "lyambda:\t" <<  lyambda << endl;
    cout << "gradient:\t" << gradient[0][0] << " " << gradient[1][0] << endl;
    cout << "dGradient:\t" << dGradient[0][0] << " " << dGradient[1][0] << endl;

    /*********************************DFP Alghorithm: 1..n step**************************************************
     The cycle continues until the difference between the values of the functions in the step i and i + 1,
     is less than the error
     */
    do {
        step++; // next step

        // 1.0 output of the results at the i step
        cout << "\n\t\t\tstep " << step << endl;
        cout << "\npoints:\t" << points[0][0] << " " << points[1][0] << endl;
        cout << "gradient:\t" << gradient[0][0] << " " << gradient[1][0] << endl;
        cout << "function:\t" << function(points[0][0], points[1][0]) << endl;
        cout << "dGradient:\t" << dGradient[0][0] << " " << dGradient[1][0] << endl;

        // 1.1 new coefficients A, B for approximation of the inverse Hess matrix, new invHess matrix
        A = coeffA (nGrad, dGradient);
        B = coeffB (invHess, dGradient);
        A = addMatrix(A, B, false);
        invHess = addMatrix(invHess, A , true);

        // 2.1 ngrad = lyambda * invHess * gradient, search for a new gradient as a derivate at points.
            //dg = gradient, ngrad = invHess * gradient
        nGrad = multiplication (invHess, gradient);
        cMatrix (points[0][0], nGrad[0][0], points[1][0], nGrad[1][0]);
        lyambda = golden_section_method();
        nGrad = multOnNumber (nGrad, lyambda);

        // 2.2 new points, gradient, delta gradient
        points1 = points;
        points = addMatrix (points, nGrad, false);
        dGradient = gradient;
        gradient = derivateInPoint (gradient, points[0][0], points[1][0]);
        dGradient = addMatrix (dGradient, gradient, false);

        cout << "matrix invHess(Hessian)\n" << invHess[0][0] << "\t" << invHess[0][1] << "\n" <<
                invHess[1][0] << " " << invHess[1][1] << endl;
        cout << "lyambda:\t" <<  lyambda << endl;
    } while ( ( fabs( (function (points[0][0], points[1][0]) - function(points1[0][0], points1[1][0]))) > error) );

    return 0;
}
