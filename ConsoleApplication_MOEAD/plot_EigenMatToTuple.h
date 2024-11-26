#ifndef PLOT_EIGENMATTOTUPLE
#define PLOT_EIGENMATTOTUPLE

#include <vector>
#include <iostream>
#include <Eigen/Dense>
using namespace std;


void plot_EigenMatToTuple(Eigen::MatrixXf matrix1, Eigen::MatrixXf matrix2, string titleS) {



    std::vector<std::tuple<float, float, float>> data3d;
    std::vector<std::tuple<float, float, float>> data3d_1;
    float x, y, z, a,b,c;

    
    //data3d.emplace_back(a, b ,c);
    for (int i = 0; i < matrix1.rows(); ++i) {
        x = matrix1(i, 0); // ith row, 0 col
        y = matrix1(i, 1); // ith row, 1 col
        z = matrix1(i, 2); // ith row, 2 col
        data3d.emplace_back(x, y, z); // 添加到 data3d
    }
    for (int i = 0; i < matrix2.rows(); ++i) {
        a = matrix2(i, 0); // ith row, 0 col
        b = matrix2(i, 1); // ith row, 1 col
        c = matrix2(i, 2); // ith row, 2 col
        data3d_1.emplace_back(a, b, c); // 添加到 data3d
    }

    Gnuplot gp;
    gp << "set title ' " << titleS << "' \n";

    gp << "set grid\n";
    gp << "set border 4095\n";
    gp << "splot" << gp.file1d(data3d) << " with points pointtype 3 linecolor 'red' \n";//title '3D data'
    gp << "splot" << gp.file1d(data3d_1) << " with points pointtype 15 linecolor 'blue' \n";//title '3D data'
}

#endif #pragma once
