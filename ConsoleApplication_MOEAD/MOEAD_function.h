#ifndef MOEAD_FUNCTION_H_INCLUDED
#define MOEAD_FUNCTION_H_INCLUDED

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm> //for std::shuffle as randsample in matlab
#include <cstdlib> //rand()
#include <ctime> //time()
#include <Eigen/Dense>
#include <type_traits>//for isfield>>has_cost
#include <memory>//std::unique_ptrb
#include <math.h>

#include "DTLZ1_Para.h"
#include "DTLZ1_function.h"
#include "DTLZ1_OutSource.h"//---------------------------------Testing dtlz1 function code
#include "unifrnd.h"
#include "factorial.h"
#include "randsample.h"
#include "plot_EigenMatToVec.h"
#include "plot_DVecToVec.h"
using namespace std;
using namespace Eigen;

/**pre-define variable------------------------------------------------------------------------------------*/

DTLZ1_Para_F MOP;
int nVar=MOP.objfun_dim;          // Number of Decision Variables
float VarMin = MOP.searchspaceLB(0);//MOP.searchspaceLB(0);         // Decision Variables Lower Bound
float VarMax = MOP.searchspaceUB(0);//MOP.searchspaceUB(0);         // Decision Variables Upper Bound
int nObj=MOP.nobjfun;
float sigma=0.1*(VarMax-VarMin);//0.1 ??
int CrsOver_T=20; // from NSGAIII (Deb) =neighbor size

/**pre-define function------------------------------------------------------------------------------------*/

class crossover_params_class
{
public:
    int number=20;
    float gamma=0.5; // from NSGAIII (Deb)
    float VarMin_cross=0;
    float VarMax_cross=1;
};
class mutation_params_class
{
public:
    float number=20;
    float possibility=0;
};

crossover_params_class Crossover_params;
mutation_params_class Mutation_params;

struct empty_Subproblem
{
    MatrixXf lambda;
    MatrixXf Neighbors;
};

struct Array_w_idx
{
    MatrixXf Val_;
    MatrixXf idx_;
};

class empty_individual_class
{
public:
    Eigen::MatrixXf Position;
    Eigen::MatrixXf Cost;
    float g;
    bool IsDominated;

    empty_individual_class(int nVar, int nObj)
    {
        Position = Eigen::MatrixXf::Zero(1, nVar);
        Cost = Eigen::MatrixXf::Zero(1, nObj);
        g=0.0f;
        IsDominated=true;
    }
};

/** func. pre-declaration*/
Eigen::Array<empty_Subproblem, 1, Eigen::Dynamic> CreateSubProblems(int nObj, int nPop, int T);

Array_w_idx sort_idx(MatrixXf unsort_arr);

Eigen::MatrixXf pdist2(const Eigen::MatrixXf& A, const Eigen::MatrixXf& B);
float DecomposedCost(empty_individual_class, MatrixXf, MatrixXf );
void DetermineDomination(std::vector<empty_individual_class>&);
void SortDominatedPop(std::vector<empty_individual_class>&, std::vector<empty_individual_class>& );
void Crossover_Func(Eigen::MatrixXf&, const Eigen::MatrixXf, const Eigen::MatrixXf);
void c_test(const Eigen::MatrixXf);
void Mutate_Func(Eigen::MatrixXf& y_Position, Eigen::MatrixXf x);
void findlimits(Eigen::MatrixXf &pop_Position, Eigen::MatrixXf Lb, Eigen::MatrixXf Ub);
void PlotPop(std::vector<empty_individual_class> pop, string);



#endif // MOEAD_FUNCTION_H_INCLUDED



