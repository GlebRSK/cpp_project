#include <cstdlib>
#include <iostream>
#include <ctime>
#include <math.h>
//#include <cuda_runtime.h>

using namespace std;
__global__ void Solve (double *a, double *c, double *x0, double *x1, int N) {

    double aa = 1, sum = 0;
    int t = blockIdx.x * blockDim.x + threadIdx.x;

    for (int i = 0; i < N; i++) {
        //sum += a[i + t*N] * x0[i];
        sum += a[t + i*N] * x0[i];
        if (i == t)
            aa = a[i + t*N];
    }
    x1[t] = x0[t] + (c[t] - sum) / aa;
}

__global__ void Eps (double *x0, double *x1, double *delta, int N) {
    int i = blockIdx.x*blockDim.x + threadIdx.x;
    delta[i] = abs (x0[i] - x1[i]);
    x0[i] = x1[i];
}

void fillingOneDIM(double *a, int size, bool b) {

    if (b) {

        for (int i = 0; i < size; i++) {
            a[i]  = (double)(-10 + rand()*10)/RAND_MAX;
        }
    }
    else {

        for (int i = 0; i < size; i++) {
            a[i] = 1;
        }
    }
}

int main() {
    //srand(time(0));
    double EPS = 1e-8, eps = 1;
    int N = 2048, size = N*N, N_Thread = 512, numIteration = 0;
    int mem_sizeA = sizeof (double)*size;
    int mem_sizeX = sizeof (double)*N;

    double *devA, *devC, *devX0, *devX1, *devDelta;
    double *hA = (double*) malloc (mem_sizeA);
    double *hC = (double*) malloc (mem_sizeX);
    double *hX = (double*) malloc (mem_sizeX);
    double *hX0 = (double*) malloc (mem_sizeX);
    double *hX1 = (double*) malloc (mem_sizeX);
    double *hDelta = (double*) malloc (mem_sizeX);

    fillingOneDIM (hA, size, true);
    fillingOneDIM (hC, N, true);
    fillingOneDIM (hX, N, false);
    fillingOneDIM (hX0, N, false);

    for (int i = 0; i < size; i += (N + 1)) {

        if (hA[i] == 0) {
            hA[i] = 1;
        }
        hA[i] = fabs(hA[i])*70100;
    }

    cudaMalloc ((void**) &devA, mem_sizeA);
    cudaMalloc ((void**) &devC, mem_sizeX);
    cudaMalloc ((void**) &devX0, mem_sizeX);
    cudaMalloc ((void**) &devX1, mem_sizeX);
    cudaMalloc ((void**) &devDelta, mem_sizeX);

    

    cudaMemcpy (devA, hA, mem_sizeA, cudaMemcpyHostToDevice);
    cudaMemcpy (devC, hC, mem_sizeX, cudaMemcpyHostToDevice);
    cudaMemcpy (devX0, hX0, mem_sizeX, cudaMemcpyHostToDevice);

    int N_Blocks = 4;
    
    float timerValueGPU;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord (start, 0);
    
    do {
        numIteration++;
        Solve <<<N_Blocks, N_Thread>>> (devA, devC, devX0, devX1, N);
        Eps <<<N_Blocks, N_Thread>>> (devX0, devX1, devDelta, N);
        cudaMemcpy (hDelta, devDelta, mem_sizeX, cudaMemcpyDeviceToHost);

        eps = 0;
        for (int i = 0; i < N; i++) {
            eps += hDelta[i];
        }
        eps /= N;
    } while(eps > EPS);
//    cout << numIteration << endl;
    cudaMemcpy (hX1, devX0, mem_sizeX, cudaMemcpyDeviceToHost);

    cudaEventRecord (stop, 0);
    cudaEventSynchronize (start);
    cudaEventSynchronize (stop);
        cudaEventElapsedTime (&timerValueGPU, start, stop);
    cout << "time GPU: " << timerValueGPU/1000000 << endl;

    cudaFree(devX0);
    cudaFree(devC);
    cudaFree(devX0);

    double sum = 0;
    numIteration = 0;

    float timeStart = clock()/(float)CLOCKS_PER_SEC;
    eps = 1;

    while (eps > EPS) {
        numIteration++;
        cout << numIteration << endl;
        for (int i = 0; i < N; i++) {
            sum = 0;
            double aa = 1;

            for (int j = 0; j < N; j++) {
                sum += hA[i + j*N]*hX0[j];
                if (i == j)
                    aa = hA[j + i*N];
            }
            hX1[i] = hX0[i] + (hC[i] - sum) / aa;
        }
        eps = 0;

        for (int i = 0; i < N; i++) {
            hDelta[i] = fabs (hX0[i] - hX1[i]);
            eps += hDelta[i];
            hX0[i] = hX1[i];
        }

        eps /= N;
    }

    float timeStop = clock()/(float)CLOCKS_PER_SEC;
    cout << "time CPU: " << (float)timeStop - timeStart << endl;
    return 0;
}
