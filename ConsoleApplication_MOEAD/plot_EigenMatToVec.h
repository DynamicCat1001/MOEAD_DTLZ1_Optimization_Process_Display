#ifndef PLOT_EIGENMATTOVEC
#define PLOT_EIGENMATTOVEC

#include <vector>
#include <iostream>
#include <Eigen/Dense>
using namespace std;


void plot_EigenMatToVec(MatrixXf ref_pt_matrix, string titleS) {



    vector<vector<float>> data;
    vector<float> v(3);
    float* p = &v[0];

    for (int i = 0; i < ref_pt_matrix.rows(); ++i) {
        Eigen::Map<MatrixXf>(p, 1, 3) = ref_pt_matrix.row(i);
        data.push_back(v);
        //cout << "coverted vector:" << endl;
        //cout << v[0] << " , " << v[1] << " , " << v[2] << endl;
    }

    Gnuplot gp;
    gp << "set title ' "<< titleS <<"' \n";
    //gp << "set terminal wxt 0 position 0, 0\n";
    //string AllTile = { "set terminal wxt title '" + titleS + "'" };
    //gp << AllTile;
    //gp << "set xrange [0:0.5]\nset yrange [0:0.5]\nset zrange[0:0.5]\n";
    gp << "set grid\n";
    gp << "set border 4095\n";

    //gp << "plot'-'with points\n";
    //gp.send1d(array1);

    gp << "splot" << gp.file1d(data) << "with points pointtype 3 linecolor 'blue'\n";//"with points palette\n"

}

#endif 