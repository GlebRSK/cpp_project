#include <cstdlib>
#include <math.h>
#include <ctime>
#include <omp.h>
using namespace std;

#define koeff 1
#define Nx 100*koeff
#define Ny 100*koeff
#define Nt 100*koeff
#define D pow10(-6)
#define Vx 0.1
#define Vy 0.1
#define hx 0.7/Nx
#define hy 0.7/Ny
#define k pow10(-2)
#define dt 10/Nt

int main() {
    int n = 0;
    double ***Carr = new double**[Nx];
    double *alpha = new double[Nx];
    double *betta = new double[Ny];
    double *_alpha = new double[Nx];
    double *_betta = new double[Ny];
    double **dT2 = new double*[Nx];
    double **dT4 = new double*[Nx];
    for(int i = 0; i < Nx; i++) {
        dT2[i] = new double[Ny];
        dT4[i] = new double[Ny];
        Carr[i] = new double*[Ny];
    }
    for(int i = 0; i < Nx; i++) {
        for(int j = 0; j < Ny; j++) {
            Carr[i][j] = new double[Nt];
        }
    }
    clock_t start_time = clock();
    for(int i = 0; i < Nx; i++) {
        for(int j = 0; j < Ny; j++) {
            Carr[n][i][j] = 0;
        }
    }
    double Ci = -D*dt/(2*pow(hx,2)) - Vx*dt/(2*hx),
            Bi = 1 + D*dt/(pow(hx,2)) + Vx*dt/(2*hx),
            Ai = -D*dt/(2*pow(hx,2));
    alpha[0] = 0;
    betta[0] = 1;
    _alpha[0] = 0;
    _betta[0] = 1;
    while (n < Nt - 1) {
        Carr[n][0][0] = 1;
        Carr[n][Nx - 2][0] = 1;
        Carr[n][0][Ny - 2] = 1;
    #pragma omp shared(Carr, alpha, betta, dt4, dt2) for schedule(dynamic) num_threads(4) {
        for(int j = 1; j < Nx - 2; j++) {
            Carr[n][0][j] = 1;
            for (int i = 1; i < Nx - 2; i++) {
                alpha[i] = -Ai/(Ci*alpha[i - 1] + Bi);
                betta[i] = (Carr[n][i][j] - betta[i - 1]*Ci)/(Bi + Ci*alpha[i - 1]);
            }
            dT4[Nx - 1][j] = betta[Nx - 2]/(1 - alpha[Nx - 2]);
            for(int i = Nx - 2; i >= 1; i--) {
                dT4[i][j] = alpha[i] * dT4[i + 1][j] + betta[i];
            }
        }
        #pragma omp shared(Carr, _alpha, _betta, dT4, dt2) for schedule(dynamic) num_threads(4)
        for(int i = 1; i < Nx - 2; i++) {
            Carr[n][i][0] = 1;
            for (int j = 1; j < Ny - 2; j++) {
                _alpha[j] = -Ai/(Ci*_alpha[j - 1] + Bi);
                _betta[j] = (dT4[i][j] - _betta[j - 1]*Ci)/(Bi + Ci*_alpha[j - 1]);
            }
            dT2[i][Ny - 1] = _betta[Nx - 2]/(1 - _alpha[Nx - 2]);
            for(int j = Ny - 2; j >= 1; j--) {
                dT2[i][j] = _alpha[j] * dT2[i][j + 1] + _betta[j];
            }
        }
        for(int i = 1; i < Nx - 1; i++) {
            for(int j = 1; j < Nx - 1; j++) {
                double lyambdaXX = (dT2[i - 1][j] - 2 * dT2[i][j] +  dT2[i + 1][j]) / pow(hx,2);
                double lyambdaYY = (dT2[i][j - 1] - 2 * dT2[i][j] +  dT2[i][j + 1]) / pow(hy,2);
                Carr[n + 1][i][j] = Carr[n][i][j] + dt*D*(lyambdaXX + lyambdaYY) -
                        (Vx/hx)*(dT2[i][j] - dT2[i - 1][j]) - (Vy/hy)*(dT2[i][j] - dT2[i][j - 1]);

            }
        }
        n++;
    }
    for(int i = 0; i < Nx; i++) {
        for(int j = 0; j < Nx; j++) {
            cout << Carr[50][i][j] << " ";
        }
        cout << "\n";
    }
    clock_t end_time = clock();
    double the_solution_time  = double(end_time - start_time)/CLOCKS_PER_SEC;
    cout << "The solution time of the equation: " << the_solution_time << "\n";
    return 0;
}
