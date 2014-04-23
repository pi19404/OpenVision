#ifndef EIGENUTILS_HPP
#define EIGENUTILS_HPP

#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include "Common/OpenCVCommon.hpp"
#include <iostream>
#include "ImgML/eigenutils.hpp"
using namespace Eigen;

class EigenUtils
{
public:

    static MatrixXd setData(cv::Mat &v)
    {
        //Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,v.rows,v.cols) ;
        MatrixXd  ref;
        v.convertTo(v,CV_64FC1,1.0,0);
        cv2eigen(v,ref);
        //=mappedMat;
        return ref;

    }

    static MatrixXf setDataf(cv::Mat &v)
    {
        //Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,v.rows,v.cols) ;
        MatrixXf  ref;
        //v.convertTo(v,CV_64FC1,1.0,0);
        cv2eigen(v,ref);
        //=mappedMat;
        return ref;

    }

    static float logsumexp(vector<float> work)
    {

        std::vector<float>::iterator result;
        result = std::max_element(work.begin(), work.end());
        float r=0;
        for(int i=0;i<work.size();i++)
        {
            r=r+std::exp(work[i]-*result);
        }
        r=std::log(r);
        r=r+*result;

        return r;

    }


};

#endif // EIGENUTILS_HPP
