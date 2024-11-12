#ifndef SLOPE_H_INCLUDED
#define SLOPE_H_INCLUDED

Eigen::MatrixXf Slope(Eigen::MatrixXf Zr_matrix, Eigen::MatrixXf ref_pt_matrix, int psdiv)
{
    Eigen::MatrixXf temp_2,slope_0,slope_1,slope_2,s_dist,min_matrix;
    int min_idxR,min_idxC;
    float min_val;

    int ps=0;
    float div_=1.00f/psdiv;
    Eigen::MatrixXf rps;
    Eigen::MatrixXf T_dist(ref_pt_matrix.rows(),ref_pt_matrix.cols());


    for(int i=0; i<Zr_matrix.cols(); ++i)
    {
        temp_2=(Zr_matrix.col(i)).transpose();//temp_2.transposeInPlace();
        slope_0=MatrixXf::Constant(ref_pt_matrix.rows(), 1,1) * (Zr_matrix.col(i)).transpose();//2500x3

        slope_1=slope_0.array()*ref_pt_matrix.array();
        slope_1 = slope_1.rowwise().sum().eval();


        slope_2=slope_0.array().square();
        slope_2 =slope_2.rowwise().sum().eval();


        s_dist=slope_1.array()/slope_2.array();

        T_dist<<s_dist,s_dist,s_dist;
        T_dist=T_dist.array()*slope_0.array();

        min_matrix= (ref_pt_matrix-T_dist).array().pow(2);
        min_matrix=(min_matrix.rowwise().sum().eval()).array().sqrt();//sqrt(sum((reference_ps-t).^2,2));

        min_val = min_matrix.minCoeff(&min_idxR,&min_idxC);
//        cout<<"1_over_15: "<<div_<<", min val:"<<min_val<<endl;

        if(min_val<div_)
        {
            rps.conservativeResize(ps+1,3);
            rps.row(ps)=ref_pt_matrix.row(min_idxR);
            ps++;
        }
    }
//    cout<<ps<<endl;
//    cout<<rps;

    ref_pt_matrix.conservativeResize(rps.rows(), rps.cols());
    ref_pt_matrix=rps;

    return ref_pt_matrix;
}


#endif // SLOPE_H_INCLUDED
