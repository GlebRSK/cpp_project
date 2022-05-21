#include <cstdlib>
#include <iostream>
#include "/usr/include/mpich/mpi.h"

using namespace std;

#define lengthMess 15

int main(int argc, char** argv){    
    MPI_Status status;
    int N = 5, rank;
    char *wMessage = new char[lengthMess];
    char *hMessage = new char [lengthMess];
    bool wife = false, husband = true;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    for (int i = 0; i < N; i++){ 
        if (rank == 0){
            
            for(int j = 0; j < lengthMess; j++){
                wMessage[j] = (char)(rand() % 26 + 65);
                if ((j == 5) or (j == 10)){
                    wMessage[j] = (char)32;
                }
            }
            MPI_Send(wMessage, sizeof(wMessage), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                  MPI_Recv(wMessage, sizeof(wMessage), MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);            
            cout << husband << " " << rank << " " << wMessage << endl;
          }else{
            for(int j = 0; j < lengthMess; j++){
                                hMessage[j] = (char)(rand() % 26 + 65);
                                if ((j == 5) or (j == 10)){
                                        hMessage[j] = (char)32;
                                }
                        }
            MPI_Send(hMessage, sizeof(hMessage), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(hMessage, sizeof(hMessage), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
                        cout << wife << " " << rank << " " << hMessage << endl;
           }
    }
    
    MPI_Finalize();
    return 0;
}
