#ifndef IMGVO_H
#define IMGVO_H

#include "ImgFeatures/FeatureDetector.hpp"
#include "ImgFeatures/Fast.hpp"
#include "ImgFeatures/HarrisCorner.hpp"
#include "ImgFeatures/GoodFeatureToTrack.hpp"


class CameraParameters
{
public:
    Mat intrinsic;
    Mat distortion;
    Size size;

    CameraParameters()
    {
        intrinsic=Mat(3,3,CV_32FC1);
        distortion=Mat(1,8,CV_32FC1);
        size.width=320;
        size.height=240;

    }


};

class ImgVo
{
public:
    ocv::FeatureDetection::GoodFeatureToTrack fd;
    ocv::FeatureDetection::SubPixelCorner s;
    float distance;
    vector<Mat> _images;
    vector<vector<Point2f> > _features;
    int N;
    int index;
    bool init;
    TermCriteria termcrit;
    CameraParameters _camera;
    int _method;

    void setMethod(int _m)
    {
        _method=_m;
    }

    class Method
    {
    public:
        const static int RIGID_TRANSFORM=1;
        const static int NEWTON_RAPHSON=2;
        const static int GRADIENT_DESCENT_1=3;
        const static int GRADIENT_DESCENT_2=5;
        const static int REGULARIZED_GRADIENT_DESCENT=4;

    };



    void undistortFeatures(vector<Point2f> &_features)
    {

        Mat _i1,_i2;
        _i1=Mat(1,_features.size(),CV_32FC2);
        for(int i=0;i<_features.size();i++)
        {
            _i1.at<cv::Vec2f>(0,i)[0]=_features[i].x;
            _i1.at<cv::Vec2f>(0,i)[1]=_features[i].y;
        }
        //undistortPoints(_i1,_i2,_camera.intrinsic,_camera.distortion);
        //_features.clear();
        for(int i=0;i< _features.size();i++)
        {
            Point2f a;
            a.x=_i1.at<cv::Vec2f>(0,i)[0]-160;
            a.y=_i1.at<cv::Vec2f>(0,i)[1]-120;
            _features[i]=a;
        }

    }

    ImgVo(int n=10)
    {
        N=n;
        _images.resize(10);
        _features.resize(10);
        fd.setMaxCorners(100);
        fd.setminDistance(10);
        fd.setqualityLevel(0.1);
        index=0;
        init=false;
        termcrit=TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,100,0.1);

