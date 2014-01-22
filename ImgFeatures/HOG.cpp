/*
 * This is a sample program for computation of HOG features
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
 *
 *
 *
 *If you find this code useful in your projects kindly inform me and cite the document
 *and let me know on any issue encountered .
 */



#include <QtGui/QApplication>


#include "cv.h"
#include <fstream>
#include <iostream>
#include "cv.h"
#include "highgui.h"
#include <QtGui/QImage>
#include <QtCore/qfileinfo.h>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"


static double pi = 3.1416;
using namespace cv;
using namespace std;

std::string outputFilename;

/**
 * @brief function calculates the maximum of 3 floating point integers
 * @param x float
 * @param y float
 * @param z float
 * @return max(x,y,z)
 */

static float maximum(float x, float y, float z) {
    int max = x; /* assume x is the largest */

    if (y > max) { /* if y is larger than max, assign y to max */
        max = y;
    } /* end if */

    if (z > max) { /* if z is larger than max, assign z to max */
        max = z;
    } /* end if */

    return max; /* max is the largest value */
}

/**
 * @brief function computes the histogram of oriented gradient for input image
 * @param Im - input image
 * @param descriptors -output desciptors
 */
static void HOG3(IplImage *Im,vector<float>& descriptors)
{


int nwin_x=3;   //number of windows in x directions
int nwin_y=3;   //number of windows in y directions
int B=12;       //number of orientations


int L=Im->height;   //image height
int C=Im->width;    //image widht

descriptors.resize(nwin_x*nwin_y*B);    //allocating memory for descriptors

CvMat angles2;
CvMat magnit2;
CvMat* H = cvCreateMat(nwin_x*nwin_y*B,1, CV_32FC3);
IplImage *Im1=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,3);
cvConvertScale(Im,Im1,1.0,0.0);



int step_x=floor(C/(nwin_x+1));
int step_y=floor(L/(nwin_y+1));
int cont=0;
CvMat *v_angles=0, *v_magnit=0,h1,h2,*v_angles1=0,*v_magnit1=0;

CvMat *hx=cvCreateMat(1,3,CV_32F); hx->data.fl[0]=-1;
hx->data.fl[1]=0; hx->data.fl[2]=1;
CvMat *hy=cvCreateMat(3,1,CV_32F);
hy->data.fl[0]=1;
hy->data.fl[1]=0;
hy->data.fl[2]=-1;


IplImage *grad_xr = cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F, 3);
IplImage *grad_yu = cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F, 3);



//calculating gradient in x and y directions
cvFilter2D(Im1, grad_xr,hx,cvPoint(1,0));
cvFilter2D(Im1, grad_yu,hy,cvPoint(-1,-1));

cvReleaseImage(&Im1);
cvReleaseMat(&hx);
cvReleaseMat(&hy);



IplImage *magnitude=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,3);
IplImage *orientation=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,3);
IplImage *magnitude1=cvCreateImage(cvSize(C,L),IPL_DEPTH_32F,1);
IplImage *orientation1=cvCreateImage(cvSize(C,L),IPL_DEPTH_32F,1);
IplImage *I1=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,1);
IplImage *I2=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,1);
IplImage *I3=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,1);
IplImage *I4=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,1);
IplImage *I5=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,1);
IplImage *I6=cvCreateImage(cvGetSize(Im),IPL_DEPTH_32F,1);


//cartesian to polar transformations
cvCartToPolar(grad_xr, grad_yu, magnitude, orientation,0);
cvReleaseImage(&grad_xr);
cvReleaseImage(&grad_yu);

cvSubS(orientation,cvScalar(pi,pi,pi),orientation,0);




cvSplit( magnitude, I4, I5, I6, 0 );


cvSplit( orientation, I1, I2, I3, 0 );

int step       = I1->widthStep/sizeof(uchar);


