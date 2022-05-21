#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define blockSize 32

__global__ void Multiplication (int *dA, int *dB, int *dC, int aSize, int bSize, int cSize) {
    int t = blockIdx.y*blockSize + threadIdx.y;
    int v = blockIdx.x*blockSize + threadIdx.x;
    
    int summ = 0;
    
    for (int i = 0; i < (blockSize + aSize -1)/ blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            if ((i*blockSize + j < aSize && t < aSize) && (i*blockSize + j < bSize && v < bSize)) {
                summ += dA[t*aSize + i*blockSize + j]*dB[(i*blockSize + j)*bSize + v];
            }
        }
    }
    if (t < cSize && v < cSize)
        dC[t*cSize + v] = summ;
        
}

void fillingMatrix (int **a, int dim1, int dim2, bool b) {
    if (b) {
        for (int i = 0; i < dim1; i++) {
            for (int j = 0; j < dim2; j++) {
                a[i][j] = rand() % 15 - 5;
            }
        }
    } 
    else {
        for (int i = 0; i < dim1; i++) {
            for (int j = 0; j < dim2; j++) {
                a[i][j] = 0;
            }
        }
    }
}

void twiceDimToOneDim(int **dim2, int *dim1, int x, int y, bool needTranspose) {
    if (!needTranspose) {
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++)    dim1[i*x + j] = dim2[i][j];
        }
    }
    else {
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++)    dim1[i*x + j] = dim2[j][i];
        }
    }
}

void multiply (int *a, int *b, int *c, int dimA, int dimB) {
    int summ = 0;
    for (int i = 0; i < dimA; i++) {
        for (int j = 0; j < dimB; j++) {
            for (int e = 0; e < blockSize*blockSize; e++) {
                summ += a[j*blockSize*blockSize + e]*b[j*blockSize*blockSize + e];
            }
        }
    }
}

void printMatrix (int **a, int b, int c, string matrixName) {
    
    for (int i = 0; i < b; i++) {
        for (int j = 0; j < c; j++) {
            cout << a[i][j] << " ";
        } cout << endl;
    }
    cout << endl;
}

int main() {
    srand(time(0));

    int dimyA = 1024, dimxA = 1024, dimxB = 1024, dimyB = 1024;
    int deepC= dimxA/blockSize;
    int **a = new int*[dimyA];
    int **b = new int*[dimyB];
    int ***c = new int**[dimyA];
    int **ct = new int*[dimyA];
    int *devA, *devB, *devC;
    int *oneDimA = new int[dimyA*dimxA];
    int *oneDimB = new int[dimxB*dimyB];
    int *oneDimC = new int[dimyA*dimxB];

    for (int i = 0; i < dimyA; i++) {
        a[i] = new int[dimxA];
        c[i] = new int*[dimxB];
        ct[i] = new int[dimxB];
        for (int j = 0; j < dimxB; j++) {
            c[i][j] = new int[deepC];
        }
    }
    
    for (int i = 0; i <dimyB; i++) {
        b[i] = new int[dimxB];
    }
    
    
    clock_t startCPU = clock();
    
    fillingMatrix(a, dimyA, dimxA, true);
    fillingMatrix(b, dimyB, dimxB, true);
    fillingMatrix(ct, dimyA, dimxB, false);
    
    clock_t endCPU = clock();
    cout << (float)(endCPU - startCPU)/CLOCKS_PER_SEC << endl;
    
    startCPU = clock();
    for (int i = 0; i < dimxA; i++) {
        for (int j = 0; j < dimxA; j++) {
            for (int e = 0; e < dimxA; e++) {
                ct[i][j] += a[i][e]*b[e][j];    
            }
        }
    }
    
    endCPU = clock();
    cout << (float)(endCPU - startCPU)/CLOCKS_PER_SEC << endl;

    startCPU = clock();
    int countLayer = 0;
    while (countLayer < deepC) {
        for (int i = 0; i < dimyA; i += blockSize) {
            for (int j = 0; j < dimxB; j += blockSize) {
                for (int e = 0; e < blockSize; e++) {
                    for (int o = 0; o < blockSize; o++) {
                        for (int u = 0; u < blockSize; u++) {
                            c[i + e][j + o][0] += a[i + e][j + u]*b[i + u][j + o];
                        }
                    }
                }
            }
        }
        countLayer++;
    }

    endCPU = clock();
    cout << (float)(endCPU - startCPU)/CLOCKS_PER_SEC << endl;
    //printMatrix(ct, dimyA, dimxB, "C");
    
    twiceDimToOneDim(a, oneDimA, dimyA, dimxA, false);
    twiceDimToOneDim(b, oneDimB, dimyB, dimxB, false);
    twiceDimToOneDim(ct, oneDimC, dimyB, dimxB, false);

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    
    cudaMalloc ((void**) &devA, sizeof (int)*dimyA*dimxA);
    cudaMalloc ((void**) &devB, sizeof (int)*dimxB*dimyB);
    cudaMalloc ((void**) &devC, sizeof (int)*dimyA*dimxB);
    
    cudaMemcpy(devA, oneDimA, sizeof (int)*dimyA*dimxA, cudaMemcpyHostToDevice);
    cudaMemcpy(devB, oneDimB, sizeof (int)*dimyB*dimxB, cudaMemcpyHostToDevice);

    dim3 dimBlock(dimyA, dimyA);
    dim3 dimGrid(blockSize, blockSize);
    
    Multiplication <<<dimGrid, dimBlock>>> (devA, devB, devC, dimyA, dimyB, dimxA);
    
    cudaMemcpy(oneDimC, devC, sizeof (int)*dimyA*dimxB, cudaMemcpyDeviceToHost);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float eps;
    
    cudaEventElapsedTime(&eps, start, stop);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    
    /*for (int i = 0; i < dimyA; i++) {
        for (int j = 0; j < dimxA; j++) {
            cout << oneDimC[i*dimxA + j] << " ";
        } cout << endl;
    }*/
 

    cout << eps/1000 << endl;
    
    return 0;
}
