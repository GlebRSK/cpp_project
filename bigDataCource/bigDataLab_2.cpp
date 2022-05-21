//============================================================================
// Name        : bigDataLab_2.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <vector>

using namespace std;

int const numParametrs = 7;

class variables {
public:
	int size;
	double *array;
	double m1;
	double m2;
	double sigma;

public:
	variables (const double *_array, int _size) {
		size = _size;
		array = new double[size];
		for (int i = 0; i < size; i++) {
			array[i] = _array[i];
		}
	}

	void parametrs () {
		m1 = 0;
		for (int i = 0; i < size; i++) {
			m1 += array[i];
		}

		m1 /= size;

		m2 = 0;
		for (int i = 0; i < size; i++) {
			m2 += (array[i] - m1)*(array[i] - m1);
		}

		m2 /= (size - 1);
		sigma = pow(m2, 0.5);
	}
	
	void print() {
		for (int i = 0; i < size; i++) {
			cout << array[i] << " ";
		}
		cout << endl;
	}
		

	~variables() {
		delete[] array;
	}


};

void correlation_coefficient0 (variables *x, variables *y, double *r) {
	double m1x = x->m1, m1y = y->m1;

	for (int i = 0; i < x->size; i++) {
		*r += (x->array[i] - m1x)*(y->array[i] - m1y);
	}
	
	*r /= ((x->size - 1)*x->sigma*y->sigma);
	cout << x->sigma << " " << y->sigma << " " << *r << endl;
}

void correlation_coefficient1(variables *x1, variables *x2, variables *y, double *r, double *arrayR, int i, int imax) {
	double rx1x2 = 0;
	correlation_coefficient0(x1, x2, &rx1x2);

	*r = (arrayR[i] - arrayR[imax]*rx1x2)/(pow( (1 - arrayR[imax]*arrayR[imax])*(1 - rx1x2*rx1x2), 0.5));
}

void correlation_coefficient2(variables *x1, variables *x2, variables *y, double *r, double *arrayR, int i, int imax) {
	double rx = 0;
	
}

double** allocationMatrix (double **x, int dim1, int dim2) {
	x = new double*[dim1];
	for (int i = 0; i < dim1; i++) {
		x[i] = new double[dim2];
	}
	return x;
}

vector< variables* > adding_parametrs (vector <variables*> v, int N) {
	
	int size = v.size() - 1;
	double *x = new double[N]; 
	for (int i = 0; i < size; i++) {
		
		for (int j = i; j < size; j++) {
			
			for (int e = 0; e < N; e++) {
				x[e] = v[i]->array[e] * v[j]->array[e];
			}
			v.push_back(new variables(x, N));
		}
	}
	
	return v;
}

vector< variables* > import_data(int size) {
	vector < variables* > parametrs;
	ifstream file ("data_lab2.txt", ios::out);
	string str = "";
	int index = 0;
	double **x = allocationMatrix(x, size, numParametrs - 1);
	double *y = new double[size];
	double *current_x = new double[size];

	while(getline(file, str)) {

		string substr = "";
		int count = 0;
		for (unsigned int i = 0; i < str.length(); i++) {

			if ((int)str[i] != 9) {
				substr += str[i];
			}
			else {
				count++;

				if (count == 1) {
					index = atoi(substr.c_str());
				}
				else {
					x[index - 1][count - 2] = atoi(substr.c_str());
				}

				substr = "";
			}
		}

		y[index - 1] = atof(substr.c_str());
	}

	for (int i = 0; i < numParametrs - 1; i++) {
		for (int j = 0; j < size; j++) {
			current_x[j] =  x[j][i];
		}

		variables *memberOfClass = new variables(current_x, size);
		parametrs.push_back(memberOfClass);
	}

	variables *memberOfClass = new variables(y, size);
	parametrs.push_back(memberOfClass);
	file.close();
	return parametrs;
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


int main() {
	int N = 50;
	
	double *r2 = new double[numParametrs - 3];

	vector < variables* > parametrs;
	parametrs = import_data(N);
	parametrs = adding_parametrs(parametrs, N);
	
	for (int i = 0; i < N; i++) {
		double temp = parametrs[parametrs.size() - 1]->array[i];
		parametrs[parametrs.size() - 1]->array[i] = parametrs[numParametrs - 1]->array[i];
		parametrs[numParametrs - 1]->array[i] = temp;
	}

	for (int i = 0; i < parametrs.size(); i++) {

		parametrs[i]->parametrs();
	}

	int max_correlationIndex = 0;
	double max_correlationCoefficient = -1;

	double *r0 = new double[parametrs.size() - 1];
	double *r1 = new double[parametrs.size() - 2];

	for (int i = 0; i < parametrs.size() - 1; i++) {
		double r = 0;
		correlation_coefficient0(parametrs[i], parametrs[parametrs.size() - 1], &r);
		r0[i] = r;

		if (fabs(r0[i]) > max_correlationCoefficient) {
			max_correlationIndex = i;
			max_correlationCoefficient = fabs(r0[i]);
		}
	}
	cout << max_correlationIndex << endl;
	int max_correlationIndex1 = 0;
	max_correlationCoefficient = -1;

	for (int i = 0; i < parametrs.size() - 1; i++) {
		double r = 0;
		int j = 0;

		if (i != max_correlationIndex) {
			j++;
			correlation_coefficient1(parametrs[i], parametrs[max_correlationIndex], parametrs[parametrs.size()- 1],
					&r, r0, i, max_correlationIndex);
			r1[j] = r;

			if (fabs(r) > max_correlationCoefficient) {
				max_correlationCoefficient = fabs(r);
				max_correlationIndex1 = j;
			}
		}
	}
	cout << max_correlationCoefficient << endl;
	return 0;
}
