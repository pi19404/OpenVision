#ifndef DENSEOF_H
#define DENSEOF_H
#include "ImgBasis/polybasis.hpp"

#include "Common/OpenCVCommon.hpp"
#include "ImgProc/convolution.hpp"
#include "ImgProc/channelfilter.hpp"
#include "ImgBasis/polybasis.hpp"
typedef Vec<float,6> Vec6f;
typedef Vec<float,5> Vec5f;
class DenseOF
{
public:
    vector<Mat> _images;
    int N;
    int _index;
    bool init;
    Mat x,y,xx,yy,xy,c,kernel;
    vector<Mat> res;
    Size s;
    Size win;
    Mat G;
    Mat flow;

    //class which computes polynomial basis coefficients
    polyBasis poly;

    //filter to perform seperable convolution
    //SeperableSConvolution filter;
    //channel filter used for computing inverse
    channelFilter cfilter;

    //SeperableSConvolution avgflow;



    //Ptr<FilterEngine> fx,fy,fxy,fxx,fyy,fc,cf;


    vector<Mat> o1,o2;
    vector<Mat> channels,channels1;
    Mat i1,i2;
    Mat prevflow;
    Mat row;

    /**
     * @brief DenseOF : constructor for dense optical flow estimation
     */
    DenseOF();

    /**
     * @brief DenseOF::getIndex : method returns modulo N buffer index
     * @param index
     * @return
     */
    int getIndex(int index);


    /**
     * @brief getSubPixel
     * @param ptr   : Input pointer of Mat arrayy
     * @param p     : Subpixel at which pixel values is to be estimated
     * @param ch    : number of channels in the image
     * @param s     : Image size
     * @param step1 : width step
     * @return
     */
    Mat getSubPixel(const float *ptr,Point2f p,int ch,Size s,int step1);

    /**
     * @brief updatePoly : The function computes the optical flow displacement vector
     * @param ptr1 : pointer to array containing the polynomial basis components
     * @param ptr2 : pointer to array containing the polynomial basis components
     * @param d    : estimate of displacement vector at the current point
     * @param flag : the flag indicating if the point is border pixels or not
     * @param M    : pointer to output array returning the computed coefficients
     * @param x    : co-ordinate at the current point where the coefficients is evaluated
     * @param y    :
     * @param s    : windows size for averaging
     */
    void updatePoly(const float *ptr1,const float *ptr2,Point2f d,bool flag,float *M,int x,int y,Size s);


    /**
     * @brief update_index : updates the index of current image
     */
    void update_index();


    //this function computes the optical flow displacement at each point in the image
    //R0 and R1 are mat containing the polynomial basis of prev and curr frame
    //M contains the initial estimate of the computed flow
    //flow is input inital flow provided as input

    /**
     * @brief EstimateFlow : This function computes the displacement field
     * @param _R0
     * @param _R1
     * @param _flow
     * @param matM
     * @param _y0
     * @param _y1
     */
    void EstimateFlow( const Mat& _R0, const Mat& _R1, const Mat& _flow, Mat& matM);


    /**
      * @brief AverageFlow
      * @param _R0   : Polynomial basis of prev frame
      * @param _R1   : Polynomial basis coefficients of current frame
      * @param _flow :estimate of current displacement field
      * @param matM :containing coefficients of polynomial basis for computing displacemnt field
      */
     void AverageFlow( const Mat& _R0, const Mat& _R1,Mat& _flow, Mat& matM,int winsize);


     /**
      * @brief DenseOF::compute : Main function that computes dense optical flow
      * @param image    : vector of images
      * @param levels   : number of pyramidal levels
      * @param pyr_scale : scaling factor
      * @param iterations : number of iterations
      */
     void compute(Mat image,int levels=1,float pyr_scale=0.9,int iterations=3,bool init=false);

};

#endif // DENSEOF_H
