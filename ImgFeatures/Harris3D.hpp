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
#ifndef HARRIS3D_H
#define HARRIS3D_H

#include "FeatureDetector.hpp"


namespace FeatureDetection
{

class TemporalBoxFilter
{
    int ksize;          //kernel size for temporal filters
    float *kernel;    //kernel for temporal averaging filters

public:
    //constructor for TemporalBoxFilter which initializes the kernel
    TemporalBoxFilter();

    //constructor for TemporalBoxFilter which initalizes the gaussian kernel
    TemporalBoxFilter(int value);


    //function to be called for termporal filtering
    void process(vector<Mat> image,int index,int bsize,Mat output);


};

class SpatioTemporalDerivativeFilter
{

    public:
    vector <int> block_size;
    int bsize;
    int index;    
    float *kernel;
    int scale;
    int ksize;
    int aperture_size;
    int borderType;

    //constructor for spatio temporal derivative filter
    SpatioTemporalDerivativeFilter();

    //setting the parameters for spatio termporal block size
     void set_bsize(vector <int>value);

     //processing function for spatio termporal derivative filter
     void process(vector<Mat> image,int index,int bsize,vector <Mat> Dt,vector <Mat> Dx,vector <Mat> Dy);


};

class Harris3D :public FeatureDetector
{

vector <int> block_size;          // a vector to hold the block size of spatial and temporal dimensions.
int aperture_size;          //aperture size for spatial edge detection
vector <Mat> image;     //vector queue of images for input to termporal  block filtering
vector <Mat > Dt;   //vector queue containing output of termporal block filtering
vector <Mat > Dx,Dy;    //vector queue which contains spatial derivatives
int index;                       // starting index of  bottom of queue
vector<Mat> block_data;
Mat final_output;
Mat final_return;
SpatioTemporalDerivativeFilter t;       //object for computing spatio-temporal derivatives
TemporalBoxFilter t1;
Mat current_frame;

public:
Harris3D();

//function returns the current frames
Mat ret_current_frame();

      bool start;

//processing function for corner detection
vector<cv::Point2f> run(Mat src);
};
}
#endif // HARRIS3D_H


