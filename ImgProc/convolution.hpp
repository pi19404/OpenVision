#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "Common/OpenCVCommon.hpp"
//class for seperable convolution for float images
class SeperableSConvolution
{
public:
    SeperableSConvolution();
    vector<Mat> rowk;//vector of row filter
    vector<Mat> colk;//vector of column filters
    vector<bool> rsymmetric; //info if filter is symmetric or not
    vector<bool> csymmetric; //info if filter is symmetric or not
    int N;//number of channels in the output image

    //function to set the filter parameters
    void setKernel(Mat row,Mat col,bool symm,bool symm1);
    //function to clear the filter parameters
    void clearKernel();




    //apply a set of seperable symmetric/anti-symmetric filters on
    //the source image and return a multi channel destination image
    ////function to perform seperable filtering
    void apply(Mat &src,Mat &dst);


};

#endif // CONVOLUTION_H
