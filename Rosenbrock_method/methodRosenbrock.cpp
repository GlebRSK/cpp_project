//============================================================================
// Name        : methodRosenbrock.cpp
// Author      : Gleb
// Version     : 1.3
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

int const numVar = 2; //variables count
double const error = 0.6; //calculation error
double const expansionCoeff = 2; //expansion coeff
double const compressionCoeff = -0.5; // compression coeff
int const maxStep = 3; // count of fail steps

double function (double, double); // minimization function
void directionCheck(double*, double*, double*, double*, int, const vector <vector <double> >&);

vector <vector <double> > multiplication (const vector <vector <double> >&, const vector <vector <double> >&);
vector <vector <double> > addMatrix (vector <vector <double> >&, vector <vector <double> >&, bool);
vector <vector <double> > transpose (const vector <vector <double> >&); //транспонирование матрицы
vector <vector <double> > multOnNumber (vector <vector <double> >&, double); //умножение матрицы на число
// Gram-Shmidt procedure
vector <vector <double> > gramShmidt (double, double, double, double, vector <vector <double> >&);

int main() {
    vector < vector <double> > directionalMatrix (numVar, vector <double> (numVar)); // directional matrix
    double x0 = 8, y0 = 9; // start points
    double delta1 = 1, delta2 = 2;
    double x, y, x1, y1;
    int count = 0, step = 0;
    
//initianal filling of directional matrix
    for (int i = 0; i < numVar; i++) {
        for (int j = 0; j < numVar; j++) {
            if (i == j) directionalMatrix[i][j] = 1;
            else directionalMatrix[i][j] = 0;
        }
    }
    
    x = x0, y = y0 ,x1 = x0, y1 = y0;
    do {
        if (count > maxStep) {
            gramShmidt(x0, x1, y0, y1, directionalMatrix);
            delta1 = 1, delta2 = 2;
            count = 0;
        }
        label: // step label
        step++; 
        for (int i = 0; i < numVar; i++) { // second step
            directionCheck(&x, &y, &delta1, &delta2, i, directionalMatrix);
        }
        cout << "\ndirection check\n";
        cout << "x: " << x << " y: " << y << " delta1 " << delta1 << " delta2 " << delta2 << " " <<
                function(x,y) << endl;
        if (function(x, y) < function(x1, y1)) { // if directional descent is success
            x1 = x, y1 = y; // rewrite points
            cout << "2 x: " << x << " y: " << y << " delta1 " << delta1 << " delta2 " << delta2 << " " <<
                    function(x,y) << endl;
            goto label; // go to next step
        }
        else { 
            count++; // count of failed steps
            if (function(x1, y1) < function(x0, y0)) { // if this iteration has at least one success step
                if ( sqrt ( (x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1)) < error) { // check stop condition
                    break; 
                }
                else { 
                    gramShmidt(x0, x1, y0, y1, directionalMatrix); // create Gram-Shmidt procedure
                    x0 = x1, y0 = y1; // rewrite points
                    delta1 = 1, delta2 = 2; // rewrite delta coeff
                    goto label;
                }
            }
            else if (count <= maxStep) { // else, iteration hasn't success steps, and count of failed iteration bigger then num iter
                if ((fabs(delta1) > error) || (fabs(delta2) > error)) { // check stop condition
                    
                    cout << "3 x: " << x << " y: " << y << " delta1 " << delta1 << " delta2 " << delta2  << " " <<
                            function(x,y) << endl;
                    goto label;
                }
                else break;
            }
        }
    } while ( ( sqrt ( (x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1))) > error);// stop condition
    cout << step << " " << x << " " << y << " " << function(x, y);
    return 0;
}

double function (double x, double y) { // create function 
    return 4*(x - 5)*(x - 5) + (y - 6)*(y - 6);
}

void directionCheck(double *x, double *y, double *d1, double *d2, int i, const vector <vector <double> > &v) {
    if (i == 0) { // check first coordinate
        if (function(*x + *d1*v[i][i], *y + *d1*v[i][1]) < function(*x, *y)) {
                                                                               
            *x += *d1*v[i][i]; 
            *y += *d1*v[i][1];
            *d1 *= expansionCoeff;
        }
        else { 
            *d1 *= compressionCoeff;
        }
    }
    else { // check second coordinate
        if (function(*x + *d1*v[i][0], *y + *d2*v[i][i]) < function(*x, *y)) {
            *x += *d1*v[i][0];
            *y += *d2*v[i][i];
            *d2 *= expansionCoeff;
        }
        else {
            *d2 *= compressionCoeff;
        }
    }

}
// Gram-Shmidt procedure
vector <vector <double> > gramShmidt (double x, double y, double x1, double y1, vector <vector <double> >&v) {
    vector < vector <double> > v1 (numVar, vector <double> (numVar));
    vector < vector <double> > B (numVar, vector <double> (1));
    vector < vector <double> > a1 (numVar, vector <double> (1));
    vector < vector <double> > a2 (numVar, vector <double> (1));
    vector < vector <double> > b1 (numVar, vector <double> (1));
    vector < vector <double> > b2 (numVar, vector <double> (1));
    vector < vector <double> > d3 (numVar, vector <double> (1));
    v1 = v;
    B[0][0] = y - x, B[1][0] = y1 - x1;
    double det = v[0][0]*v[1][1] - v[1][0]*v[0][1];
    v[1][0] = - v[1][0];
    v[0][1] = - v[0][1];
    double temp = v[0][0];
    v[0][0] = v[1][1];
    v[1][1] = temp;
    for (int i = 0; i < numVar; i++) {
        for (int j = 0; j < numVar; j++) {
            v1[i][j] /= det;
        }
    }
    B = multiplication(v1, B);
    a1[0][0] = B[0][0]*v[0][0] + B[1][0]*v[0][1];
    a1[1][0] = B[0][0]*v[1][0] + B[1][0]*v[1][1];
    a2[0][0] = B[1][0]*v[1][0];
    a2[1][0] = B[1][0]*v[1][1];
    b1 = a1;
    double normB = sqrt(b1[0][0]*b1[0][0] + b1[1][0]*b1[1][0]);
    v[0][0] = b1[0][0]/normB; v[1][0] =  b1[1][0]/normB;
    d3[0][0] = v[0][0], d3[1][0] = v[1][0];
    b2 = multiplication (multiplication(transpose(a2), d3), d3);
    vector < vector <double> > temp1 (1, vector <double> (1));
    temp1 = multiplication(transpose(a2), d3); temp = temp1[0][0];
    b2 = multOnNumber(d3, temp);
    b2 = addMatrix(a2, b2, false);
    normB = sqrt(b2[0][0]*b2[0][0] + b2[1][0]*b2[1][0]);
    v[0][1] = b2[0][0]/normB, v[1][1] = b2[1][0]/normB;
    return v;
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
vector <vector <double> > multOnNumber (vector <vector <double> > &v, double number) {

    for (unsigned short i = 0; i < v.size(); i++) {
        for (unsigned short j = 0; j < v[0].size(); j++) {
            v[i][j] *= number;
        }
    }
    return v;
}
