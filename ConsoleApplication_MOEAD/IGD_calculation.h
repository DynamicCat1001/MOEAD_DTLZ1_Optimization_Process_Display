#ifndef IGD_CALCULATION_H_INCLUDED

#include <iostream>
#include <vector>
#include <numeric>
#include <Eigen/Dense>


using namespace std;
using namespace Eigen;
#define IGD_CALCULATION_H_INCLUDED


//void SaveToCVS(MatrixXf A, MatrixXf B, MatrixXf C, MatrixXf D, string title);

float IGD_calculation(Eigen::MatrixXf ref_pt_matrix , Eigen::Matrix<float, Eigen::Dynamic, 3> obtained_solutions) {
	float IGD=0;
    
    Eigen::MatrixXf dist_bias(obtained_solutions.rows(), obtained_solutions.cols());
    Eigen::MatrixXf Distance(obtained_solutions.rows(), 1);
	Eigen::MatrixXf reference_matrix(obtained_solutions.rows(), obtained_solutions.cols());
    vector<float> obtain_to_reference(ref_pt_matrix.rows());

    for (int i = 0; i < ref_pt_matrix.rows(); ++i) {//ref_pt_matrix.rows()
        
        reference_matrix = (MatrixXf::Ones(obtained_solutions.rows(), 1)) * ref_pt_matrix.row(i);
		
		dist_bias = obtained_solutions.array() - reference_matrix.array();
        
		Distance = ((dist_bias.array().square()).rowwise().sum()).sqrt();
        
        obtain_to_reference[i] = Distance.minCoeff();

	}
    IGD = accumulate(obtain_to_reference.begin(), obtain_to_reference.end(), 0)/ ref_pt_matrix.rows();

	return IGD;
}


//void SaveToCVS(MatrixXf A, MatrixXf B, MatrixXf C, MatrixXf D,  string title) {
//    ofstream file2;
//    std::string AAA = title + ".csv";
//    file2.open(AAA, ios::out | ios::trunc);
//    //file2.trunc;
//    if (file2) {
//
//        for (int ww = 0; ww < A.rows(); ++ww) {
//            for (int qq = 0; qq < A.cols(); ++qq) {
//                file2 << A.row(ww).col(qq) << ",";
//            }
//            for (int qq = 0; qq <B.cols(); ++qq) {
//                file2 <<B.row(ww).col(qq) << ",";
//            }
//            for (int qq = 0; qq < C.cols(); ++qq) {
//                file2 << C.row(ww).col(qq) << ",";
//            }
//            file2 << D.row(ww).col(0) << ",";
//
//            file2 << endl;
//        }
//    }
//    file2.close();
//
//}
#endif // IGD_CALCULATION_H_INCLUDED

