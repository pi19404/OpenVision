#ifndef INTEGRALIMAGE_H
#define INTEGRALIMAGE_H

#include "Common/OpenCVCommon.hpp"
class IntegralImage
{
public:

Mat _integral;		//integral image
Mat _sq_integral;	//sq integral image
Mat _image;		//original image
IntegralImage();

//function too compute integral image
void compute(Mat image);
//function to compute mean value of a patch
float calcMean(Rect r);

//function to compute variance of a patch
float calcVariance(Rect r);
};


#endif // INTEGRALIMAGE_H
