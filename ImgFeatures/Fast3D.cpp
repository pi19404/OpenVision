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
 */#include "Fast3D.hpp"

using namespace FeatureDetection;

Fast3D::Fast3D()
{
    block_size=11;
    aperture_size=3;
    borderType=cv::BORDER_DEFAULT;
    current_frame=Mat();
    start=false;
    minDistance=10;
    qualityLevel=0.01;
    image.resize(aperture_size);
    index=0;
    threshold=40;
    maxCorners=100;
    start=false;
    corner_count=0;
}

vector<cv::Point2f> Fast3D::run(Mat src)
{

    keypoints.resize (0);
    corners.resize(0);
    Mat tmp;
    Mat xx1;
    src.copyTo (xx1);
    //vector <cv::Point2f> corners;
   src.copyTo(image[index]);
   Size size=image[0].size();
    corner_count=0;

   cv::GaussianBlur (image[index],image[index],Size(3,3),2);
   if(start==false && index == aperture_size-1)
   {
       start=true;
     }
   if(start==false && index<aperture_size)
   {

       Mat tmp_output;
       tmp_output.create(image[0].rows,image[0].cols,CV_32FC(1));
       Mat matrix1( size, CV_32FC(1));
       matrix1.copyTo (final_return);

   }
else if(start==true)
       {

       double scale = (double)(1 << ((aperture_size > 0 ? aperture_size : 3) - 1)) * block_size;

        scale *=255.;
        scale = 1./scale;

           //computing the derivatives in x and y direction
           Mat Dx, Dy;

           int mindex=(index+1)%aperture_size; //index of the oldest image
           //computing the spatial derivatives
           Sobel( image[mindex], Dx,CV_32F, 1, 0, aperture_size, scale, 0, borderType );
           Sobel(  image[mindex], Dy,CV_32F, 0, 1, aperture_size, scale, 0, borderType );

           Mat mag;
           cv::magnitude (Dx,Dy,mag);  //computing the gradient magnitude
           double max1;
           cv::minMaxLoc (mag,0,&max1,0,0);    //commputing the maximum value of gradient
           //cv::threshold (mag,mag,max1*qualityLevel,0,THRESH_TOZERO);


           for(int i=0;i<mag.rows;i++)
           {
               float *ptr=mag.ptr <float>(i);
               for(int j=0;j<mag.cols;j++)
               {
                   float val=ptr[j];
                   //point satisfies the spatial derivative test
                   if(val >=max1*0.1)  //checking for significant spatial derivatives
                   {



                       //checking the temporal derivative test
                           int index1=(index+1)%3;
                           int index2=(index+2)%3;
                           int index3=(index+3)%3;
                           int index4=i*image[0].step+j;
                           float v1=2*image[index2].data[index4]-image[index1].data[index4]-image[index3].data[index4];
                           v1=abs(v1);
                           if(v1>threshold)            //checking for significant temporal derivatives
                           {
                           //analyze the 3D block for spatio temporal block test.

                                int l1,l2,l3,l4;
                               l1=max(0,i-1);
                               l2=min(mag.rows,i+1);
                               l3=max(0,j-1);
                               l4=min(mag.cols,j+1);
                               int positive=0;
                               int negative=0;
                               for(int x1=l1;x1<=l2;x1++)
                               {
                                   for(int x2=l3;x2<=l4;x2++)
                                   {
                                       if(x1!=i && x2!=j)
                                           continue;
                                       int index5=x1*image[0].step+x2;
                                       for(int x3=0;x3<3;x3++)
                                       {
                                       float val1=image[x3].data[index5];
                                       if(val1 > val+threshold)
                                       {
                                           positive++;
                                       }
                                       if(val1 < val-threshold)
                                       {
                                           negative++;
                                       }
                                       }


                                   }
                               }


                               if(positive>13 || negative >13)
                               {
                                   cv::KeyPoint k;
                                   k.pt=Point(j,i);
                                   k.response=max(positive,negative);
                                   keypoints.push_back (k);

                               }

                           }



                   }

               }
           }



           //for computing the harris response function
           cv::boxFilter (Dx,Dx,Dx.depth (),Size(block_size,block_size));
           cv::boxFilter (Dy,Dy,Dy.depth (),Size(block_size,block_size));


           Mat final_return;
           Dx.copyTo (final_return);
           final_return.setTo (cv::Scalar::all (0));
           std::vector<cv::KeyPoint>::iterator it;


           for( it= keypoints.begin(); it!= keypoints.end();it++)
           {
               //computing the harris response function

               cv::KeyPoint k=*it;
               float * ptr1=Dx.ptr<float>(k.pt.y);
               float * ptr2=Dy.ptr<float>(k.pt.y);
               float   v1=ptr1[(int)k.pt.x];
               float   v2=ptr2[(int)k.pt.x];
               float a=v1*v1;
               float b=v1*v2;
               float c=v2*v2;
               double u = (a + c)*0.5;
               double v = std::sqrt((a - c)*(a - c)*0.25 + b*b);
               double l1 = u - v;      //minimum eigen values
               double l2 = u + v;    //maximum eigen values

               //storing the response function
               k.response=l1;
           }

           //storing the response function in Matrix
           for( it= keypoints.begin(); it!= keypoints.end();it++)
           {

               cv::KeyPoint k=*it;
               float *ptr=final_return.ptr<float>(k.pt.y);
               ptr[(int)k.pt.x]=k.response;



           }


           //imshow("DDD",final_return);
           filter_corners (final_return);


        //getting the center pixel
        int cindex=(index+2)%(aperture_size);
        if(cindex<0)
            cindex=(cindex+aperture_size);
        image[cindex].copyTo(current_frame);

        //subpixel corner refinement
        _subPixel.RefineCorner (image[index],corners);
       }


           index =(index+1) % (aperture_size);


           return corners;



}
