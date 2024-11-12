#ifndef IS_FIELD_H_INCLUDED
#define IS_FIELD_H_INCLUDED


#include <iostream>
#include <Eigen/Dense>
#include <type_traits>

using namespace std;
using namespace Eigen;


template<typename T>//header
bool  has_cost(const T& obj){
    return std::is_same<decltype(obj.Cost), MatrixXf>::value;
}

#endif // IS_FIELD_H_INCLUDED