/**Main MOEAD function-----------------------------------------------------------------------------------------*/
//std::vector<empty_individual_class> MOEAD_function(DTLZ1_Para_F MOP)
vector<Matrix<float, Eigen::Dynamic, 3>> MOEAD_function(DTLZ1_Para_F MOP)
{

    // Problem Definition

    MatrixXf(*CostFunction)(MatrixXf)=DTLZ_1;
    vector<Matrix<float, Eigen::Dynamic, 3>> Storage_Cost;


    Eigen::MatrixXf Lb(1,MOP.nobjfun), Ub(1,MOP.nobjfun);
    Lb=MOP.searchspaceLB;
    Ub=MOP.searchspaceUB;


    // MOEA/D Settings
    MOP.nRep=round(1.5*(pow(MOP.nbox, MOP.nobjfun-1))/(factorial(MOP.nobjfun-1)));//=469


    Crossover_params.VarMin_cross=VarMin;
    Crossover_params.VarMax_cross=VarMax;
    Mutation_params.possibility=1.0f/nVar;//1 must be float so that divide work properly.

    /** Initialization*/
    // Create Sub-problems
    Eigen::Array<empty_Subproblem, 1, Eigen::Dynamic> sp=CreateSubProblems(nObj,MOP.nPop,CrsOver_T);


    // Empty Individual
    empty_individual_class empty_individual(nVar,nObj);
    /** Initialize Goal Point*/
    MatrixXf z_min_pt=MatrixXf::Zero(1,nObj);//min. pt.

    /** Create Initial Population*/
    std::vector<empty_individual_class> pop, non_dominate_pop;
    pop.resize(MOP.nPop, empty_individual_class(nVar, nObj));



    for(int i=0; i<MOP.nPop; ++i)
    {
        pop[i].Position=unifrnd(VarMin,VarMax,nVar);
        
    }


//    for(int i=0; i<round(MOP.nPop*0.1); ++i)
//    {
//        pop[i].Position=MOP.Position.row(i);//had been calculate in Matlab_DTLZ1
//    }
    
    MatrixXf Cost2Plot(MOP.nPop, nObj);
    //MatrixXf Cost2Plot2(MOP.nPop, nObj);

    for(int i=0; i<MOP.nPop; ++i)
    {
        pop[i].Cost=CostFunction(pop[i].Position);
        z_min_pt=z_min_pt.eval().cwiseMin(pop[i].Cost);
        Cost2Plot.row(i) = pop[i].Cost;

    }
    plot_EigenMatToVec(Cost2Plot, "Cost1");
    //plot_EigenMatToVec(Cost2Plot2, "Cost2");


    for(int i=0; i<MOP.nPop; ++i)
    {
        pop[i].g=DecomposedCost(pop[i], z_min_pt, sp(i).lambda);

    }

    

    // Determine Population Domination Status
    DetermineDomination(pop);

    
    //Initialize Estimated Pareto Front
    std::vector<empty_individual_class> Elite_Pop;

    SortDominatedPop(pop,Elite_Pop);

    //################//
    //PlotPop(pop, "After Initialization");

    // Main Loop
    int CrsOverRand, j0, j1;

    empty_individual_class y(nVar, nObj);
    MatrixXf p0(1, nVar),p1(1, nVar);


    Matrix<int, 1, Eigen::Dynamic> vec(1,CrsOver_T);

    std::random_device rd;
    srand(time(NULL));

    for(int t=0; t<CrsOver_T; ++t)
    {
        vec(t)=t;
    }
    

    for (int it=0; it<MOP.MaxIt; ++it) //MOP.MaxIt
    {
        std::cout<<"MOP.MaxIt in MOEAD"<<it<<endl;
//################//
        //PlotPop(pop, { "Before Optim iter " + std::to_string(it) + ":" });
        for(int i=0; i<MOP.nPop; ++i)//MOP.nPop
        {
             //cout<<"MOP nPop_"<< i<<"Reproduction (Crossover)"<<endl;
            

            CrsOverRand=rand()%(Crossover_params.number)+1;  //(rand() % static_cast<int>(Crossover_params.number + 1));//0~20
            //cout<<"_CrsOver:"<<CrsOverRand<<endl;
            y.Position=MatrixXf::Constant(1,nVar,0);
            //cout << i<<"_th_neighbor: "<< sp(i).Neighbors << endl;
            for (int nCrossover_iter=0; nCrossover_iter<CrsOverRand; ++nCrossover_iter)//CrsOverRand
            {

                std::shuffle(vec.begin(), vec.end(), std::default_random_engine(rd()));

                

//                cout<<"iter:"<<nCrossover_iter<<endl;
                j0=sp(i).Neighbors(vec(0));
                //cout<<"j0 : "<<j0;

                p0 = pop[j0].Position;//memory issue??
                //cout<<" , p0:"<<p0<<endl;

                j1=sp(i).Neighbors(vec(1));
                //cout<<"j1 : "<< j1;
                p1 = pop[j1].Position;
                //cout<<", p1:"<<p1<<endl;
                Crossover_Func(y.Position, p0,p1);

//                cout<<"y Position:"<<y.Position.rows()<<y.Position.cols()<<"_:"<<y.Position<<endl;
                y.Cost=CostFunction(y.Position);
                //cout<<"y Cost:"<<y.Cost<<endl;
                z_min_pt=z_min_pt.eval().cwiseMin(y.Cost);
//              //value of z pt is TBC--------------------------------------------------------!!!
            }

            int sp_N;
            for (int j=0; j<sp(0).Neighbors.cols(); ++j)//neighbor
            {

                sp_N=sp(i).Neighbors(j);
                //                cout<<"Neighbor_it  "<<j<<", sp_N  "<<sp_N<<endl;

                y.g=DecomposedCost(y, z_min_pt,sp(sp_N).lambda);//the ith sp,the jth Neighbors
                //                cout<<"g:"<<y->g<<endl;
                if(y.g <= pop[sp_N].g)
                {
//                                pop(sp_N)=std::unique_ptr<empty_individual_class>(new empty_individual_class(y)));
                    pop[sp_N]=y;
                }
            }
//################//
            //if(i == (MOP.nPop-1)) PlotPop(pop, { "AfterCrossOver" + std::to_string(it) + ":"});

            // cout<<"Reproduction (Mutaion)"<<endl;
            //ex. pop(i)->g=DecomposedCost(pop(i), z_min_pt, sp(i).lambda);


            for (int nMutation_iter=0; nMutation_iter<Mutation_params.number; ++nMutation_iter)//Mutation_params.number
            {

                Mutate_Func(y.Position, pop[i].Position);
                findlimits(pop[i].Position,Lb,Ub);
                y.Cost=CostFunction(y.Position);
                z_min_pt=z_min_pt.eval().cwiseMin(y.Cost);

            }

            //cout<<"sp(i).Neighbors;"<<sp(i).Neighbors;
            for (int j_Nb_Mu=0; j_Nb_Mu<sp(0).Neighbors.cols(); ++j_Nb_Mu)//neighbor
            {
                sp_N=sp(i).Neighbors(j_Nb_Mu);
                y.g=DecomposedCost(y,z_min_pt,sp(sp_N).lambda);
                if(y.g <= pop[sp_N].g)
                {
                    pop[sp_N]=y;
//                    cout<< "pop[sp_N]" << pop[sp_N]<<endl;
                }
            }
//################//
            //if (i== (MOP.nPop-1)) PlotPop(pop, {"AfterMutation" + std::to_string(it) + ":"});
        }


        // Determine Population Domination Status


        DetermineDomination(pop);
        SortDominatedPop(pop, Elite_Pop);
        MatrixXf Matrix_Cost;
        /*for(unsigned int k=0; k<pop.size(); ++k)
        {
            if(pop[k].IsDominated==false)
            {
                Elite_Pop.push_back(pop[k]);
            }
        }*/

        //in case 0 solutions
        if (Elite_Pop.size()==0)
        {
            it=0;
            continue;
        }
        else
        {
        std::cout << "Elite Pop" << Elite_Pop.size() << endl;
            DetermineDomination(Elite_Pop);

            Elite_Pop.erase( remove_if( Elite_Pop.begin(), Elite_Pop.end(), [](const empty_individual_class& sub_pop)
            {
                return sub_pop.IsDominated;
            }), Elite_Pop.end());


            if((int)(Elite_Pop.size()) > MOP.nRep)//MOP.nRep
            {
                //cout<<"Over Elite Size: "<< Elite_Pop.size()<<endl;
                int Extra=Elite_Pop.size() - MOP.nRep;//MOP.nRep
                //cout<<"Elite Size: "<< Elite_Pop.size() <<", nRep: "<<MOP.nRep <<", Extra: "<<Extra<<endl;
                VectorXi ToBeDeleted;
                ToBeDeleted=randsample(Elite_Pop.size(), Extra);
                //cout<<"ToBeDeleted"<< ToBeDeleted<<endl;

                Elite_Pop.erase(std::remove_if(Elite_Pop.begin(), Elite_Pop.end(), [&](const empty_individual_class& element) //remove_if(begin,end, lambda formula)
                {
                    int index_E=&element - &Elite_Pop[0];//calculate the index
                    return std::find(ToBeDeleted.begin(),ToBeDeleted.end(), index_E) !=ToBeDeleted.end();//confirm if indexE in ToBeDeleted
                }), Elite_Pop.end());

                //cout<<"Z"<<endl;

            }
            Matrix_Cost.resize(Elite_Pop.size(),MOP.nobjfun);
            for(int ff=0; ff<(int)Elite_Pop.size(); ++ff)
            {

                Elite_Pop[ff].Cost=CostFunction(Elite_Pop[ff].Position);
                //cout<<ff<<"_"<<Elite_Pop[ff].Position<<Elite_Pop[ff].Cost<<endl;
                Matrix_Cost.row(ff)=Elite_Pop[ff].Cost;
            }

            //cout<<"Iteration Information"<< it <<": Number of Elite Pareto Solutions ="<<Elite_Pop.size()<<endl;

        }

        Storage_Cost.push_back(Matrix_Cost);
//################//
        string title_iter = { "iter " + std::to_string(it) + " EP" };
        std::cout << title_iter << endl;
        //if(it%10==0) plot_EigenMatToVec(Matrix_Cost, title_iter);
    }
    
    
    std::cout<<": Number of Elite Pareto Solutions ="<<Elite_Pop.size()<<endl;
    
    return Storage_Cost;//Objective value of all iterations for openGL_plot 
     
}
/**function define----------------------------------------------------------------------------------------*/

