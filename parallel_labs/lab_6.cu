#include <cstdlib>
#include <iostream>
#include <ctime>
#include <math.h>
//#include <cuda_runtime.h>

using namespace std;

__global__ void Matrix_A (double *dA, double *dx, int N) {
        int j = blockIdx.x*blockDim.x + threadIdx.x;
        int i = blockIdx.x*blockDim.y + threadIdx.y;
        
        int Nd = (int)(0.15*N);
        
        if (i <= j + Nd && i >= j - Nd) {
            dA[i + j*N] = pow(sin(dx[j])*cos(dx[i]), 2) + (double)N*D(i,j); 
        } else {
            dA[i + j*N]    = 0;        
        }
}

__global__ void AX (double *dAX, double *dA, double *dX, int N) {
    int i = blockIdx.x*blockDim.x + threadIdx.x;
    double sum = 0;
    
    for (int j = 0; j < N; j++) {
        sum += dA[i + j*N]*dX[j];
    }
    
    dAX[i] = sum;
}

__global__ void PHI (double *dPhi, double *dAX, double *dF) {
        int i = blockIdx.x*blockDim.x + threadIdx.x;
        dPhi[i] = dAX[i] - dF[i];
}

__global__ void D_PHI (double *dL, double *dX0, int N) {
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    double sum1 = sum2 = 0.; int k,k1,k2,Nd = (int)(0.15*N);
    if ( i <= j + Nd && i >= j - Nd ) {
        
        if ( i >= 0 && i <= Nd ) {
            k1 = 0; k2 = i+Nd + 1;
        } // область 1
        
        if ( i >= Nd+1 && i < N-Nd ) {
            k1 = i-Nd; k2 = i+Nd+1;
        } // область 2
        
        if ( i >= N-Nd && i < N ) {
            k1 = i-Nd; k2 = N;
        } // область 3
        
        for ( k = k1; k < k2; k++ ){
            sum1 += D(k,j)*(pow(sin(dX0[i])*cos(dX0[k]),2.)+D(i,k)*(double)N);
            sum2 += dX0[k]*(sin(2.*dX0[i])*pow(cos(dX0[k]),2.)*D(i,j)-
            sin(2.*dX0[k])*pow(sin(dX0[i]),2.)*D(k,j));
        } // k
        dL[i+j*N] = sum1 + sum2; // dLT !
    } else {
        dL[i+j*N] = 0.;
    }
}

__global__ void Solve_G (double *dX0, double *dX1, double *dV0, double tau) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    dX1[i] = dX0[i] + tau * dV0[i];
}

__global__ void Eps_G (double *dX0, double *dX1, double *d_dX ) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    d_dX[i] = abs (dX0[i] - dX1[i]);
    dX0[i] = dX1[i];
}
    
int main() {

    while ( eps_G > EPS_G ) {
        Matrix_A <<< nBlk_MtrxA, nTid_MtrxA >>> ( dA, dX0, N );
        AX <<< nBlk_L, nTid_L >>> ( dAX, dA, dX0, N );
        PHI <<< nBlk_L, nTid_L >>> ( dPhi, dAX, dF );
        D_PHI <<< nBlk_MtrxA, nTid_MtrxA >>> ( dL, dX0, N );
        cudaMemset ( dV0, 1, mem_sizeX ); 
        eps_L = 1.;
        
        while ( eps_L > EPS_L ) {
            Solve_L <<< nBlk_L, nTid_L >>> ( dL, dPhi, dV0, dV1, N );
            Eps_L <<< nBlk_L, nTid_L >>> ( dV0, dV1, d_dV, N );
            cudaMemcpy ( h_dV, d_dV, mem_sizeX, cudaMemcpyDeviceToHost );
            eps_L=0.; 
            for ( j = 0; j < N; j++ )
                eps_L += h_dV[j]; 
            eps_L = eps_L / N;
        } //while_L

        Solve_G <<< nBlk_L, nTid_L >>> ( dX0, dX1, dV0, tau );
        Eps_G <<< nBlk_L, nTid_L >>> ( dX0, dX1, d_dX );
        cudaMemcpy ( h_dX, d_dX, mem_sizeX, cudaMemcpyDeviceToHost );
        eps_G=0.; 
        
        for ( k = 0; k < N; k++ )
            eps_G += h_dX[k]; 
        eps_G = eps_G / N;
    } 
    return 0;
}