for(int i=0;i<I4->height;i++)
{
   for(int j=0;j<I4->width;j++)
   {


       float *pt1= (float*) (I4->imageData + (i * I4->widthStep));
       float *pt2= (float*) (I5->imageData + (i * I5->widthStep));
       float *pt3= (float*) (I6->imageData + (i * I6->widthStep));
       float max = pt1[j]; /* assume x is the largest */

       if (pt2[j] > max) { /* if y is larger than max, assign y to max */
           ((float *)(I4->imageData + i*I4->widthStep))[j] = ((float *)(I5->imageData + i*I5->widthStep))[j];
           ((float *)(I1->imageData + i*I1->widthStep))[j] =((float *)(I2->imageData + i*I2->widthStep))[j];
       } /* end if */
       //consider only H and S channels.
       if (pt3[j] > max) { /* if z is larger than max, assign z to max */
           ((float *)(I4->imageData + i*I4->widthStep))[j] = ((float *)(I6->imageData + i*I6->widthStep))[j];
           ((float *)(I1->imageData + i*I1->widthStep))[j] =((float *)(I3->imageData + i*I3->widthStep))[j];

       }

       float * pt=((float *)(I1->imageData + i*I1->widthStep));


       if(pt[j]>0)
             {

                 if(pt[j]>pi && (pt[j]-pi <0.001))
                     pt[j]=0;
                 else if(pt[j]<pi && (pt[j]+pi<0.001))
                     pt[j]=0;
                 else
                     pt[j]=pt[j];
                 if(pt[j]>0)
                     pt[j]=-pt[j]+pi;

                 pt[j]=-pt[j];
             }
             else if(pt[j]<0)
             {
                 if(pt[j]>pi && (pt[j]-pi <0.001))
                     pt[j]=0;
                 else if(pt[j]<pi && (pt[j]+pi<0.001))
                     pt[j]=0;
                 else
                     pt[j]=pt[j];
                 if(pt[j]<0)
                     pt[j]=pt[j]+pi;


             }


   }


}
//finding the dominant orientation
cvCopy(I4,magnitude1,0);
cvCopy(I1,orientation1,0);





cvReleaseImage(&orientation);
cvReleaseImage(&magnitude);
cvReleaseImage(&I1);
cvReleaseImage(&I2);
cvReleaseImage(&I3);
cvReleaseImage(&I4);
cvReleaseImage(&I5);
cvReleaseImage(&I6);




int x, y;

int m=0,n=0;


//for each subwindow computing the histogram

for(int n=0;n<nwin_x;n++)
{
   for(int m=0;m<nwin_y;m++)
   {

       cont=cont+1;


       cvGetSubRect(magnitude1,&magnit2,cvRect((m*step_x),(n*step_y),2*step_x,2*step_y));

       v_magnit1=cvCreateMat(magnit2.cols,magnit2.rows,magnit2.type);
       cvT(&magnit2,v_magnit1);
       v_magnit=cvReshape(v_magnit1, &h2,1,magnit2.cols*magnit2.rows);



        cvGetSubRect(orientation1,&angles2,cvRect((m*step_x),(n*step_y),2*step_x,2*step_y));

         v_angles1=cvCreateMat(angles2.cols,angles2.rows,angles2.type);
         cvT(&angles2,v_angles1);
        v_angles=cvReshape(v_angles1, &h1,1,angles2.cols*angles2.rows);

       int K=0;
       if(v_angles->cols>v_angles->rows)
       K=v_angles->cols;
       else
       K=v_angles->rows;
       int bin=0;

       CvMat* H2 = cvCreateMat(B,1, CV_32FC1);
       cvZero(H2);


       float temp_gradient;

       //adding histogram count for each bin
       for(int k=0;k<K;k++)
       {
       float* pt = (float*) ( v_angles->data.ptr + (0 * v_angles->step));
                           float* pt1 = (float*) ( v_magnit->data.ptr + (0 * v_magnit->step));
                           float* pt2 = (float*) ( H2->data.ptr + (0 * H2->step));
       temp_gradient=pt[k];
       if (temp_gradient <= -pi+((2*pi)/B)) {
           bin=0;
           pt2[bin]=pt2[bin]+(pt1[k]);
       }
       else if ( temp_gradient <= -pi+4*pi/B) {
           bin=1;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }
       else if (temp_gradient <= -pi+6*pi/B) {
           bin=2;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }
       else if (  temp_gradient <= -pi+8*pi/B) {
           bin=3;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }
       else if (temp_gradient <= -pi+10*pi/B) {
           bin=4;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }
       else if (temp_gradient <= -pi+12*pi/B) {
           bin=5;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }
       else if (temp_gradient <= -pi+14*pi/B) {
           bin=6;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }
       else if (temp_gradient <= -pi+16*pi/B) {
           bin=7;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }
       else {
           bin=8;
           pt2[bin]=pt2[bin]+(pt1[k]);

       }

       }
       cvReleaseMat(&v_magnit1);
       cvReleaseMat(&v_angles1);
       cvNormalize(H2, H2, 1, 0, 4);



       for(int y1=0;y1<H2->rows;y1++)
       {
       float* pt2 = (float*) ( H2->data.ptr + (0 * H2->step));
       float* pt3 = (float*) ( H->data.ptr + (0 * H->step));
       pt3[(cont-1)*B+y1]=pt2[y1];

       }


       v_angles=0;
       v_magnit=0;
       cvReleaseMat(&H2);

   }
}