//Create Subproblem
Eigen::Array<empty_Subproblem, 1, Eigen::Dynamic> CreateSubProblems(int nObj, int nPop, int T)
{

    //Create Subproblem with member lambda(dist) and Neighbor(closest pt.)
    Eigen::Array<empty_Subproblem, 1, Eigen::Dynamic> sp(1,nPop);


    MatrixXf lambda_(1, nObj);
    MatrixXf LAMBDA(nPop,nObj);
    MatrixXf Distance(nPop,nPop);
    //create pair of Val & indx
    for(int i=0; i<nPop; ++i)//nPop
    {
        lambda_=MatrixXf::Random(1, nObj).array() * 0.5 + 0.5;/**random type TBC*/
        //cout<<lambda_<<endl;

        lambda_=lambda_.array()/lambda_.norm();//Norm­
        

        sp[i].lambda=lambda_;
        LAMBDA.row(i)=lambda_;

    }

    Distance=pdist2(LAMBDA,LAMBDA);


    //Sort by idx & Val, then assign into "sp"
    for(int i=0; i<nPop; ++i)
    {
        sp[i].Neighbors.resize(1,T);
        Array_w_idx Sorted_Array=sort_idx(Distance.row(i));

        sp[i].Neighbors=Sorted_Array.idx_.middleCols(1,T);
        sp[i].Neighbors.array()-=1;

//        cout<<i<<" _Neighbor: "<<sp[i].Neighbors<<endl;
    }
    return sp;
}



