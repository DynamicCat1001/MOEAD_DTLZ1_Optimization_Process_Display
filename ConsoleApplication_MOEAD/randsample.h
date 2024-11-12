#ifndef RANDSAMPLE_H_INCLUDED
#define RANDSAMPLE_H_INCLUDED

#include <iostream>
#include <random>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

#endif // RANDSAMPLE_H_INCLUDED

VectorXi randsample(int range, int numbers)
{

    std::random_device rd;
    std::mt19937 generator( rd() );
    std::uniform_int_distribution<int> distribution(0, range-1);//for C++ index start from zero

    VectorXi rand_Vector=VectorXi::Random(numbers);//randsample(n,k)¥ák­Ó1~n§¡¤Ã¤À¥¬¼Æ­È  randsample(nVar,nMu)
    if(numbers==1)
    {
        rand_Vector(0)=distribution(generator);
    }
    else
    {
        for(int i=0; i<numbers; ++i)
        {
            rand_Vector(i) = distribution(generator);
        }
    }
//    cout<<rand_Vector<<endl;
    return rand_Vector;
}
