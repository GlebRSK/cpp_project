#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include "/usr/include/mpich/mpi.h"

using namespace std;

#define N 4

void Filling(int **arr) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            arr[i][j] = rand() % 15 - 5;
        }
    }
}

void Minor(int **arr, int **arr1, int M, int k, int l) {
    int i1 = 0, j1 = 0;
    for(int i = 0; i < M-1; i++) {
        j1 = 0;
        if (i == k - 1) {
            i1 = 1;
        }
        for(int j = 0; j < M-1; j++) {
            if (j == l - 1) {
                j1 = 1;
            }
            arr1[i][j] = arr[i + i1][j + j1];
        }
    }
}

void printMatrix(int **arr, int M) {
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < M; j++) {
            cout << arr[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

int Determinant(int **arr, int M, bool _flag) {
    bool flag = _flag;
    double d = 0;
    int k = 1, m = M - 1, det = 0, size, rank;
    int **additionalMatrix;
    MPI_Status  status;
    additionalMatrix = new int*[m];
        if (flag) {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    }
    for(int i = 0; i < m; i++) {
        additionalMatrix[i] = new int[m];
    }
    
    if (m == 1) {
        det = arr[0][0] * arr[1][1] - arr[0][1] * arr[1][0];
        return det;
    } else {
        if (flag) {
            for(int i = 0; i < M; i++) {
                if (rank == i + 1) {
                    Minor(arr, additionalMatrix, M, i + 1, 1);
                    det = k * arr[i][0] * Determinant(additionalMatrix, m, false);
                    MPI_Send(&det, 1, MPI_INT, 0, i, MPI_COMM_WORLD);
                    k = -k;
                }
                if (rank == 0) {
                    for(int i = 0; i < M; i++) {
                        MPI_Recv(&det, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &status);
                        d += det;
                    }
                }
            }
        } else {
            for(int i = 0; i < M; i++) {
                Minor(arr, additionalMatrix, M, i + 1, 1);
                d = k* arr[i][0] * Determinant(additionalMatrix, m, false); 
            }
        }
    }
    return det;
}

int Determ = 0;
void F(int **arr, const int rank, int M) {
    int d = 0;
     d += Determinant(arr, M - 1, true);
    MPI_Reduce(&d, &Determ, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    //srand(time(0));
    int rank;
    int **arr = new int*[N];
    int **arr1 = new int*[N - 1];
    MPI_Init(&argc, &argv);
        MPI_Status st;
    for(int i = 0; i < N; i++) {
        arr[i] = new int[N];
    }
    for(int i = 0; i < N-1; i++) {
        arr1[i] = new int[N - 1];
    }
    Filling(arr);
    int k = 0; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int det, d = 0, count = 0;
    for(int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Minor(arr,arr1, N, i+1, j+1);
            F(arr1, rank, N);
            count++;
        }
    }
    
    if (rank == 0) {
        cout << Determ;
    }
    return 0;
}