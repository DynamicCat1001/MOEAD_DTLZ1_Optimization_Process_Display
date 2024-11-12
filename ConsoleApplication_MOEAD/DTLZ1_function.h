#include <iostream>
#include <fstream>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
# define M_PI           3.14159265358979323846

#ifndef DTLZ1_FUNCTION_H_INCLUDED
#define DTLZ1_FUNCTION_H_INCLUDED


    MatrixXf DTLZ_1(MatrixXf x){

        float pi_=M_PI;
        int M = 3,
            k = 5; //as suggested by Deb
//        int n = M-1 + k; // Error check: the number of dimensions must be M-1+k

        MatrixXf Mat_f(1,3), Mat_g;
        MatrixXf xm;
        float const_g;

        xm = x.rightCols(k);//xm contains the last k variables

        Mat_g=pow((xm.array() - 0.5),2)-(20*pi_*(xm.array() - 0.5)).cos();//3*x.size
        const_g=100*(Mat_g.sum()+k);//3*1

//        MatrixXf MAT_W(1,3);
//        MAT_W=20*pi_*(xm.array()-0.5).cos();
//        cout<<"WW_"<<0.5*pi_<<endl;

        /* Now, computes the functions. The first and the last will be written separately to facilitate things*/

        Mat_f.col(0)=0.5*(1+const_g)*(x.col(0)*x.col(1));

        for(int i=2;i<M;++i){//M=5,M-2=3,M-3=2,M-4=1
            Mat_f(i-1)=0.5*(1+const_g)*(x.leftCols(M-i).prod())*(1-x(M-i));
        }
        Mat_f(M-1)=0.5*(1-x(0))*(1+const_g);
    //    cout<<Mat_f<<endl;
        return Mat_f;
    }


#endif // DTLZ1_FUNCTION_H_INCLUDED

/**DTZL1 DTLZ1 multi-objective function
%   This function represents a hyper-plane.
%   Using k = 5, the number of dimensions must be n = (M - 1) + k.
%   The Pareto front of this function happens at xm := x(n-k+1:end) = 0.5,
%   that is, when the last k variables are all equal to 0.5. The first
%   M-1 variables are varied to map the whole Pareto front. Of course, this
%   is not the easiest thing to do for more than M = 3 objectives. However,
%   the hyper-surface is a hyper-plane satisfying the equation
%      f1 + f2 + f3 + ... + fm = 0.5
%
%   Syntax:
%      f = dtzl1(x, M)
%
%   Input arguments:
%      x: a n x mu matrix with mu points and n dimensions
%      M: a scalar with the number of objectives
%
%   Output argument:
%      f: a m x mu matrix with mu points and their m objectives computed at
%         the input
*/
