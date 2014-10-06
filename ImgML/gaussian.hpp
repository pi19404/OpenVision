#ifndef GAUSSIAN_HPP
#define GAUSSIAN_HPP

#include "Common/OpenCVCommon.hpp"
#include <iostream>
#include "ImgML/eigenutils.hpp"

#define PI CV_PI
namespace ocv
{


class Gaussian
{

    MatrixXd _mu;
    MatrixXd _Sigma;
    MatrixXd _invsigma;
    LLT<MatrixXd> _LI;
    double _scale;
    double _det;
    int _dim;
    double prior;



public:
    class gtype{
    public:
    enum type {TYPE_FULL, TYPE_DIAGONAL, TYPE_SPHERICAL};
    type t;
    };

    gtype t;

    Gaussian();

    void validate(float &res);

    //function to compute the pdf of multivatiate gaussian distribution
    float Prob(Mat &x);

    float logProb(Mat &x);


    //function to set the value of mean vector
    void setMean(vector<float> &v);
    void setMean(Mat &v);

    //function to initialize eigen matrix datastructure with opencv Mat
    MatrixXd setData(cv::Mat &v);

    //function to set the value of covariance matrix
    void setSigma(cv::Mat &v);

};
}
using namespace ocv;
#endif // GAUSSIAN_HPP
