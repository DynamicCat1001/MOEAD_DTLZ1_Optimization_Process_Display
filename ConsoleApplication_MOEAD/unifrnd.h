#ifndef UNFIRND_H_INCLUDED
#define UNFIRND_H_INCLUDED

#include <iostream>
#include <random>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

#endif // UNFIRND_H_INCLUDED

MatrixXf unifrnd(float VarMin, float VarMax, int VarSize)
{


    MatrixXf random_num(1,VarSize);

    std::random_device rd;
    std::mt19937 generator( rd() );
    /* normal distribution*/
    std::uniform_real_distribution<float> distribution(VarMin, VarMax);

    /* generate normal distribution random nums */
    for (int i=0;i<VarSize;++i){
       random_num(i) = distribution(generator);
    }

//    std::cout << "position = " << random_num << endl;
    return random_num;

}
