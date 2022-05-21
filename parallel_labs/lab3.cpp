#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define N 7

void array(double **arr){
    for (int i = 0; i < N;i++){
        for(int j = 0; j < N; j++){
            arr[i][j] = rand() % 10 +2;
            cout << arr[i][j] << " ";
        }
        cout << "\n";
    } cout << endl;
}

int main() {
    srand(time(0));
    double **arr = new double*[N];
    for (int i = 0; i < N; i++){
        arr[i] = new double[N];
    }
    array(arr);
    double *average = new double[N];
    #pragma omp paralel    
        #pragma omp for
        for (int j = 0; j < N; j++){
            average[j] = 0;
            for(int i = 0; i < N; i++){
                average[j] += arr[i][j];
            }
            average[j] /= N;
        }
        for(int i = 0; i < N; i++){
            cout << average[i]<< " " ;
        }
        cout << "\n" << endl;
    #pragma omp paralel    
       #pragma omp for
           for(int j = 0; j < N; j++){
            for (int i = 0; i < N; i +=2){
                arr[i][j] = average[j];
            }
        }

      for (int i = 0; i < N; i++){
          for(int j = 0; j < N; j++){
                cout << arr[i][j] << " ";
            }
            cout << "\n";
      }
return 0;
}
