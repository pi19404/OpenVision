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

#include "Common/OpenCVCommon.hpp"
#include "ImgProc/Histogram.hpp"

namespace ImgProc
{
/**
 * @brief The ContrastStretching class implements methods for contrast stretching application.Two versions
 * of contrast stretching methods are provided the standard version and modified version which provides
 *more robustness to noise
 */
class ContrastStretching
{
enum { MODE_DEFAULT,MODE_MODIFIED};

public:

ContrastStretching();


private :
vector <uchar> _LUT;            //lookup table for transformation
vector <Mat> _channel;      // RGB channel buffer
double _min1,_max1;         //temporal variables
Scalar _min,_max;               //minimum and maximum pixel intensities used for transformation
Mat _image;                         //temporary buffer for image
int _channels,_rows,_cols; //temporary variables for image information
std::vector<float> lowerRatio;  //lower and upper ration thresholds for image
std::vector<float> upperRatio;
int threshold ; //distance threshold between new and old threshold
Mode  _mode;

vector <int> channels;  //variable indicating which  channels should be processed


/**
 * @brief MakeGlobalLUT methods constructs lookup table for contrast stretching
 */
void MakeGlobalLUT();

public:
Scalar getMin();
Scalar getMax();

/**
 * @brief getLUT methods returns the lookup table
 * @return
 */
vector <uchar> getLUT()
{
    vector <uchar> tmp;
    tmp.resize(_LUT.size());
    copy(_LUT.begin(), _LUT.end(), tmp.begin());
    return tmp;
}

Scalar getLUT(Scalar a);

public:
/**
 * @brief GlobalContrastStretching is the processing function to perform global contrast stretching operations
 * @param image is input image
 * @return is contrast stretched images
 */
Mat GlobalContrastStretching(Mat image);

/**
 * @brief setMode methods sets the type of global contrast stretching to be performed
 * @param mode
 */
void setMode(int mode);
/**
 * @brief getMode method return the mode
 * @return
 */
Mode getMode();
};

}


