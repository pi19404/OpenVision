#ifndef POLYBASIS_HPP
#define POLYBASIS_HPP
#include "Common/OpenCVCommon.hpp"
#include "ImgProc/convolution.hpp"
#include "ImgProc/channelfilter.hpp"
//the class implements methods to compute the polynomial basis
//1,x,y,x^2,y^2,xy for a path of image
class polyBasis
{
public:
    polyBasis();
    //class to perform separable symmetric/anti-symmetric filtering
    SeperableSConvolution filter;
    //class to perform channel filtering
    channelFilter cfilter;


    //function to computer seperable filters
    //for polynomial basis,size provides size of neighborhood over which the
    //basis is computed
    void computeSeparableFilters(int size)
    {
        Mat x;
        x.create(1,size,CV_32FC1);
        for (int i = 0;i<size; i++) {
            x.at<float>(0,i) = i-floor(size/2);
        }

        vector<Mat> kc,kc1;
        kc.resize(6);
        kc1.resize(6);

        Mat f,f1;
        f=Mat(size,1,CV_32FC(6));
        f1=Mat(1,size,CV_32FC(6));
        cv::split(f,kc);
        cv::split(f1,kc1);


        Mat _x;
        x.copyTo(_x);
        Mat x1=_x.row(0);
        Mat k=cv::getGaussianKernel(size,1,CV_32F);
        Mat k1;
        k.copyTo(k1);
        k1.setTo(cv::Scalar::all(1));




        fc=cv::createSeparableLinearFilter(CV_32F,CV_32F,k,k.t(),Point(-1,-1),0,cv::BORDER_REPLICATE);

        filter.setKernel(k,k,true,true);
        cv::multiply(k,k1,k1);
        cv::multiply(x1,k.t(),x1);

//        fx=cv::createSeparableLinearFilter(CV_32F,CV_32F,k1.t(),x1.t(),Point(-1,-1),0,cv::BORDER_REPLICATE);
//        fy=cv::createSeparableLinearFilter(CV_32F,CV_32F,x1.t(),k1.t(),Point(-1,-1),0,cv::BORDER_REPLICATE);

        filter.setKernel(k1,x1,true,false);
        filter.setKernel(x1,k1,false,true);


        x.copyTo(_x);
        x1=_x.row(0);
        cv::multiply(x1,x1,x1);
        cv::multiply(x1,k.t(),x1);
//        fxx=cv::createSeparableLinearFilter(CV_32F,CV_32F,k1.t(),x1.t(),Point(-1,-1),0,cv::BORDER_REPLICATE);
//        fyy=cv::createSeparableLinearFilter(CV_32F,CV_32F,x1.t(),k1.t(),Point(-1,-1),0,cv::BORDER_REPLICATE);

        filter.setKernel(k1,x1,true,true);
        filter.setKernel(x1,k1,true,true);

        x.copyTo(_x);
        x1=_x.row(0);
        //cv::flip(x1,x1,1);
        cv::multiply(x1,k.t(),k);
        k1=k.t();
//        fxy=cv::createSeparableLinearFilter(CV_32F,CV_32F,k1,k,Point(-1,-1),0,cv::BORDER_REPLICATE);


        filter.setKernel(k1,k,false,false);







    }

    //main function that computes the polynomial basis filters
    void polyBasis(Mat &A,Mat x,Mat y,Mat xx,Mat yy,Mat xy,Mat kernel)
    {
        Mat tmp;
        A=Mat(6,6,CV_32FC1);
        A.setTo(cv::Scalar::all(0));
        Scalar s;
        x.copyTo(tmp);
        tmp.setTo(cv::Scalar::all(1));


        cv::multiply(tmp,kernel,tmp);


        s=cv::mean(tmp);
        A.at<float>(0,0)=s[0]*tmp.rows*tmp.cols;;

        cv::multiply(xx,kernel,tmp);
        s=cv::mean(tmp);
        A.at<float>(1,1)=s[0]*tmp.rows*tmp.cols;
        A.at<float>(2,2)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(0,3)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(0,4)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(3,0)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(4,0)=s[0]*tmp.rows*tmp.cols;;

        cv::multiply(xx,xx,tmp);
        cv::multiply(tmp,kernel,tmp);
        s=cv::mean(tmp);
        A.at<float>(3,3)=s[0]*tmp.rows*tmp.cols;
        A.at<float>(4,4)=s[0]*tmp.rows*tmp.cols;


        cv::multiply(xx,yy,tmp);
        cv::multiply(tmp,kernel,tmp);
        s=cv::mean(tmp);
        A.at<float>(5,5)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(3,4)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(4,3)=s[0]*tmp.rows*tmp.cols;;


        Mat mask;
        cv::threshold(A,mask,0.001,1,CV_THRESH_BINARY);
        cv::multiply(A,mask,A);


        A=A.inv(DECOMP_CHOLESKY);


    }




};

#endif // POLYBASIS_HPP
