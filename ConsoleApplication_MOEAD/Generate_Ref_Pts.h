#ifndef GENERATE_REF_PTS_H_INCLUDED
#define GENERATE_REF_PTS_H_INCLUDED

#include <iostream>
#include <vector>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

//Eigen::MatrixXf GenerateReferencePoints(int, int);
Eigen::MatrixXf GetFixedRowSumIntegerMatrix(int num_obj, int num_obj_dim_depart);

#endif // GENERATE_REF_PTS_H_INCLUDED

Eigen::MatrixXf  GenerateReferencePoints(int num_obj, int num_obj_dim_depart)
{

    Eigen::MatrixXf Ref_Mat;


    Ref_Mat=GetFixedRowSumIntegerMatrix(num_obj,num_obj_dim_depart);

    Ref_Mat=Ref_Mat/num_obj_dim_depart;

//    cout<<Ref_Mat;
    return Ref_Mat;

}

Eigen::MatrixXf  GetFixedRowSumIntegerMatrix(int num_obj, int RowSum)
{

    int total=(2+RowSum)*(RowSum+1)/2;
    Eigen::MatrixXf Ref_Mat(num_obj, total);
    int count=0;

    for(int e=RowSum; e>=0; --e)
    {

        Eigen::MatrixXf W(e+1,num_obj);

        for(int i=0; i<=e; ++i)
        {
            W.row(i)<<RowSum-e,i,e-i;
        }
        W.transposeInPlace();


        Ref_Mat.middleCols(count,e+1)=W;
        count+=(e+1);


    }
//    cout<<Ref_Mat;
    return Ref_Mat;

}
