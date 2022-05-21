//============================================================================
// Name        : nsKohonen.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <math.h>
#include <string>
#include <ctime>

int const N = 30;
int const M = 3;
double const Rc = 0.52;
double const v = 0.5;

using namespace std;

ostream & operator>>(ostream & lhv, const vector<double> & rhv) { //перегрузка потока. чтобы считать информацию одной
    copy(rhv.begin(), rhv.end(), ostream_iterator<double>(cout << " "));//строчкой
    return lhv;
}

int main() {

    string file_name = "inputData.txt";
    unsigned int start_time = clock();
    srand(time(0));
    ifstream f;
    f.open(file_name);

    if (!f.is_open()) {
        return 1;
    }
    double r = 0, rmin = 1000;

    int numCluster = 1, commonClust = 10;

    vector<double> input((istream_iterator<double>(f)), istream_iterator<double>());
    vector< vector<double> > inputData(N, vector<double>(M));
    vector< vector<double> > weightCoeff(M);
    vector< string > cluster;

    string sArr[N] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",
            "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30"};
    int z = 0;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            inputData[i][j] = input[z];
            z++;
        }
    }

    for (int i = 0; i < M; i++) {
        weightCoeff[i].resize(numCluster);
    }

    cluster.push_back("Cluster: " + sArr[0]);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < numCluster; j++) {
            weightCoeff[i][j] = inputData[numCluster - 1][i];
        }
    }

    for (int i = 1; i < N; i++) {
        for (int j = 0; j < numCluster; j++) {
            for (int o = 0; o < M; o++) {
                r += (weightCoeff[o][j] - inputData[i][o])*(weightCoeff[o][j] - inputData[i][o]);
            }

            if (pow(r, 0.5) <= rmin) {
                rmin = pow(r, 0.5);
                commonClust = j;
            }
            r = 0;
        }

        if (rmin <= Rc) {

            for (int j = 0; j < M; j++) {
                weightCoeff[j][commonClust] += v*(inputData[i][j] - weightCoeff[j][commonClust]);
            }

            string str = cluster[commonClust] + " & " + sArr[i];
            cluster[commonClust] = str;
        } else {
            numCluster++;

            for (int j = 0; j < M; j++) {
                weightCoeff[j].resize(numCluster);
            }

            for (int e = 0; e < M; e++) {
                    weightCoeff[e][numCluster - 1] = inputData[i][e];
            }

            cluster.push_back("Cluster: " + sArr[i]);
        }
        rmin = 10;
    }

    for (int i = 0; i < cluster.size(); i++) {
        cout << cluster[i] << endl;
    }

    f.close();
    unsigned int end_time = clock();
    cout << clock()/1000.0;
    return 0;
}
