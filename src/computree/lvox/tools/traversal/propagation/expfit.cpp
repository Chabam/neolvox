#include <math.h>
#include <cmath>
//#include <iostream>
#include "opencv2/core/cuda.hpp"
#include <QStack>
#include "tools/lvox3_errorcode.h"
#include <Eigen/Dense>
#include <QObject>
#include "tools/lvox3_gridtype.h"

void fit(double paramFit[3], Eigen::MatrixXd data, int Ndata){

    std::vector <double> S(Ndata);
    S[0] = 0;

    double Sxsqr = 0;
    double SxS = 0;
    double SSksqr = 0;
    double Syx = 0;
    double SyS = 0;

    for(int i = 1;i<Ndata;++i)
    {
        S[i] = S[i-1] - (data(i ,1)-data(i-1 ,1))*(data(i ,0)-data(i-1 ,0));
        Sxsqr += pow(data(i ,0)-data(0 ,0) ,2);
        SxS += (data(i ,0)-data(0 ,0))*S[i];
        SSksqr += pow(S[i],2);
        Syx += (data(i ,0)-data(0 ,0))*(data(i ,1)-data(0 ,1));
        SyS += S[i]*(data(i ,1)-data(0 ,1));
    }

    Eigen::Vector2d out;
    Eigen::Matrix2d L(2,2);
    Eigen::Vector2d R;

    L << Sxsqr, SxS,   SxS, SSksqr;
    R << Syx, SyS;

    out = L.fullPivLu().solve(R);

    //VOIR assignation
    double c1 = static_cast<double>(out(1));
    double theta = 0;
    double Stheta = 0;
    double Sytheta = 0;
    double Sy = 0;
    double Stheta2 = 0;

    for(int i = 0; i<Ndata;++i)
    {
        theta = exp(c1*data(i,0));
        Stheta += theta;
        Sytheta += theta*data(i,1);
        Stheta2 += pow(theta,2);
        Sy += data(i,1);
    }

    Eigen::Vector2d out2;
    Eigen::Matrix2d L2(2,2);
    Eigen::Vector2d R2;

    L << Ndata, Stheta, Stheta, Stheta2;
    R << Sy, Sytheta;

    out2 = L2.fullPivLu().solve(R2);

    paramFit[1] = out2(1);//Sill
    paramFit[0] = out2(0);//Nugget
    paramFit[2] = c1;//Range
}