Array_w_idx sort_idx(MatrixXf unsort_arr)
{
    //create array to storage Val & Indx
    Array_w_idx Arr_sorted;
    Arr_sorted.Val_.resize(unsort_arr.rows(),unsort_arr.cols());/**Assertion Error if no initialization**/
    Arr_sorted.idx_.resize(unsort_arr.rows(),unsort_arr.cols());
    //Sort unsort_arr & input to Arr_sorted.Val_ and Arr_sorted.idx_

    for(int j=0; j<unsort_arr.rows(); ++j)
    {
        Eigen::Array<pair<float, int>,1, Eigen::Dynamic >index(1,unsort_arr.cols());//1x6
        for(int i=0; i<unsort_arr.cols(); ++i) //6
        {
            index[i]=make_pair(unsort_arr(j,i),i);
        }
        std::sort(index.begin(),index.end());

        for(int i=0; i<unsort_arr.cols(); ++i)
        {
            Arr_sorted.Val_(j,i)=index[i].first;
            Arr_sorted.idx_(j,i)=index[i].second+1;
        }
    }
    return Arr_sorted;

}

Eigen::MatrixXf pdist2(const Eigen::MatrixXf& A, const Eigen::MatrixXf& B)
{
    if (B.cols() != A.cols()) std::cout << "error: Matrix column are different in pdist2!";

    int A_r= A.rows();
    int B_r = B.rows();

    MatrixXf V_dist;

    Eigen::MatrixXf C(A_r, B_r);

    for (int i = 0; i < A_r; i++)
    {
        for (int j = 0; j < B_r; j++)
        {
            V_dist=(A.row(i)-B.row(j)).array().pow(2);
            C(i, j)=(V_dist.sum());
        }
    }
    return C;
}


