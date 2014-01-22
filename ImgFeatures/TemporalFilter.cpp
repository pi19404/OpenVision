/*
 * This module describes methods temporal filter class
 * using opencv
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
#include "TemporalFilter.hpp"

void Temporal_Filter::Log_filter(float input[],float mask[])
{


    int index1=0;
    for(int j=0;j<ksize;j++)
    {
         output[j]=0;

    for(int i=0;i<ksize;i++)
    {
        index1=(index1+M+i-j)%ksize;

        output[j]= output[j]+input[i]*mask[index1];
    }

    }

}

Temporal_Filter::Temporal_Filter()
{
    cerr << "const" << endl;
    PI=3.1412;
    ksize=7;
    var=2;
    M=4;
    k=0;
    index1=0;
    mask=(float*)calloc(ksize,sizeof(float));
    output=(float*)calloc(ksize,sizeof(float));
    //mask=new float[ksize];
    //output=new float[ksize];

}



Mat Temporal_Filter::temporal_filter(Mat input1)
{


    Mat dst;
    float temp=0;
    if(k<ksize)
    {
        if(k==0)
        {
        image.resize(ksize);
        log1.resize(ksize);
        index1=0;
        /**
         * Precmputing the kernel masks
         */
        for(int i=0;i<ksize;i++)
        {
        output[i]=0;
        float scale=((1/(var*sqrt(2*var*PI))));
        float f1=(((i+1-M)*(i+1-M)/(var))-1);
        float v=(((float)-1*(i+1-M)*(i+1-M))/(float)(2*var));
        mask[i]=scale*std::exp(v)*f1;

        temp=temp+mask[i];

        }

        //normalising the kernel masks
        temp=temp/ksize;
        float min=999999;
        for(int i=0;i<ksize;i++)
        {
            mask[i]=mask[i]-temp;
            if(abs(mask[i])<min)
                min=abs(mask[i]);
        }
        //making the lowest value of mask as 1
        for(int i=0;i<ksize;i++)
        {

            mask[i]=ceil(mask[i]/abs(min));

        }

        }
        k=k+1;

        //accumulating images
        input1.copyTo(image[index1]);
        index1  =(index1+1) % ksize;
        return input1;

    }

        //copy updated images
        input1.copyTo(image[index1]);
        input1.copyTo(dst);
        dst.setTo(cv::Scalar::all(0));

        for( int i=0;i<image[0].rows;i++)
        {

            for(int j=0;j<image[0].cols;j++)
             {
                float input[ksize];



                for(int l=0;l<ksize;l++)
                {
                    input[l]=image[((index1+l)%ksize)].at<uchar>(i,j);
                }

                    //compute the log filter output
                    Log_filter(input,mask);
                    for(int m=0;m<ksize-1;m++)
                    {


                        //check for temporal zero filter crossings
                        if((output[m]<0 && output[m+1]>0 )||(output[m]>0 && output[m+1]<0 ))
                        {
                            dst.at<uchar>(i,j)=output[m];
                            break;

                        }
                        else if(((m>1 && m <ksize) && output[m]==0) && ((output[m-1]>0 && output[m+1] <0 )||(output[m-1]<0 && output[m+1] >0 )))
                        {
                            dst.at<uchar>(i,j)=output[m];
                        }
                        else
                        {
                            dst.at<uchar>(i,j)=0;
                        }
                }
             }


        }



            //update the buffer index
            index1  =(index1+1) % ksize;

            k=k+1;

        return dst;



}