for(int i=0;i<descriptors.capacity();i++)
{

    float* pt2 = (float*) ( H->data.ptr + (0 * H->step));
    descriptors[i]=pt2[i];

}
cvReleaseImage(&magnitude1);
cvReleaseImage(&orientation1);
cvReleaseMat(&H);


}



int modea=1;
int modeb=1;


/**
 * @brief function that can be used to call various feature extracting function
 * @param Im    -   input image
 * @param descriptors   - output label
 * @param label
 * @param modes
 * @return
 */
int calculate_features(IplImage *Im,vector<float>& descriptors,float label,bool modes)
{

    if(modeb==1)
  HOG3(Im,descriptors);
    if(modeb==2)
    {
        Mat image=Mat(Im);
        cvtColor(image,image,CV_BGR2GRAY);
        cv::threshold(image,image,0,255,CV_THRESH_BINARY);
        Moments moments =cv::moments(image,true);
        double h[7];
        HuMoments(moments,h);
        descriptors.resize(7);
        for(int i=0;i<7;i++)
        {
            if(abs(h[i])<0.0001)
            descriptors[i]=0;
            else
            descriptors[i]=h[i];
        }
    }


}



/**
 * @brief main  MAIN function of application
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{
    int mode=modea;
    char filename1[1000];


   //output training file name
   if(mode==1)
   {
   outputFilename="/media/LINUX/PROJECT_X3/software/java/train.file";
   sprintf(filename1,"/media/sda9/tdata/train");
   }
   //output testing file name
   else
   {
   outputFilename="/media/LINUX/PROJECT_X3/software/java/test.file";
   sprintf(filename1,"/media/sda9/tdata/test/");
   }




    std::ofstream ofst(outputFilename.c_str());


    vector<float> descriptors;

    IplImage *aa=0;

    IplImage *handimg1;




     QDir data_path(filename1);
     assert(data_path.exists());
     QStringList cSets;
     QStringList classes = data_path.entryList();
     QString trainSet;

     for (int i = 0; i < classes.size(); ++i) {
          trainSet = classes.at(i);
          if (trainSet.startsWith("."))
              continue;
          cSets.append(trainSet);


     };
     std::vector<int> labels;
     labels.begin();
     QString fileSet ;

     QStringList fileSets;
     QStringList fileList;
     //QString fileSet;
     string handPathString ;
     for(unsigned int trainNum=0; trainNum < cSets.size(); trainNum++) {
         trainSet = cSets.at(trainNum);
         QDir file_path(data_path.path()+QString("/%1").arg(trainSet));



          fileList = file_path.entryList();
          for (int i = 0; i < fileList.size(); i++) {
            fileSet = fileList.at(i);
            if (fileSet.startsWith("."))
                continue;
            fileSets.append(fileSet);
            fileSet.clear();
           };
            fileList.clear();
           unsigned int i=0;
          for(i=0; i < fileSets.size(); i++) {

              fileSet = fileSets.at(i);


              QFileInfo handPath(data_path.path() + QString("/%1/%2").arg(trainSet).arg(fileSet));
              handPathString = handPath.filePath().toStdString();
              handimg1 = cvLoadImage(handPathString.c_str(),1);





              aa=cvCreateImage(cvSize(32,24),handimg1->depth,handimg1->nChannels);


              cerr << handPathString.c_str() << endl;
              //resizing image to fixed size
              cvResize(handimg1,aa);


              //calculating theHOG feature descriptors
              calculate_features(aa,descriptors,trainSet.toFloat(),false);

              //saving the descriptors
               ofst<<trainSet.toFloat()<<" ";
                for(int j=0;j<descriptors.capacity();j++)
                {
                        ofst<<j+1<<":"<<(float)descriptors.at(j)<<" ";
                }
                 ofst<<std::endl;

                cvReleaseImage(&handimg1);
                descriptors.resize(0);
                cvReleaseImage(&aa);
          }
            fileSets.clear();


     }



      ofst.close();


}