//DecomposedCost

float DecomposedCost(empty_individual_class individual, MatrixXf z, MatrixXf lambda)//pop(i).g=DecomposedCost(pop(i),z,sp(i).lambda);
{

    MatrixXf fx(1,lambda.cols());

    fx=individual.Cost;
    float g=(lambda.array()*abs(fx.array()-z.array())).matrix().maxCoeff();

    return g;

}



void DetermineDomination(std::vector<empty_individual_class>& pop)
{
    // Determine Domination        Cost_mat.rows()=nPop-1
    MatrixXf Cost_mat(pop.size()-1,(pop[0].Cost).cols()),  Cost_mat_origin(pop.size(),(pop[0].Cost).cols());
    MatrixXf NPOP_mat;
    Matrix<bool,Eigen::Dynamic,3> compare1(pop.size()-1,3), compare2(pop.size()-1,3);
    Matrix<int,Eigen::Dynamic,Eigen::Dynamic>compare1_int, compare2_int,rowsum_c, Ones_set;
    Ones_set.resize(pop.size()-1,1);
    Ones_set.fill(1);

    for(unsigned int i=0; i<pop.size(); ++i)
    {
        Cost_mat_origin.row(i)=pop[i].Cost;

    }

    Cost_mat=Cost_mat_origin.bottomRows(pop.size()-1);

    for(unsigned int i=0; i<pop.size(); ++i)
    {
        if(i>0)
        {
            Cost_mat.topRows(i)=Cost_mat_origin.topRows(i);//Cost_mat.rows()=nPop-1=99 ,Cost_mat saves cost except ith cost
            Cost_mat.bottomRows(pop.size()-i-1)=Cost_mat_origin.bottomRows(pop.size()-i-1);
        }


        NPOP_mat=MatrixXf::Constant(pop.size()-1,1,1);//reassign
        NPOP_mat=NPOP_mat.eval()*Cost_mat_origin.row(i);

        compare1=NPOP_mat.array() <= Cost_mat.array();//99x3
        compare1_int=compare1.cast<int>();
        rowsum_c= compare1_int.rowwise().sum();
        compare1_int.conservativeResize(pop.size(), 1);
        compare1_int=rowsum_c;


        compare2=NPOP_mat.array() < Cost_mat.array();
        compare2_int=compare2.cast<int>();
        rowsum_c= compare2_int.rowwise().sum();
        compare2_int.conservativeResize(pop.size(), 1);
        compare2_int=rowsum_c;
//if(i==nPop-1){cout<<"C2:"<<endl;cout<<compare2_int;}


//        Matrix<bool,Eigen::Dynamic,Eigen::Dynamic> diff;//just for test & simplify
//        diff= compare1_int.array()==compare2_int.array();

        if( !(compare1_int.array() <Ones_set.array() ).any() )//why not compare2 only???
        {
            if( !(compare2_int.array() <Ones_set.array() ).any() )//if one pt sum is 0 means this pt is dominated
            {
                pop[i].IsDominated=false;

            }

        }//compare 99x99 times

//        cout<<i<<"_"<<pop(i)->IsDominated<<endl;
        //To  do list: porduce random case to prove compare2 only is right

    }

}

void SortDominatedPop(std::vector<empty_individual_class>& pop, std::vector<empty_individual_class>& Elite_Pop)
{
    //std::vector<empty_individual_class> filteredElements;
    
    vector<vector<float>> array_temp;
    vector<float> v_temp(3);
    float* p = &v_temp[0];
    

    for (int i = 0; i < (int)pop.size(); ++i)
    {
        if ((pop[i].IsDominated)==false)
        {
            Elite_Pop.push_back(pop[i]);

            Eigen::Map<MatrixXf>(p, 1, 3) = pop[i].Cost;
            array_temp.push_back(v_temp);
            
        }
    }
    //if(Elite_Pop.size()!=0)plot_DVecToVec(array_temp, "EPandPopSort");

}


