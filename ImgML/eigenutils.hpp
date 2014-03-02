#ifndef EIGENUTILS_HPP
#define EIGENUTILS_HPP

#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
using namespace Eigen;

class EigenUtils
{
public:

    static MatrixXf setData(cv::Mat &v)
    {
        Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,v.rows,v.cols) ;
        MatrixXf  ref=mappedMat;
        return ref;

    }


};

#endif // EIGENUTILS_HPP
