#ifndef PLOT_DVECTOVEC
#define PLOT_DVECTOVEC

#include <vector>
#include <iostream>
#include <Eigen/Dense>
using namespace std;


void plot_DVecToVec(vector<vector<float>> data, string titleS) {
 

    Gnuplot gp;
    gp << "set title 'vecPlot: "<< titleS <<"' \n";
    //gp << "set xrange [0:0.5]\nset yrange [0:0.5]\nset zrange[0:0.5]\n";
    gp << "set grid\n";
    gp << "set border 4095\n";


    gp << "splot" << gp.file1d(data) << "with points\n";

}

#endif #pragma once
