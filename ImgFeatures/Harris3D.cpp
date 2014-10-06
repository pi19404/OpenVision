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
#include "Harris3D.hpp"

using namespace FeatureDetection;



TemporalBoxFilter::TemporalBoxFilter()
{

    ksize=3;
    kernel=(float *)malloc(ksize*sizeof(float));
    kernel[0]=1;
    kernel[1]=1;
    kernel[2]=1;

    cerr << "initialization completed " << endl;

}

TemporalBoxFilter::TemporalBoxFilter(int value)
{


    float sigma=1;
    ksize=value;
    kernel=(float *)malloc(ksize*sizeof(float));
    for(int i=0;i<ksize;i++)
        kernel[i]=(1/sqrt(2*3.1412*sigma))*exp(-(i-ksize/2)*(i-ksize/2)/(2*sigma));

}

void TemporalBoxFilter::process(vector<Mat> image,int index,int bsize,Mat output)
  {



      for(int i=0;i<image[0].rows;i++)
      {
          for(int j=0;j<image[0].cols;j++)
          {

                  float value[output.channels ()];
                  for(int m=0;m<output.channels ();m++)
                      value[m]=0;
                      for(int l=0;l<ksize;l++)
                       {

                           int sindex=(index+1+l)%(ksize);
                          if(sindex<0)
                              sindex=sindex+(ksize);
                          float *ptr=(float *)image[sindex].ptr<float>(i);
                          for(int m=0;m<output.channels ();m++)
                          value[m]=value[m]+(kernel[l]*(float)ptr[image[sindex].channels ()*j+m]);
                      }

                       //value=value;
                      float *out_ptr=(float *)output.ptr<float>(i);
                      for(int m=0;m<output.channels ();m++)
                       out_ptr[j*output.channels ()+m]=value[m]/ksize;




          }
       }


  }


SpatioTemporalDerivativeFilter::SpatioTemporalDerivativeFilter()
{
    bsize=5;
    ksize=3;
    kernel=(float *)malloc(ksize*sizeof(float));
    kernel[0]=-1;
    kernel[1]=0;
    kernel[2]=1;
    aperture_size=3;
    borderType=cv::BORDER_DEFAULT;

}



void SpatioTemporalDerivativeFilter::set_bsize(vector <int>value)
{
   block_size=value;
   bsize=value[2];
   ksize=3;
   kernel=(float *)malloc(ksize*sizeof(float));
   kernel[0]=-1;
   kernel[1]=2;
   kernel[2]=-1;
   aperture_size=3;
   borderType=cv::BORDER_DEFAULT;
}

void SpatioTemporalDerivativeFilter::process(vector<Mat> image,int index,int bsize,vector <Mat> Dt,vector <Mat> Dx,vector <Mat> Dy)
  {



    for(int k=0;k<bsize;k++)
    {
        int sindex=(index+1+k)%(bsize+2);
        if(sindex<0)
            sindex=sindex+(bsize+2);

        Dt[k].setTo(cv::Scalar::all (0));
        Dx[k].setTo(cv::Scalar::all (0));
        Dy[k].setTo(cv::Scalar::all (0));

        double scale = (double)(1 << ((aperture_size > 0 ? aperture_size : 3) - 1)) * block_size[0];
        double scale1 = (double)(1 << ((aperture_size > 0 ? aperture_size : 3) - 1)) * block_size[1];

         scale *=255.;
         scale = 1./scale;

         scale1 *=255.;
         scale1 = 1./scale1;

        cv::Sobel(image[sindex],Dx[k],CV_32F, 1, 0, aperture_size, scale, 0, borderType );
        cv::Sobel(image[sindex],Dy[k],CV_32F, 0, 1, aperture_size, scale1, 0, borderType );

    }



      for(int i=0;i<image[0].rows;i++)
      {
          for(int j=0;j<image[0].cols;j++)
          {
              for(int k=0;k<bsize;k++)
              {

                      float value=0;
                      for(int l=0;l<ksize;l++)
                       {


                           int sindex=(index+1+l+k)%(bsize+2);
                          if(sindex<0)
                              sindex=sindex+(bsize+2);
                          value=value+(kernel[l]*(float)image[sindex].at<uchar>(i,j));
                      }


                       Dt[k].at<float>(i,j)=value;

              }

          }
       }


  }

Harris3D::Harris3D()
{
    current_frame=Mat();
   start=false;
    block_size.resize(3);
    block_size[2]=5;            //block size in termporal scale
    block_size[0]=5;            //block size in  x spatial dimension
    block_size[1]=5;            //block size in y spatial dimension
    minDistance=10;
    qualityLevel=0.001;
    image.resize (block_size[2]+2);
    index=0;
    t.set_bsize (block_size);
    t1=TemporalBoxFilter(block_size[2]);

}


