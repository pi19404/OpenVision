#include "HarrisCorner.hpp"

using namespace FeatureDetection;


//constructor for good feature to track class
HarrisCorner::HarrisCorner()
{
    maxCorners=100;
    aperture_size=3;
    qualityLevel=0.01;
    minDistance=10;
    block_size=3;
    borderType=cv::BORDER_DEFAULT;
}

HarrisCorner::HarrisCorner(uint _maxCorners,int _minDistance,int _aperture_size,double _qualityLevel,int _block_size)
{
    maxCorners=_maxCorners;
    aperture_size=_aperture_size;
    qualityLevel=_qualityLevel;
    minDistance=_minDistance;
    block_size=_block_size;
    borderType=cv::BORDER_DEFAULT;
}


vector<cv::Point2f> HarrisCorner::run(Mat src)
{

    corners.resize(0);
    Mat dst;
    Mat tmp;
    dst.create( src.size(), CV_32F );






    //scaling by block size in x and y directions.
    //scaling by aperture size 2^(aperture size/2)
   double scale = (double)(1 << ((aperture_size > 0 ? aperture_size : 3) - 1)) * block_size;

    scale *=255.;
    scale = 1./scale;

       //computing the derivatives in x and y direction
       Mat Dx, Dy;
       Sobel( src, Dx,CV_32F, 1, 0, aperture_size, scale, 0, borderType );
       Sobel( src, Dy,CV_32F, 0, 1, aperture_size, scale, 0, borderType );

       Size size = src.size();

       //matrix containing matrix components at each point
       Mat matrix( size, CV_32FC3 );

       //accessing rows
       for(int i = 0; i < size.height; i++ )
       {
           //accessing pointer to the rows
          const float* dx1 = (const float*)(Dx.data + i*Dx.step);
          const float* dy1= (const float*)(Dy.data + i*Dy.step);
          float* vals = (float*)(matrix.data + i*matrix.step);

          //accessing the columns of the matrix
           for(int  j = 0; j < size.width; j++ )
           {
                   //assigning values to the elements of rows
               float dx = dx1[j];
               float dy = dy1[j];

               vals[j*3] = dx*dx;
               vals[j*3+1] = dx*dy;
               vals[j*3+2] = dy*dy;

               //computing the eigen values at each point


           }

       }

       //taking weighted average of pixel values to compute eigen values of block about the pixel
       //we can use other types of filters for weighted averages
       boxFilter(matrix, matrix, matrix.depth(), Size(block_size, block_size),Point(-1,-1), false, borderType );

       //computing the eigen values

       for(int  i = 0; i < size.height; i++ )
       {
           float* vals = (float*)(matrix.data + i*matrix.step);
           float* o1 = (float*)(dst.data + dst.step*i);
           for(int  j = 0; j < size.width; j++ )
           {
               float a = vals[j*3];
               float b = vals[j*3+1];
               float c = vals[j*3+2];
               //calculation of response function
               double u =(float)(a*c - b*b - 0.04*(a + c)*(a + c));

               o1[j] = u;
           }
       }


       filter_corners(dst);


       _subPixel.RefineCorner (src,corners);


       return corners;

}
