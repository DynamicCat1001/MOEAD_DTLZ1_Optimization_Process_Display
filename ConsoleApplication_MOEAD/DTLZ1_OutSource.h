/** https://github.com/RyuichiHashimoto/NSGA-II_cpp_version/blob/master/DTLZ1.cpp */
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
# define M_PI           3.14159265358979323846

#ifndef DTLZ1_OUTSOURCE
#define DTLZ1_OUTSOURCE
MatrixXf DTLZ1_OutSource(MatrixXf solution) {

	int numberOfObjective_ = 3;
	int numberOfValue_ = solution.cols();
	MatrixXf ObjectiveValue(1,3);

	float* x = new float[solution.cols()];
	float* f = new float[3];
	int k = numberOfValue_ - numberOfObjective_ + 1;//5

	for (int i = 0; i < numberOfValue_; i++)
		x[i] = solution(0,i);

	float g = 0.0;
	for (int i = numberOfValue_ - k; i < numberOfValue_; i++)
		g += (x[i] - 0.5) * (x[i] - 0.5) - cos(20.0 * M_PI * (x[i] - 0.5));

	g = 100 * (k + g);
	for (int i = 0; i < numberOfObjective_; i++)
		f[i] = (1.0 + g) * 0.5;

	for (int i = 0; i < numberOfObjective_; i++) {
		for (int j = 0; j < numberOfObjective_ - (i + 1); j++)
			f[i] *= x[j];
		if (i != 0) {
			int aux = numberOfObjective_ - (i + 1);
			f[i] *= 1 - x[aux];
		} //if
	}//for

	for (int i = 0; i < numberOfObjective_; i++)
		ObjectiveValue(0, i) = f[i];
		//ObjectiveValue.col(i).array() = f[i];
	

	delete[] x;
	delete[] f;

	return ObjectiveValue;
} // evaluate

#endif
