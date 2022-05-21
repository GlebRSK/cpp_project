//============================================================================
// Name        : bigDataLab_1.cpp
// Author      : Gleb
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream.h>
#include <string.h>
#include <math.h>

using namespace std;

int const numVar = 2;

void importData (int *x1, int *x2, double *y, int size) {
	ifstream file ("data_lab1.txt", ios::out);
	string str;

	int count = 0, index = 0;
		while (getline(file, str)) {
			string substr = "";
			count = 0;
			for (unsigned int i = 0; i < str.length(); i++) {
				if ((int)str[i] != 9) {
					substr += str[i];
				}
				else {
					count++;
					switch (count) {
						case 1:
							index = atoi(substr.c_str());
							break;
						case 2:
							x1[index - 1] = atoi(substr.c_str());
							break;
						case 3:
							x2[index -1] = atoi(substr.c_str());
							break;
						default:
							break;
					}
					substr = "";
				}
			}
			index <= size ? y[index - 1]= atof(substr.c_str()) : index = 0;
		}
		file.close();
}

double** allocationMatrix (double **x, int dim1, int dim2) {
	x = new double*[dim1];
	for (int i = 0; i < dim1; i++) {
		x[i] = new double[dim2];
	}
	return x;
}

void transposeMatrix (double **x, double **xT, int dim1, int dim2) {
	for (int i = 0; i < dim1; i++)
		for (int j = 0; j < dim2; j++)
			xT[j][i] = x[i][j];
}

void multiplicationMatrix (double **x, double **y, double **c, int dimX1, int dimY2, int dimCommon) {

	for (int i = 0; i < dimX1; i++) {
		for (int j = 0; j < dimY2; j++) {
			for (int e = 0; e < dimCommon; e++) {
				c[i][j] += x[i][e] * y[e][j];
			}
		}
	}

}

void reverseMatrix (double **x, int size) {
	double **e = allocationMatrix(e, size, size);
	double a = 0;

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++) i == j ? e[i][j] = 1 : e[i][j] = 0;

	for(int i = 0; i < size; i++){

		a = x[i][i];
		for(int j = 0 ; j < size; j++){
			x[i][j] /= a;
			e[i][j] /= a;
		}

		for (int k = i + 1; k < size; k++ ){

			a = x[k][i];
			for(int j = 0; j < size; j++){
				x[k][j] -= x[i][j] * a;
				e[k][j] -= e[i][j] * a;
			}
		}
	}

	for(int i = size - 1; i >0; i--){
		for(int j = i - 1; j >= 0; j--){

			a = x[j][i];
			for(int k = 0; k < size; k++){
				x[j][k] -= x[i][k] * a;
				e[j][k] -= e[i][k] * a;
			}
		}
	}

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			x[i][j] = e[i][j];
}

double** mnk1 (int *x1matrix, int *x2matrix, double *ymatrix, int N) {
	double x1 = 0, x2 = 0, y = 0, yx1 = 0, x12 = 0, x11 = 0, yx2 = 0, x22 = 0;
	double **B = allocationMatrix(B, numVar + 1, 1);

	for (int i = 0; i < N; i++) {
			x1 += x1matrix[i];
			x2 += x2matrix[i];
			y += ymatrix[i];
			yx1 += x1matrix[i]*ymatrix[i];
			x12 += x1matrix[i]*x2matrix[i];
			x11 += x1matrix[i]*x1matrix[i];
			yx2 += x2matrix[i]*ymatrix[i];
			x22 += x2matrix[i]*x2matrix[i];
	}
	x1 /= N, x2 /= N, y /= N, yx1 /= N, x12 /= N, x11 /= N, yx2 /= N, x22 /= N;

	B[2][0] = ((yx2 - y*x2)*(x1*x1 - x11) + (y*x1 - yx1)*(x1*x2 - x12))/(-2*x2*x1*x12+x12*x12 + x2*x2*x11 + x22*x1*x1 - x22*x11);
	B[1][0] = (y*x1 - yx1 - B[2][0]*(x2*x1 - x12))/(x1*x1 - x11);
	B[0][0] = y - B[1][0]*x1 - B[2][0]*x2;

	return B;

}
double** mnk2 (int *x1, int *x2, double *y, int size) {

	double **x = allocationMatrix(x, size, numVar + 1);
	double **yMatrix = allocationMatrix(yMatrix, size, 1);
	double **xT = allocationMatrix(xT, numVar + 1, size);
	double **x_xT = allocationMatrix(x_xT, numVar + 1, numVar + 1);
	double **x_xT_xT = allocationMatrix(x_xT_xT, numVar + 1, size);
	double **B = allocationMatrix(B, numVar + 1, 1);

	for (int i = 0; i < size; i++) {
		x[i][0] = 1;
		x[i][1] = x1[i];
		x[i][2] = x2[i];
		yMatrix[i][0] = y[i];
	}

	transposeMatrix(x, xT, size, numVar + 1);
	multiplicationMatrix(xT, x, x_xT, numVar + 1, numVar + 1, size);
	reverseMatrix(x_xT, numVar + 1);
	multiplicationMatrix(x_xT, xT, x_xT_xT, numVar + 1, size, numVar + 1);
	multiplicationMatrix(x_xT_xT, yMatrix, B, numVar + 1, 1, size);

	return B;
}

void regression (int *x1, int *x2, double *y, int N) {
	double m1x1 = 0, m2x1 = 0, sigmaX1 = 0;
	double m1x2 = 0, m2x2 = 0, sigmaX2 = 0;

	double m1y = 0, m2y = 0, sigmaY = 0;
	double rx1y = 0, rx2y = 0;

	for (int i = 0; i < N; i++) {
		m1x1 += x1[i];
		m1x2 += x2[i];
		m1y += y[i];
	}

	m1x1 /= N, m1x2 /= N, m1y /= N;

	for (int i = 0; i < N; i++) {
		m2x1 += (x1[i] - m1x1)*(x1[i] - m1x1);
		m2x2 += (x2[i] - m1x2)*(x2[i] - m1x2);
		m2y += (y[i] - m1y)*(y[i] - m1y);
	}

	m2x1 /= (N - 1), m2x2 /= (N - 1), m2y /= (N - 1);

	sigmaX1 = pow(m2x1, 0.5), sigmaX2 = pow(m2x2, 0.5), sigmaY = pow(m2y, 0.5);

	for (int i = 0; i < N; i++) {
		rx1y += (x1[i] - m1x1)*(y[i] - m1y);
		rx2y += (x2[i] - m2x2)*(y[i] - m1y);
	}

	rx1y /= (N*sigmaX1*sigmaY);
	rx2y /= (N*sigmaX2*sigmaY);

	double b1 = rx1y*sigmaY/sigmaX1;
	double b2 = rx2y*sigmaY/sigmaX2;

	double b1x1 = 0, b2x2 = 0;
	for (int i = 0; i < N; i++) {
		b1x1 += b1*x1[i];
		b2x2 += b2*x2[i];
	}

	double b0 = m2y - b1x1 - b2x2;
	cout << b1*x1[0] + b2*x2[0] + b0;
}

int main() {
	int N = 49;
	int *x1 = new int[N];
	int *x2 = new int[N];
	double *Y = new double[N];

	double **Bmnk1 = allocationMatrix(Bmnk1, numVar + 1, 1);
	double **Bmnk2 = allocationMatrix(Bmnk2, numVar + 1, 1);


	importData(x1, x2, Y, N);
	Bmnk1 = mnk1(x1, x2, Y, N);
	Bmnk2 = mnk2 (x1, x2, Y, N);

	regression(x1, x2, Y, N);
	return 0;
}