Mat Harris3D::ret_current_frame()
{
    return current_frame;
}


vector<cv::Point2f> Harris3D::run(Mat src)
{
    Mat tmp;
    Mat xx1;
    src.copyTo (xx1);
//    vector <cv::Point2f> corners;
   int ksize=block_size[2];


   src.copyTo(image[index]);
   Size size=image[0].size();


   cv::GaussianBlur (image[index],image[index],Size(3,3),2);
   if(start==false && index == ksize+2-1)
   {
       start=true;
    }

   if(start==false && index<ksize)
   {

       Mat tmp_output;
       tmp_output.create(image[0].rows,image[0].cols,CV_32FC(1));
       Dt.push_back (tmp_output);
       Dx.push_back (tmp_output.clone());
       Dy.push_back (tmp_output.clone ());
       Mat matrix( size, CV_32FC(6));
       block_data.push_back (matrix.clone ());
       matrix.copyTo (final_output);
       Mat matrix1( size, CV_32FC(1));
       matrix1.copyTo (final_return);
       //cerr << index << ": " << output[index].rows << ": " << output[index].cols << endl;
   }
else if(start==true)
       {
           t.process(image,index,block_size[2],Dt,Dx,Dy);
//             Mat xx;

           Mat xx=cv::abs(Dt[2]);
           cv::normalize(xx,xx,0,1,CV_MINMAX);
           imshow("Dt",xx);
           /*
           xx=cv::abs(Dx[2]);
           cv::normalize(xx,xx,0,1,CV_MINMAX);

           imshow("Dx",xx);
           xx=cv::abs(Dy[2]);
           cv::normalize(xx,xx,0,1,CV_MINMAX);

           imshow("Dy",xx);
*/

       //computing the matrix components A,





       //accessing temporal frames
       for(int k=0;k<ksize;k++)
       {
       //accessing rows
       for(int i = 0; i < size.height; i++ )
       {

           //accessing pointer to the rows
          const float* dx1 = (const float*)(Dx[k].ptr<float>(i));
          const float* dy1= (const float*)(Dy[k].ptr<float>(i));
          const float* dt1= (const float*)(Dt[k].ptr<float>(i));
          float* vals = (float*)((block_data[k].ptr<float>(i)));

          //accessing the columns of the matrix
           for(int  j = 0; j < size.width; j++ )
           {
                   //assigning values to the elements of rows
               float dx = dx1[j];
               float dy = dy1[j];
               float dt = dt1[j];
               vals[j*6] = dx*dx;
               vals[j*6+1] = dy*dy;
               vals[j*6+2] = dx*dy;
               vals[j*6+3] = dt*dx;
               vals[j*6+4] = dt*dy;
               vals[j*6+5] = dt*dt;

               //computing the eigen values at each point


           }

       }

       }
       //computing the averaage across spatial dimentions
       vector <Mat> channels;

       for(int k=0;k<ksize;k++)
       {

           cv::split(block_data[k],channels);
           for(int l=0;l<channels.size ();l++)
           {
               cv::GaussianBlur (channels[l],channels[l],Size(block_size[0],block_size[1]),0.1);
               //cv::boxFilter (channels[l],channels[l],channels[l].depth(),Size(block_size[0],block_size[1]));

           }
       }


       //computing the average across temporal dimentions
        t1.process (block_data,0,ksize,final_output);



       final_return.setTo (cv::Scalar::all (0));
        for(int  i = 0; i < size.height; i++ )
        {
            float* vals = (float*)final_output.ptr<float>(i);
            float* o1 = (float*)final_return.ptr<float>(i);
            for(int  j = 0; j < size.width; j++ )
            {
                float a = vals[j*final_output.channels ()];
                float b = vals[j*final_output.channels ()+1];
                float c = vals[j*final_output.channels ()+2];
                float d = vals[j*final_output.channels ()+3];
                float e = vals[j*final_output.channels ()+4];
                float f = vals[j*final_output.channels ()+5];

                double u = (a*(c*f-e*e))-(b*(b*f-d*e))+(d*(b*e-c*d));
                double v = a+c+f;

                double l1 = u - (1/27)*v;
                //double l2 = u + v;    //maximum eigen values

                o1[j] = l1;                 //using the maximum eigen value to determine strong edge
            }
        }


        filter_corners (final_return);





        int cindex=(index-ksize/2)%(ksize+2);
        if(cindex<0)
            cindex=(cindex+ksize+2);
        image[cindex].copyTo(current_frame);

        _subPixel.RefineCorner (current_frame,corners);

       }

       //imshow("3d Harris ",xx1);
           index =(index+1) % (ksize+2);
           return corners;



}

