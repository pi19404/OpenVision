#ifndef TEMPORALFILTER_HPP
#define TEMPORALFILTER_HPP
#include "cv.h"
#include <fstream>
#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/core/core.hpp>
/*
 * head file for temporal filter class
 * Copyright (C) 2012 by pi19404
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

using namespace cv;
using namespace std;


class Temporal_Filter
{
float PI;    //approaximation of pi
int ksize;      //kernel size for log filter mask discrete approximation
float var;      // variance of log filter gaussian
int M;            // mean of log filter gaussian

vector<Mat> image;  // vector of input images
vector<Mat> log1;
float *mask;  // temporal filter calculated kernel mask
float *output;//  output of temporal filter

int k;
int index1;



public:

Temporal_Filter();
//int t[ksize];
/**
 * @brief function to perform 1D filtering operation
 * @param inputs are 1D signal and filter mask kernel
 *
 */
void Log_filter(float input[],float mask[]);

/**
 * @brief method that performs temporal filtering
 * @param input1 is the input image
 * @return  output is the temporal filtered image
 */
Mat temporal_filter(Mat input1);


};

#endif // TEMPORALFILTER_HPP
