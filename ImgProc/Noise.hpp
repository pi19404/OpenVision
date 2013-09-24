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

#include "common.hpp"


class GaussianNoise
{
   Mat noisyI;
public:
   Mat add(Mat input,double mean,double std1)
   {
       noisyI.create (input.rows,input.cols,CV_32FC(1));
       noisyI.setTo (cv::Scalar::all (0));
       //double min1,max1;

       vector <Mat>  _channel;
       input.convertTo (input,CV_32FC(3),1.0,0);
       cv::split(input,_channel);
       for(int i=0;i<input.channels ();i++)
       {
           randn(noisyI, Scalar::all(0), Scalar::all(std1/2));
           //cv::minMaxLoc (noisyI,&min1,&max1,0,0);
           //cerr << min1 << ":" << max1 << ":" << std1 << endl;
           //GaussianBlur(noisyI, noisyI, Size(3, 3), 0.5, 0.5);
           cv::add(_channel[i],noisyI,_channel[i]);
       }
       cv::merge (_channel,input);
       input.convertTo (input,CV_8UC(3),1.0,0);
       return input;
   }



};



