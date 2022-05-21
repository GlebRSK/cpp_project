/*
 * otherMethods.cpp
 *
 *  Created on: 5 ����. 2018 �.
 *      Author: Gleb
 */
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

double coefficient_matrix[4]; //coefficients for one-dimensional search

void cMatrix(double a, double b, double c, double d) {
    coefficient_matrix[0] = a; // new coefficient_matrix
    coefficient_matrix[1] = b;
    coefficient_matrix[2] = c;
    coefficient_matrix[3] = d;
}

double function (double x, double y) {  // function for minimization
    return pow(x-2, 4) + pow(x - 2*y, 2);
}

double lFunction (double x) { //function for one-dimensional search
    return function (coefficient_matrix[0] - coefficient_matrix[1]*x, coefficient_matrix[2] - coefficient_matrix[3]*x);

}

vector <vector <double> > transpose(const vector <vector <double> > &v) {
    unsigned short _N = v.size(), _M = v[0].size();
    vector <vector <double> > vT (_M, vector <double>(_N));

    for (unsigned short i = 0; i < _N; i++) {
        for (unsigned short j = 0; j < _M; j++) {
            vT[j][i] = v[i][j];
        }
    }
    return vT;
}

vector <vector <double> > multiplication (const vector <vector <double> > &v1, const vector <vector <double> >
                                    &v2) {
    unsigned short dim1 = v1.size(), dim2 = v1[0].size(), dim3 = v2[0].size();
    vector <vector <double> > multV (dim1, vector <double>(dim3));

    for (int i = 0; i < dim1; i++) {
        for (int j = 0; j < dim3; j++) {
            multV[i][j] = 0;
        }
    }

    for (unsigned short i = 0; i < dim1; i++) {
        for (unsigned short j = 0; j < dim3; j++) {
            for (unsigned short k = 0; k < dim2; k++) {
                multV[i][j] += v1[i][k]*v2[k][j];
            }
        }
    }
    return multV;
}

vector <vector <double> > derivateInPoint (vector <vector <double> > &gradient, double x, double y) {
    double eps = 0.00001;
    gradient[0][0] = (function(x + eps, y) - function(x, y))/eps;
    gradient[1][0] = (function(x, y + eps) - function(x, y))/eps;
    return gradient;
}

// When we localize border for one-dimensional optimaization, we use the lFunction
vector <vector <double> > svennsAlghorithm(vector <vector <double> > &border, double x0) {
    double h = 1, x, d = 0, a = 0, b = 0;;
    x = x0;

    if ((lFunction(x0 - h) >= lFunction(x0)) && (lFunction(x0 + h) >= lFunction(x0))) {
        border[0][0] = x0 - h;
        border[1][0] = x0 + h;
        return border;
    }

    if ((lFunction(x0 - h) <= lFunction(x0)) && (lFunction(x0) >= lFunction(x0 + h))) {
        cout << "point x0= " << x0 << " is wrong. You can specify different starting point.";
    }

    if ((lFunction(x0 - h) >= lFunction(x0)) && (lFunction(x0) >= lFunction(x0 + h))) {
        d = h;
        a = x0;
        x += h;
    }
    else if ((lFunction(x0 - h) <= lFunction(x0)) && (lFunction(x0) <= lFunction(x0 + h))) {
        d = -h;
        b = x0;
        x -= h;
    }

    x0 = x;
    int k = 1;
    x += pow(2, (double)k)*d;

    while (lFunction(x) <= lFunction(x0)) {
        if (d > 0) a = x0;
        else b = x0;
        k++;
        x0 = x;
        x += pow(2, (double)k)*d;
    }

    if (d > 0) b = x;
    else a = x;
    border[0][0] = a;
    border[1][0] = b;
    return border;
}

double golden_section_method () {
    vector <vector <double> > border (2, vector <double>(1));
    border = svennsAlghorithm(border, 1);
    //cout << "Swan's Method " << border[0][0] << " " << border[1][0] << endl;
    double error = 0.00001, golden_ratio = (3 - sqrt(5))/2;
    double y = border[0][0] + golden_ratio*(border[1][0] - border[0][0]), z = border[1][0] + border[0][0] - y;

    while (fabs(border[1][0] - border[0][0]) > error) {

        if (lFunction(y) <= lFunction(z)) {
            border[1][0] = z;
            z = y;
            y = border[1][0] + border[0][0] - y;
        }
        else {
            border[0][0] = y;
            y = z;
            z = border[1][0] + border[0][0] - z;
        }
    }
    return (border[1][0] + border[0][0])/2;
}

vector <vector <double> > multOnNumber (vector <vector <double> > &v, double number) {

    for (unsigned short i = 0; i < v.size(); i++) {
        for (unsigned short j = 0; j < v[0].size(); j++) {
            v[i][j] *= number;
        }
    }
    return v;
}

vector <vector <double> > addMatrix (vector <vector <double> > &v1, vector <vector <double> > &v2, bool bSign) {
    int a = 1;
    if (!bSign) a = -1;

    for (unsigned short i = 0; i < v1.size(); i++) {
        for (unsigned short j = 0; j < v1[0].size(); j++) {
            v1[i][j] += a*v2[i][j];
        }
    }
    return v1;
}

vector <vector <double> > coeffA (vector <vector <double> > &v1, vector <vector <double> > &v2) {
    vector <vector <double> > numerator;
    vector <vector <double> > denominator;
    numerator = multiplication (v1, transpose (v1));
    denominator = multiplication (transpose (v1), v2);
    double d = 1 / denominator[0][0];
    return multOnNumber (numerator, d);
}

vector <vector <double> > coeffB (vector <vector <double> > &v1, vector <vector <double> > &v2) {
    vector <vector <double> > numerator;
    vector <vector <double> > denominator;
    numerator = multiplication (v1, v2);
    numerator = multiplication(numerator, transpose(v2));
    numerator = multiplication(numerator, transpose(v1));
    denominator = multiplication (transpose (v2), multiplication (v1, v2));
    double number = 1/denominator[0][0];
    return multOnNumber (numerator, number);
}
