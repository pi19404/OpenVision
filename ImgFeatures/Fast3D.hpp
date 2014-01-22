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
#ifndef FAST3D_H
#define FAST3D_H

#include "FeatureDetector.hpp"


namespace FeatureDetection
{
//comparing values and returning points corresponding to sorted values




class Fast3D :public FeatureDetector
{


int aperture_size;          //aperture size for spatial edge detection
vector <Mat> image;     //vector queue of images for input to termporal  block filtering
Mat  Dt;   //vector queue containing output of termporal block filtering
Mat  Dx,Dy;    //vector queue which contains spatial derivatives
int index;                       // starting index of  bottom of queue
Mat final_return;
Mat current_frame;
int borderType;
int threshold;
int block_size;

//Mat final_output;
double qualityLevel;
public:

bool status()
{
    return start;
}
void set_status(bool st)
{
    start=false;
}

Fast3D();


void reset()
{
    start=false;
}

Mat ret_current_frame()
{
    return current_frame;
}

bool start;
std::vector<cv::KeyPoint> keypoints;

//main function to be called for feature detection
vector<cv::Point2f> run(Mat src);

///returns the corner count after filtering
int get_count()
{
            return corner_count;
 }

};
}
#endif // HARRIS3D_H


