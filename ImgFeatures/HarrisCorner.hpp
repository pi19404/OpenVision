#ifndef HARRIS_CORNER_HPP
#define HARRIS_CORNER_HPP
#include "FeatureDetector.hpp"

namespace  FeatureDetection
{
class HarrisCorner: public FeatureDetector
{
    int block_size;           // local block size used for eigen value computation
    int aperture_size;      //aperture size for edge detection
    double qualityLevel; //prameter for eigen value threshold
    int borderType;
    public:


    //constructor for good feature to track class
    HarrisCorner();

    //constructor for the class good feature to track
    HarrisCorner(uint _maxCorners,int _minDistance,int _aperture_size,double _qualityLevel,int _block_size);


    //implementation of method for feature detection for good feature to track
    vector<cv::Point2f> run(Mat src);



};
}
#endif // HARRIS_CORNER_HPP
