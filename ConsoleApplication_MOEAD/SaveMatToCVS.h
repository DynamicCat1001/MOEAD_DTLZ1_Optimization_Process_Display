#ifndef SAVEMATTOCVS
#define SAVEMATTOCVS

#include <iostream>
#include <Eigen/Dense>
using namespace std;

void SaveMatToCVS(MatrixXf M1 , MatrixXf M2, Matrix<bool, 99, 3> C1, Matrix<int, 99, 1> C1_int, Matrix<bool, 99, 3> C2, Matrix<int, 99, 1> C2_int ,string title) {
    ofstream file2;
    std::string AAA = title+".csv";
    file2.open(AAA, ios::out | ios::trunc);
    //file2.trunc;
    if (file2) {

        for (int ww = 0; ww < M1.rows(); ++ww) {
            for (int qq = 0; qq < M1.cols(); ++qq) {
                file2 << M1.row(ww).col(qq) << "," ;
            }
            for (int qq = 0; qq < M1.cols(); ++qq) {
                file2 << M2.row(ww).col(qq) << ",";
            }
            for (int qq = 0; qq < C1.cols(); ++qq) {
                file2 << C1.row(ww).col(qq) << ",";
            }
            file2 << C1_int.row(ww).col(0) << ",";
            for (int qq = 0; qq < C2.cols(); ++qq) {
                file2 << C2.row(ww).col(qq) << ",";
            }
            file2 << C2_int.row(ww).col(0);
            
            file2 << endl;
        }
    }
    file2.close();

}

#endif SAVECVS