void Crossover_Func(Eigen::MatrixXf& y_Position, const Eigen::MatrixXf x1, const Eigen::MatrixXf x2)
{
    srand(time(NULL));

    MatrixXf min_p=MatrixXf::Constant(1,x1.cols(),Crossover_params.VarMin_cross);
    MatrixXf max_p=MatrixXf::Constant(1,x1.cols(),Crossover_params.VarMax_cross);
    // alpha=unifrnd(-gamma,1+gamma,size(x1));
    MatrixXf alpha=MatrixXf::Random(x1.rows(),x1.cols()).array() * 0.5 + 0.5;//1 by n

    y_Position=alpha.array()*x1.array()+(1-alpha.array())*x2.array();
    y_Position=y_Position.eval().cwiseMax(min_p);
    y_Position=y_Position.eval().cwiseMin(max_p);
//    cout<<"Y_after:"<<y_Position<<endl;

}

void Mutate_Func(Eigen::MatrixXf& y_Position, Eigen::MatrixXf pop_Position)//y become pop first, then pick one to mutation.
{
    //sigma=0.1

    int size_j=ceil(Mutation_params.possibility*pop_Position.cols());//=1, y position only mutate 1 item
    int mu=ceil(Mutation_params.possibility*nVar);//=1

    std::random_device rd;
    std::mt19937 generator( rd() );
    std::uniform_int_distribution<int> distribution(0, nVar-1);

    VectorXi j=VectorXi::Random(size_j);//randsample(n,k)丟k個1~n均勻分布數值  randsample(nVar,nMu)
    if(size_j==1)
    {
        j(0)=distribution(generator);
    }
    else
    {
        for(int i=0; i<mu; ++i)
        {
            j(i) = distribution(generator);
        }
    }
    y_Position=pop_Position; //y become pop first, then pick one to mutation.
//    cout<<"before: "<<y_Position<<endl;

    float randi=sigma*distribution(generator);//randn(n): nxn
    y_Position(0,j)=pop_Position(0,j).array()+randi;//[1x7][7x7]

//    cout<<"after : "<<y_Position<<endl;

}

void findlimits(Eigen::MatrixXf &pop_Position, Eigen::MatrixXf Lb, Eigen::MatrixXf Ub)
{
    // Findlimits
    // Apply the lower bound


    Eigen::MatrixXf pop_temp(1,nVar), L_float(1,nVar), U_float(1,nVar), B_remainder(1,nVar), B_Quotient(1,nVar);
    Eigen::MatrixXf A(1,nVar), B(1,nVar);

    pop_temp=pop_Position;
    Eigen::Matrix<bool, 1, 7> L_bool, U_bool;//=ns_tmp<Lb;



    L_bool=pop_temp.array() < Lb.array();
    for(int i=0; i<nVar; i++)
    {
        if(pop_temp(i)<Lb(i)) pop_temp(i) = Lb(i);
    }

    B_Quotient=abs(Lb.array()-pop_Position.array()) .cwiseQuotient(abs(Ub.array()-Lb.array()));//Quotient=A /B ;
    B_remainder=abs(Lb.array()-pop_Position.array()) - (B_Quotient.array() * (Ub.array()-Lb.array()));

    L_float=L_bool.cast<float>();

    pop_temp=pop_temp.array() + L_float.array() * B_remainder.array();
//    cout<<"after LB: "<<pop_temp<<endl;


    // Apply the upper bounds


    U_bool=pop_temp.array() < Ub.array();
    for(int i=0; i<nVar; i++)
    {
        if(pop_temp(i)>Ub(i)) pop_temp(i) = Ub(i);

    }


    B_Quotient=abs(Ub.array()-pop_Position.array()) .cwiseQuotient(abs(Ub.array()-Lb.array()));//Quotient=A /B ;
    B_remainder=abs(Ub.array()-pop_Position.array()) - (B_Quotient.array() * (Ub.array()-Lb.array()));

    U_float=U_bool.cast<float>();
    pop_temp=pop_temp.array() + L_float.array() * B_remainder.array();
//    cout<<"after UB: "<<pop_temp<<endl;


    // Update this new move
    pop_Position=pop_temp;
}

void PlotPop(std::vector<empty_individual_class> pop, string title) {

    if (pop.size() != 0) {
        vector<vector<float>> array_temp;
        vector<float> v_temp(3);
        float* p = &v_temp[0];


        for (int i = 0; i < (int)pop.size(); ++i)
        {
            Eigen::Map<MatrixXf>(p, 1, 3) = pop[i].Cost;
            array_temp.push_back(v_temp);
        }
        plot_DVecToVec(array_temp, title);
    }
    else {
        std::cout << "empty pop\n";
    }
}
