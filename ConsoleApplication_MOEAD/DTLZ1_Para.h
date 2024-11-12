#ifndef DTLZ1_Para_H
#define DTLZ1_Para_H
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include "DTLZ1_function.h"

using namespace std;
using namespace Eigen;

class DTLZ1_Para_F {
public:
    string dtlz_name = "DTLZ1";
    //    void (*func_dtlz)(MatrixXf)=DTLZ_1;
    int objfun_dim, nobjfun, MaxIt, nPop, nbox, nRep;
    MatrixXf searchspaceUB;
    MatrixXf searchspaceLB;
    MatrixXf Position;

    DTLZ1_Para_F() : objfun_dim(7), nobjfun(3), MaxIt(0),nPop(0), nbox(0) ,nRep(0),
        searchspaceUB(MatrixXf::Constant(1, objfun_dim, 1.0f)),
        searchspaceLB(MatrixXf::Constant(1, objfun_dim, 0.0f)) {}//MaxIt=1000

};



#endif // DTLZ1_H_INCLUDED
#pragma once
