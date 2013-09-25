#ifndef SUBPIXELCORNER_HPP
#define SUBPIXELCORNER_HPP
#include <iostream>

#include <iostream>
#include <iostream>

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <sstream>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/imgproc/imgproc.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <sstream>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/imgproc/imgproc.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace cv;
using namespace std;
namespace FeatureDetection
{

class SubPixelCorner
{
private:
    int _winSize;
    int _apertureSize;
    cv::TermCriteria _term;
    double eps;
    Mat mask;
    int _max_iters;
public:
    bool enable;
    SubPixelCorner();

    void checkTerm();

    double pointDist(cv::Point2f estimate_corner,cv::Point2f curr_corner);

    ///method to refine the corners
    void RefineCorner(cv::Mat image,std::vector <cv::Point2f> &corners);

    //function to generate the mask
    void generateMask();


};



}

#endif // SUBPIXELCORNER_HPP
