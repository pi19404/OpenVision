#ifndef ADAPTIVESKINDETECTOR_H
#define ADAPTIVESKINDETECTOR_H
#include "Common/OpenCVCommon.hpp"
/**
 * @brief The AdaptiveSkinColor class : Class incapsulating adaptive skin color detector
 * based on the paper An adaptive real-time skin detector based on Hue thresholding:
A comparison on two motion tracking methods by Farhad Dadgostar *, Abdolhossein Sarrafzadeh
 */

class AdaptiveSkinDetector : public Logger
{
    Mat image;

    //adaptive hue thresholds for skin color detection
    int _hueLower;
    int _hueUpper;

    //global lower and upper thresholds for skin color detection
    Scalar lower;
    Scalar higher;

    Mat hist;

    //histogram merge factor for weighted average
    float _mergeFactor;

    //histogram paramters
    vector<int> histSize;
    vector<float> ranges;
    vector<int> channels;

    //object for histogram computation
    ocv::Histogram h;

    //image required for image motion histogram
    Mat p1;

public:
    /**
     * @brief AdaptiveSkinColor : constructor
     */
    AdaptiveSkinDetector();

    /**
     * @brief run : main function that performs adaptive skin color detection
     * @param image : input BGR image
     * @param mask : output mask ,1 are skin color pixels
     */
    void run(Mat image,Mat &mask);



};

#endif // ADAPTIVESKINDETECTOR_H
