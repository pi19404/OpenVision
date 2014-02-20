#ifndef GAUSSIAN_HPP
#define GAUSSIAN_HPP

#include "Common/OpenCVCommon.hpp"
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
using namespace Eigen;


#define PI CV_PI
namespace ocv
{


class Gaussian
{

    MatrixXf _mu;
    MatrixXf _Sigma;
    MatrixXf _invsigma;
    MatrixXf _LI;
    float _scale;
    float _det;
    int _dim;



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


    //function to set the value of mean vector
    void setMean(vector<float> &v);
    void setMean(Mat &v);

    //function to initialize eigen matrix datastructure with opencv Mat
    MatrixXf setData(cv::Mat &v);

    //function to set the value of covariance matrix
    void setSigma(cv::Mat &v);

};
}
using namespace ocv;
#endif // GAUSSIAN_HPP
