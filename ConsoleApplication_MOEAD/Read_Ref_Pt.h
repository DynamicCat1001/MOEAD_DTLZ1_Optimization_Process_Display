#ifndef READ_REF_PT_H_INCLUDED
#define READ_REF_PT_H_INCLUDED

#include <iostream>
#include <fstream>
#include <conio.h>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

Eigen::MatrixXf Read_file(int col_, int row_)//col=2500, row=3
{

    ifstream file2("DTLZ1ReferencePoints.dat");
    if (!file2)
    {
        cout << "file can't open" <<endl;
    }
    else
    {
        Eigen::MatrixXf p(col_,row_);
        Eigen::RowVector3f row_element;
        float A;
        for(int i=0; i<col_; ++i)//2500
        {
            for(int j=0; j<row_; ++j)//3 dim.
            {
                A=0;
                file2>>A;
                row_element[j]=A;
            }
            p.row(i)=row_element;
        }


        cout<<"data read finished"<<endl;

        file2.close();
        return p;
    }
}


#endif // READ_REF_PT_H_INCLUDED
