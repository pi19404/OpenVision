#include <iostream>

#include "ImgProc/Histogram.hpp"
using namespace std;
#include "ImgProc/meanshift.h"
Histogram h1;
Histogram h2;


cv::Rect* sArea;
bool areaDefined = false;
Mat img;
static bool pressed=false;
bool model=false;
void mouseEventHandler(int event, int x, int y,int flags, void *params) {
    static int x1,y1;


    if(pressed) {
        //rectangle(*(Mat*)params,Point(x1,y1),Point(x,y),Scalar(255,0,0) );
        sArea = new Rect(x1,y1,x-x1,y-y1);
    }
    if(event == CV_EVENT_LBUTTONDOWN) {
        x1 = x;
        y1 = y;
        pressed = true;
        areaDefined=false;
        model=false;
    }
    if(event == CV_EVENT_LBUTTONUP) {
        sArea = new Rect(x1,y1,x-x1,y-y1);

        pressed = false;
        areaDefined = true;
        model=false;
        //cerr << x1 <<":" << y1 <<":" << sArea->width <<":" << sArea->height << endl;
//        sArea->x=80;
//        sArea->y=88;
//        sArea->width=56;
//        sArea->height=82;
        rectangle(*(Mat*)params,*sArea,Scalar(0,255,0) );

    }
}



void callbackButton2(int state,void* userdata)
{
    vector<int> channel;
    channel.push_back(0);
    h1.setChannel(channel);
    vector<int> size,size1;
    size.push_back(30);
    size.push_back(60);
    size.push_back(40);
    h1.setHistSize(size);

    size1.push_back(180);
    size1.push_back(60);
    size1.push_back(40);

    h2.setChannel(channel);
    h2.setHistSize(size1);

    Mat roi=img(*sArea);
    cvtColor(roi,roi,CV_BGR2HSV);
    h1.BuildHistogram(roi);
    model=true;
    Mat i3=h1.drawHist();
    imshow("histogram 1",i3);

    h2.BuildHistogram(roi);
    Mat i2=h2.drawHist();
    imshow("histogram",i2);



    Mat hh=h2.applyKernel(6,2);
    Mat draw=h2.drawHist(hh);
    imshow("smooth 1",draw);

    hh=h2.applyKernel(12,2);
    draw=h2.drawHist(hh);
    imshow("smooth 2",draw);


    hh=h2.applyKernel(18,2);
    draw=h2.drawHist(hh);
    imshow("smooth 3",draw);
}

int test_kde()
{

        /*VideoCapture cam;
        cam = *new VideoCapture(0);
        if( !cam.isOpened() ) {
            cout << "Failed to open camera" << endl;
            return 0;
        }*/

        Mat img1=imread("/media/software/Dropbox/Dropbox/repository/im/documents/images/image020.jpg",1);

        Mat hsvImg;
        MatND hist1;
        char key = 10;
        //cam >> img;
         img1.copyTo(img);
        namedWindow("tracker",0);
        cvSetMouseCallback("tracker",mouseEventHandler,&img);
        cvCreateButton("Build Model",callbackButton2,0, CV_PUSH_BUTTON, 0);
        char text[100];
        sprintf(text,"similarity is %f",0);
        while(key != 27) {
            //cam >> img;
            img1.copyTo(img);
            //cv::flip(img,img,1);
            if(pressed==true || areaDefined==true)
            {
                rectangle(img,*sArea,Scalar(0,255,0),3);
            }
            if(model==true)
            {
                Mat roi=img(*sArea);
                cvtColor(roi,roi,CV_BGR2HSV);
                Mat a1=h1.likeyhoodImage(roi);
                h2.BuildHistogram(roi);
                //double val=h1.compareHist(h2,Histogram::comparison_method::BHATTACHRYA1);
                double val=0;
                cvtColor(a1,roi,CV_GRAY2BGR);

                int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
                double fontScale = 1;
                int thickness = 3;

                sprintf(text,"similarity is %f",val);

                //putText(img, text,Point(20,20), fontFace, fontScale,                        Scalar(0,255,255), thickness, 8);

            }
            imshow("tracker",img);
            key = cvWaitKey(10);

        }
}

int main(int argc,char **argv)
{
    cout << "Hello World!" << endl;
    test_kde();
    return 0;
}



