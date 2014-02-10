#ifndef EDGEDETECTOR_H


#define EDGEDETECTOR_H
#include "Common/OpenCVCommon.hpp"
using namespace cv;
class EdgeDetector
{
public:
    EdgeDetector();


    Mat _lable; //edge label image
    Size s; //store image size
    Mat lout;   ///tmp image
    stack<Point> points; //point stack for connected component analysis

    int nMinx,nMiny,nMaxx,nMaxy;//min and max image bounds
    //function to perform smoothing
    void smoothing(Mat src,Mat &dst,int aperture);

    /* function to compute the color gradient
     *
     *
     */
    void colorGradient(Mat src,Mat &fmag,Mat &fori,int aperture);



    //function to perform non maxima suppression for grayscale image
    void nonMaxima(Mat &mag,Mat ori);


    //main function for performing connected component anaysis
    //starting from higher threshold images
    void connectedComponent(Mat hout);


    //main function for canny
    void canny(Mat src,vector<Mat> &dst,vector<double> lower,vector<double> higher,int aperture);





    //getting label image pixel
    int getPixel(Point p);




    //function to check if point is connected to edge
    bool check_color(Point p);


    //set pixel in the labelled image
    void setPixel(Point p);



    //function to edge following
    void stackfill(int x,int y);




};

#endif // EDGEDETECTOR_H
