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

        Mat x,y,xx,yy,xy,c;
        //get -3,-2,-1,0,1,2,3 for size=7
        //Mat x,_x,x1; //hold the polynomial basis
        Mat _x,x1;
        Mat k1,k; //hold the gaussian weighting function

        //vector to hold the row and columns seperable kernels
        vector<Mat> kc,kc1;

        //multichannel kernel whose channels represent
        //the kernels
        Mat f,f1;

        x.create(1,size,CV_32FC1);
        for (int i = 0;i<size; i++) {
            x.at<float>(0,i) = i-floor(size/2);
        }

        //allocating space for row and column kernels
        kc.resize(6);
        kc1.resize(6);


        //allocating the multi-channel kernel
        f=Mat(size,1,CV_32FC(6));
        f1=Mat(1,size,CV_32FC(6));

        //splitting the channels of the image
        cv::split(f,kc);
        cv::split(f1,kc1);


        x.copyTo(_x);
        x1=_x.row(0);

        //get the gaussian weighing function
        k=cv::getGaussianKernel(size,1,CV_32F);


        //kernel for c*gaussians
        k.copyTo(c);
        //c.setTo(cv::Scalar::all(1));

        filter.setKernel(c,k,true,true);

        //kernels for x*gaussian and y*gaussian
        //cv::multiply(k,k1,k1);
        cv::multiply(x1,k.t(),x1);
        filter.setKernel(k,x1,true,false);
        filter.setKernel(x1,k,false,true);

        //kernels for x^2*gaussian and y^*gaussian
        x.copyTo(_x);
        x1=_x.row(0);

        cv::multiply(x1,x1,x1);
        cv::multiply(x1,k.t(),x1);
        filter.setKernel(k,x1,true,true);
        filter.setKernel(x1,k,true,true);

        //kernel for xy*gaussian
        x.copyTo(_x);
        x1=_x.row(0);
        cv::multiply(x1,k.t(),x1);
        filter.setKernel(x1,x1.t(),false,false);

    }

    void meshGrid(Mat &x,Mat &y,int size)
    {
        x.create(size,size,CV_32FC1);
        y.create(size,size,CV_32FC1);
        for (int i = 0;i<size; i++) {
            x.at<float>(0,i) = i-floor(size/2);
            y.at<float>(i,0) = i-floor(size/2);
        }


        x = repeat(x.row(0),size,1);
        y = repeat(y.col(0),1,size);


    }

    void gaussian2Dkernel(Mat &kernel,int size)
    {
        Mat x,y;
        meshGrid(x,y,size);
        Mat k=cv::getGaussianKernel(size,1,CV_32F);
        k=k.t();
        cv::mulTransposed(k,kernel,true);



    }


    //main function that computes the polynomial basis filters
    //matirx for least square estimation,1,x^2,x^4 is required to
    //construct the matrix
    //polynomial basis projection required are for 1,x,y,X^2,y^2,
    //xy,x^4,y^4,x^2y^2
    void computeBasisMatrix(int size,Mat &A)
    {


        Mat tmp,x,y,xx,xy,yy,kernel;

        //generate a sizexsize grid of values
        //x matrix contains basis function for x
        meshGrid(x,y,size);
        //compute the basis function for x^2
        cv::pow(x,2,xx);
        cv::pow(y,2,yy);
        //compute the basis function for xy
        cv::multiply(x,y,xy);

        //compute gaussian 2D kernel
        gaussian2Dkernel(kernel,7);
        //compute the basis matrix


        A=Mat(6,6,CV_32FC1);
        A.setTo(cv::Scalar::all(0));
        Scalar s;
        x.copyTo(tmp);
        tmp.setTo(cv::Scalar::all(1));

        //compute basis matrix for 1*g
        //weighing basis function by gaussian filter
        cv::multiply(tmp,kernel,tmp);
        //computing the projection
        s=cv::mean(tmp);
        A.at<float>(0,0)=s[0]*tmp.rows*tmp.cols;;

        //weigh the x^2 basis by the gaussian function
        cv::multiply(xx,kernel,tmp);
        //compute the projection
        s=cv::mean(tmp);

        //due to symmetry following values are the same
        // [ x        e  e    ]
        // [    y             ]
        // [       y          ]
        // [ e        z       ]
        // [ e           z    ]
        // [                u ]

        A.at<float>(1,1)=s[0]*tmp.rows*tmp.cols;
        A.at<float>(2,2)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(0,3)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(0,4)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(3,0)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(4,0)=s[0]*tmp.rows*tmp.cols;;

        //computing the x^4*g projection
        cv::multiply(xx,xx,tmp);
        cv::multiply(tmp,kernel,tmp);
        s=cv::mean(tmp);

        A.at<float>(3,3)=s[0]*tmp.rows*tmp.cols;
        A.at<float>(4,4)=s[0]*tmp.rows*tmp.cols;

        //computing x^2*y^2*g projection
        cv::multiply(xx,yy,tmp);
        cv::multiply(tmp,kernel,tmp);
        s=cv::mean(tmp);
        A.at<float>(5,5)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(3,4)=s[0]*tmp.rows*tmp.cols;;
        A.at<float>(4,3)=s[0]*tmp.rows*tmp.cols;;


        //threshold the vallues
        Mat mask;
        cv::threshold(A,mask,0.001,1,CV_THRESH_BINARY);
        cv::multiply(A,mask,A);

        //compute the inverse of the matrix
        A=A.inv(DECOMP_CHOLESKY);


    }



    //main function to be called to precompute the filter coefficients
    //and vandermore matrix
    void computeFilters(int size)
    {
        Mat G;
        computeBasisMatrix(size,G);
        //copy the output matrix to channel filter
        G.copyTo(cfilter.kernel);
        //compute the seperable filters
        computeSeparableFilters(size);
    }


};

#endif // POLYBASIS_HPP
