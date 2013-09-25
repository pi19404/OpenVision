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
#ifndef GOOD_FEATURES_TO_TRACK_HPP
#define GOOD_FEATURES_TO_TRACK_HPP

#include "FeatureDetector.hpp"

namespace  FeatureDetection
{
class GoodFeatureToTrack : public FeatureDetector
{


int block_size;           // local block size used for eigen value computation
int aperture_size;      //aperture size for edge detection
double qualityLevel; //prameter for eigen value threshold
int borderType;
public:


//comparing values and returning points corresponding to sorted values
template<class T> struct index_cmp {
index_cmp(const T arr) : arr(arr) {}
bool operator()(const size_t a, const size_t b) const
{ return arr[a] > arr[b]; }
const T arr;
};

//constructor for good feature to track class
GoodFeatureToTrack();

//constructor for the class good feature to track
GoodFeatureToTrack(uint _maxCorners,int _minDistance,int _aperture_size,double _qualityLevel,int _block_size);

//setter methods for aperture size
void setApertureSize(int value);

//setter methods for quality level
void setqualityLevel(double value);

//setter methods for block size
void setblock_size(int value);


//implementation of method for feature detection for good feature to track
 vector<cv::Point2f> run(Mat src);

//function to filter the corners and find locally significant corner points
void filter_corners(Mat dst);


};
}
#endif // GOOD_FEATURES_TO_TRACK_HPP
