#ifndef FAST_H
#define FAST_H
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
#include "FeatureDetector.hpp"
using namespace cv;
using namespace std;
namespace  FeatureDetection
{
class Fast: public FeatureDetector
{
public:
    Fast();
    int threshold;                       //threshold for corner detection

    vector<Point2f> offset;       //relative offset of the points on the circle from center point


    uchar threshold_tab[512];   //thresholds points on circle based on comparison with center pixels for count
    vector<Point2f> run (Mat src);


    vector <uchar > checks;
    vector <uchar > checks1;   
    bool init;
    int K;
};
}
#endif // FAST_H