        Mat mask1=cv::imread("/home/pi19404/Pictures/mask1.png",0);
        Mat mask;
        mask1.copyTo(mask);
        cv::erode(mask,mask,Mat(),Point(-1,-1),10);
        cv::bitwise_not(mask,mask);
        fd.setMask1(mask);

    }


    //changes based on method
    bool matchFeatures(int index1,int index2,vector<Mat>&_images,vector<vector<Point2f> >&_features)
    {

        vector<uchar> status;
        vector<float> err;

        vector<Point2f> filter1;
        vector<Point2f> filter2;


        //cerr << index1 << ":" << index2 << endl;
        cv::calcOpticalFlowPyrLK(_images[index1],_images[index2],_features[index1],_features[index2],status,err,Size(10,10),3,termcrit);

        //std::cerr << _features[index1].size() << ":" << _features[index2].size() << std::endl;
        for(int i=0;i<_features[index2].size();i++)
        {

            float dist=(_features[index2][i].x-_features[index1][i].x);
            float dist1=(_features[index2][i].y-_features[index1][i].y);
            dist=sqrt(dist*dist+dist1*dist1);

            //cerr << dist << endl;
            if(dist <=1)
            {
               continue;
            }

            if(status[i]==0)
                continue;

            filter2.push_back(_features[index2][i]);
            filter1.push_back(_features[index1][i]);
        }

        if(_features[index2].size()<=0)
            init=false;

        //s.RefineCorner(_images[index2],filter1);
        //s.RefineCorner(_images[index1],filter2);
        _features[index1]=filter1;
        _features[index2]=filter2;


    }



    double computeRotation(vector<Point2f> p1,vector<Point2f>p2)
    {

        if(_method==Method::RIGID_TRANSFORM)
        {
        Mat r=estimateRigidTransform(p1,p2,false);
        double angle=asin(r.ptr<double>(0)[1])*180/M_PI;
        return angle;
        }
        else if(_method==Method::GRADIENT_DESCENT_1)
        {

        }
        else if(_method==Method::GRADIENT_DESCENT_2)
        {

        }
        else if(_method==Method::NEWTON_RAPHSON)
        {

        }
        else if(_method==Method::REGULARIZED_GRADIENT_DESCENT)
        {

        }


    }


    void drawPoints1(Mat &image1)
    {

        for(int k=0;k<_features[index].size();k++)
        {
        cv::circle(image1,Point(_features[index][k].x,_features[index][k].y),5,Scalar(255,0,0),-1,8);
        }

    }


    void drawPoints(Mat &image)
    {
        if(init==false)
            return;
        int ind=index-1;
        if(ind<0)
            ind=ind+N;
        ///cerr << _features[ind].size() << endl;
/*        for(int i=0;i<_features[ind].size();i++)
        {
            cv::circle(image,_features[ind][i],3,Scalar(0,255,255),-1,8);
        }*/
        int index1=ind;
        ind=ind-1;
        if(ind<0)
        ind=ind+N;
        int index2=ind;
        for(int i=0;i<_features[index1].size();i++)
        {
            float dx=-_features[index1][i].x+_features[index2][i].x;
            float dy=-_features[index1][i].y+_features[index2][i].y;
            //cerr << dx << endl;
            dx=1*dx;
            dy=1*dy;
            //dx=sqrt(dx*dx;dy=dy*dy;
            cv::circle(image,Point(_features[index2][i].x,_features[index2][i].y),3,Scalar(0,255,255),-1,8);
            cv::line(image,_features[index2][i],Point(_features[index2][i].x+dx,_features[index2][i].y+dy),Scalar(0,255,0),3,8);
            //imshow("Features",image);
        }

    }


    Mat run(Mat image,Mat disp=Mat())
    {
        if(_images[index].rows>0)
       _images[index].release();

       image.copyTo(_images[index]);

       //feature detection for the frame
       vector<Point2f> points=fd.run(image);
       _features[index]=points;

       if(disp.rows>0)
       {
       //image.copyTo(disp);
       drawPoints1(disp);
       }
//       _features[index]=points;


       if(index>=1)
           init=true;

       if(init==true)
       {
        int index1=index;
        int index2=index-1;
        if(index2<0)
            index2=index2+N;


        //feature matching
        matchFeatures(index2,index1,_images,_features);

        //undistort features
        vector<Point2f>p1,p2;
        p1=_features[index1];
        p2=_features[index2];
        undistortFeatures(p1);
        undistortFeatures(p2);

        double angle=computeRotation(p1,p2);

        int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
        double fontScale = 0.5;
        int thickness = 1;
        char text[100];
        sprintf(text,"%g",angle);
        string ss=text;
        putText(disp, ss,Point(10,10), fontFace, fontScale,Scalar(0,0,255), thickness, 8);

       }

        index=index+1;
        index=index%N;

        return disp;
    }


    Mat generate_image(Mat image,int i)
    {

        Mat output;
        Mat mask=cv::imread("/home/pi19404/Pictures/mask1.png",0);
        //cv::erode(mask,mask,Mat(),Point(-1,-1),3);
        cv::bitwise_not(mask,mask);

        image.setTo(cv::Scalar::all(0),mask);
        Mat A=getRotationMatrix2D(Point2f(320/2,240/2),i,1);
        warpAffine(image,output,A,image.size());
        return output;
    }



    bool process(Mat image,bool replace=false)
    {


    }


    bool UpdateMotion()
    {


    }

};

#endif // IMGVO_H
