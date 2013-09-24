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

#include "ContrastStretching.hpp"
using namespace ImgProc;


ContrastStretching::ContrastStretching()
{
    _mode=ContrastStretching::MODE_DEFAULT;
    lowerRatio.push_back (0.015);
    lowerRatio.push_back (0.015);
    lowerRatio.push_back (0.03);
    upperRatio.push_back (0.015);
    upperRatio.push_back (0.015);
    upperRatio.push_back (0.03);
    threshold=70;
}

void ContrastStretching::setMode(Mode mode)
{
    _mode=mode;
}

Mode ContrastStretching::getMode()
{
    return _mode;
}

Scalar ContrastStretching::getMin()
{
    return _min;
}

Scalar ContrastStretching::getMax()
{
    return _max;
}

Scalar ContrastStretching::getLUT(Scalar a)
{
    Scalar _tmp;
    _tmp[0]=_LUT[a[0]*_channels];
    _tmp[1]=_LUT[a[1]*_channels+1];
    _tmp[2]=_LUT[a[2]*_channels+2];
    return _tmp;
}


Mat ContrastStretching::GlobalContrastStretching(Mat image)
{


    if(_image.rows==0 || _image.cols==0)
    {
        _image=image.clone ();
        _channels=_image.channels ();
        _rows=_image.rows;
        _cols=_image.cols;
    }
    else
    {
        _image=image.clone ();
    }
    if(_LUT.size ()!=256*_image.channels ())
    {
        _LUT.resize (_image.channels ()*256);
    }

        cv::split(_image,_channel);
        MakeGlobalLUT();
        uchar * ptr=_image.data;;

        for(int i=0;i<_rows;i++)
        {
            for(int j=0;j<_cols;j++)
            {
                for(int c=0;c<_channels;c++)
                ptr[i*_image.step+j*_channels+c]=_LUT[_channels*ptr[i*_image.step+j*_channels+c]+c];
            }
        }


    return _image;

}


void ContrastStretching::MakeGlobalLUT()
{


    if(_mode==ContrastStretching::MODE_DEFAULT)
    {
    for(int i=0;i<_channels;i++)
    {
        cv::minMaxLoc (_channel[i],&_min1,&_max1,0,0);
        _min[i]=_min1;
        _max[i]=_max1;
        for(int k=0;k<256;k++){
             _LUT[k*_channels+i]=k-_min1>0?k>_max1?255:255*(k-_min1)/(_max1-_min1):0;
        }
    }
    }
    else if(_mode==ContrastStretching::MODE_MODIFIED)
    {
        for(int i=0;i<_channels;i++)
        {
                Histogram imgHist;
                std::vector<int> imgThresh;
                imgThresh = imgHist.getThresh(_channel[i], lowerRatio[i], upperRatio[i]);
                _min1=0;
                _max1=0;
                cv::minMaxLoc (_channel[i],&_min1,&_max1,0,0);
                _min[i]=imgThresh[0];
                _max[i]=imgThresh[1];
                for(int k=0;k<256;k++)
                {
                    if(_max1>threshold)
                    {
                        if(_max[i]<threshold/2)
                            _max[i]=threshold/2;
                        _LUT[k*_channels+i]=(k-_min[i]<0)?0:(k>_max[i])?255:(255*(k-_min[i])/(_max[i]-_min[i]));
                    }
                      else
                          _LUT[k*_channels+i]=k;
                }
        }

    }


}








