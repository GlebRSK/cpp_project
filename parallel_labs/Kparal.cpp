//============================================================================
// Name        : Kparal.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <math.h>

using namespace std;

#define Nx 10
#define Ny 10
#define Nt 10

int main() {
    double *alpha = new double[Nx];
    double *betta = new double[Ny];
    double *_alpha = new double[Nx];
    double *_betta = new double[Ny];
    double **dT4 = new double*[Nx];
    double **dT2 = new double*[Nx];
    double k = 2.7, D = pow(10, -1), Vx = 0.1, Vy = 0.1, hx = 0.01, hy = 0.01, dt = 0.01;
    double ***Carr = new double**[Nx];
    for(int i = 0; i < Ny; i++) {
        Carr[i] = new double*[Ny];
    }
    for(int i = 0; i < Nx; i++) {
        for(int j = 0; j < Ny; j++) {
            Carr[i][j] = new double[Nt];
        }
    }
    for(int i = 0; i < Nx; i++) {
        dT4[i] = new double[Nx];
        dT2[i] = new double[Nx];
    }
    int n = 0;
    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            Carr[n][i][j] = 0;
        }
    }
    alpha[0] = 0; betta[0] = 1;
    _alpha[0] = 0; _betta[0] = 1;
    double C = -D*dt/(2*pow(hx,2)), B = 1 + D*dt/(pow(hx,2)) - Vx*dt/(2*hx), A = -D*dt/(2*pow(hx,2)) + Vx*dt/(2*hx);
    while (n < Nt - 1) {
        for(int j = 1; j < Ny-1; j++) {
            for(int i = 1; i < Nx-1; i++) {
                alpha[i] = -A/(B + C*alpha[i-1]);
                betta[i] = (Carr[n][i][j] - C*betta[i-1])/(B + C*alpha[i-1]);
            }
            
            dT4[Nx-1][j] = 1;
            for(int i = Nx - 2; i > 1; i--) {
                dT4[i][j] = alpha[i]*dT4[i + 1][j] + betta[i];
            }
        }
        for(int i = 1; i < Nx - 1; i++) {
            for(int j = 1; j < Ny - 1; j++) {
                _alpha[j] = -A/(B + C*_alpha[i-1]);
                _betta[i] = (dT4[i][j] - C*_betta[i-1])/(B + C*_alpha[i-1]);
            }

            dT2[i][Ny - 1] = 1;
            for(int j = Ny - 2; j > 1; j--) {
                dT2[i][j] = alpha[j]*dT2[i][j + 1] + betta[j];
            }
        }
        for (int i = 1; i < Nx - 1; i++) {
            for(int j = 1; j < Ny - 1; j++) {
                double lyambdaXX = (dT2[i - 1][j] - 2 * dT2[i][j] +  dT2[i + 1][j]) / pow(hx,2);
                double lyambdaYY = (dT2[i][j - 1] - 2 * dT2[i][j] +  dT2[i][j + 1]) / pow(hy,2);
                Carr[n + 1][i][j] = dT2[i][j] + dt*( D*lyambdaXX + D*lyambdaYY + k*dt*dT2[i][j] -
                        (Vx/hx)*(dT2[i][j] - dT2[i - 1][j]) - (Vy/hy)*(dT2[i][j] - dT2[i][j - 1]));
            }
        }
        n++;
    }
    n = 0;
    while (n < Nt - 1) {
        for (int i = 0; i < Nx; i++) {
            for (int j = 0; j < Ny; j++) {
                cout << Carr[n][i][j] << " ";
            } cout << endl;
        }
        n++;
    }
    return 0;
}
