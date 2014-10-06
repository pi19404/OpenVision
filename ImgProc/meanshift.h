#ifndef MEANSHIFT_H
#define MEANSHIFT_H

#include "Common/OpenCVCommon.hpp"
#include "ImgProc/Histogram.hpp"

using namespace ocv;
namespace ocv
{
class ocvmeanShift
{
public:
    //constructor for mean shift
    ocvmeanShift();
    Histogram h;
private:

    Point2f p;
    Rect region;
    TermCriteria criteria;
public:
    //setting the object model for mean shift algorithm
    void buildModel(Mat image,Rect rect);

    //man function for mean shift algorithm
    Rect run(Mat image);


};
}
#endif // MEANSHIFT_H
