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

#ifndef HISTOGRAM
#define HISTOGRAM

#include "Common/OpenCVCommon.hpp"
namespace ocv
{
/**
 * @brief The Histogram class provides interface to class which handles histogram related operations
 */
class Histogram
{
    public:

    /**
     * @brief The comparison_method class - containing various comparison's method
     */
    class comparison_method
    {
    public:
        const static int NORM_L1=1;
        const static int NORM_L2=6;
        const static int INTERSECTION=2;
        const static int CORRELATION=3;
        const static int CHI_SQUARED=4;
        const static int BHATTACHRYA=5;
        const static int BHATTACHRYA1=7;

    };

        /**
         * @brief The comparison_method class contains the types of comparison method
         */


        /**
         * @brief Histogram is default constructor for histogram class
         */
        Histogram();

        /**
         * @brief getHist -returns histogram
         * @return
         */
        Mat getHist();

        /**
         * @brief BuildHistogram method constructs a histogram of source  input image which is required to be a single channel image
         * @param srcImage input single channel 8 bit image
         * @return  histogram matrix
         */
        cv::Mat BuildHistogram(cv::Mat srcImage);

        /**
         * @brief getThresh methods returns the pixel index corresponding to s1% from lower tail and s2% from the upper tails of histogram
         * @param srcImage is the input image
         * @param s1 is ration which indicates the % of pixels from the lower tail of histogram
         * @param s2 is ration which indicates the % of pixels from the upper tails of histogram
         * @return
         */
        std::vector<int> getThresh(cv::Mat srcImage, float s1, float s2);


        Mat likeyhoodImage(Mat image);


     /**
     * @brief setHistSize set the bin size of the histogram
     * @param size
     */
    void setHistSize(vector<int> size);

    /**
     * @brief setRange sets the maximum saturation bin value of the histogram
     * @param range
     */

    /**
     * @brief setRange : set the range values for channels
     */
    void setRange(vector<float>);

    /**
     * @brief setChannel : set the channels which are required to be processed
     */
    void setChannel(vector<int>);

            Mat applyKernel(int,int);

    /**
     * @brief compare - performs comparision of 2 histograms
     * @param h1 - pointer to histogram 1
     * @param h2  - pointer to histogram 2
     * @param length -- number of bins of histogram
     */
    float compareHist(Histogram hist,int method);
    Mat drawHist();
    Mat drawHist(MatND);
    private:
        /**
         * @brief _histMat is temporary matrix used to store the histogram of the image
         */
        MatND _histMat;
        /**
         * @brief _histSize is the size of the hisgoram,number of bins
         */
        vector<int> _histSize;  // Size of the histogram

        /**
         * @brief _range is range of pixel intensities to be considered for the histogram calculations
         */



        vector<float> _histRange;  //Range for all the matrices

        vector<int>_channels;

        Mat _tmpHist;




};


}






#endif


