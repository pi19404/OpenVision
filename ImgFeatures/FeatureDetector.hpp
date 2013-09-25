/* ========================================================================
 * Copyright [2013][prashant iyengar] The Apache Software Foundation
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 * ========================================================================
 */
#ifndef FEATURE_DETECTOR_HPP
#define FEATURE_DETECTOR_HPP
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
#include "SubPixelCorner.hpp"
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <sstream>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/imgproc/imgproc.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace cv;
using namespace std;

namespace  FeatureDetection
{

template<class T>
struct accessor {

    explicit accessor(const T& data) : value(data) {}
    T operator()() const { return value; }
    T& operator()() { return value; }
    void operator()(const T& data) { value = data; }

private:

    accessor(const accessor&);
    accessor& operator=(const accessor&);
    T value;

};


class FeatureDetector
{
public:

    // response function for keypoint comparison
    bool compare_response(cv::KeyPoint first,cv::KeyPoint second)
    {
      if (first.response < second.response) return true;
      else return false;
    }

    //function used by sort function for sorting keypoints according to response
    struct pred
    {
        bool operator()(KeyPoint const & a, KeyPoint const & b) const
        {
            return a.response > b.response;
        }
    };


    //consuructor for feature detection



    //function to filter features using maxima suppression and min distance criteria
    void filter_corners(Mat dst);

protected:
    template<class T> struct index_cmp {
    index_cmp(const T arr) : arr(arr) {}
    bool operator()(const size_t a, const size_t b) const
    { return arr[a] > arr[b]; }
    const T arr;
    };


    uint maxCorners;    //maximum number of corners to be detected
    double qualityLevel;    //quality level for corner response function
    int minDistance;        //min distance between detected points


    cv::vector<cv::Point2f> corners;    //vector for corners

    int corner_count; //total corner count returned

public:
    virtual vector<cv::Point2f> run(Mat src)=0;
     FeatureDetector();
     SubPixelCorner _subPixel;              //object supporting methods for subpixel estimation

    //method to set the maxCorners value
    virtual void setMaxCorners(uint value);

    virtual Mat ret_current_frame()
     {
         return Mat();
     }



    //method to set minDistance value
    void setminDistance(uint value);

    int get_count()
   {
        return 0;
      }
};
}


/* End of file */



#endif // FEATURE_DETECTOR_HPP
