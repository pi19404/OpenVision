#include "integralImage.h"

IntegralImage::IntegralImage()
{
}

void IntegralImage::compute(Mat image)
{
image.copyTo(_image);
cv::integral(_image,_integral,_sq_integral);
}

float IntegralImage::calcMean(Rect r)
{
           int width=_integral.cols;
           int height=_integral.rows;
           unsigned int *ii1 =(unsigned int *)_integral.data;
           int a=r.x+(r.y*(width));
           int b=(r.x+r.width)+(r.y*(width));
           int c=r.x+((r.y+r.height)*(width));
           int d=(r.x+r.width)+(r.y+r.height)*(width);
           float mx=ii1[a]+ii1[d]-ii1[b]-ii1[c];
           mx=mx/(r.width*r.height);
       return mx;

}

float IntegralImage::calcVariance(Rect r)
{
        int width=_integral.cols;
        int height=_integral.rows;
           int a=r.x+(r.y*width);
           int b=(r.x+r.width)+(r.y*width);
           int c=r.x+((r.y+r.height)*width);
           int d=(r.x+r.width)+(r.y+r.height)*width;
           float mx=calcMean(r);
           double *ii2 = (double *)_sq_integral.data;
           float mx2=ii2[a]+ii2[d]-ii2[b]-ii2[c];
           mx2=mx2/(r.width*r.height);
           mx2=mx2-(mx*mx);
           return mx2;
}


