#include "GoodFeatureToTrack.hpp"

using namespace FeatureDetection;

GoodFeatureToTrack::GoodFeatureToTrack ()
{
    maxCorners=100;
    aperture_size=3;
    qualityLevel=0.01;
    minDistance=10;
    block_size=3;
    borderType=cv::BORDER_DEFAULT;
}


GoodFeatureToTrack::GoodFeatureToTrack(uint _maxCorners,int _minDistance,int _aperture_size,double _qualityLevel,int _block_size)
{
    maxCorners=_maxCorners;
    aperture_size=_aperture_size;
    qualityLevel=_qualityLevel;
    minDistance=_minDistance;
    block_size=_block_size;
    borderType=cv::BORDER_DEFAULT;
}

void GoodFeatureToTrack::setApertureSize(int value)
{
    aperture_size=value;
}


void GoodFeatureToTrack::setqualityLevel(double value)
{
    qualityLevel=value;
}


void GoodFeatureToTrack::setblock_size(int value)
{
    block_size=value;
}


vector<cv::Point2f> GoodFeatureToTrack::run(Mat src)
{


    corners.resize(0);

    Mat dst;


    //matrix for storing the corner response before filtering
    dst.create( src.size(), CV_32F );

   Mat xx1(src.rows,src.cols,CV_8UC(3));
   xx1.setTo (cv::Scalar::all (0));



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

               double u = (a + c)*0.5;
               double v = std::sqrt((a - c)*(a - c)*0.25 + b*b);
               double l1 = u - v;      //minimum eigen values
               double l2 = u + v;    //maximum eigen values

               o1[j] = l1;                 //using the maximum eigen value to determine strong edge

           }
       }





       filter_corners (dst);


       _subPixel.RefineCorner (src,corners);


       return corners;

}



void GoodFeatureToTrack::filter_corners(Mat dst)
{

    vector <cv::KeyPoint> _tmp_keypoints;
    int  _maxCorners=maxCorners;
    _tmp_keypoints.resize (0);
    corners.resize (0);
    const int cell_size =cvRound(minDistance);
    int xx=floor(dst.cols/cell_size);
    int yy=floor(dst.rows/cell_size);

    //thresholding the eigen values
    double maxVal = 0;
    minMaxLoc(dst, 0, &maxVal, 0, 0,Mat() );


    //accessing the cell blocks of size min distance

    for(int x=0;x<xx;x++)
    {
        for(int y=0;y<yy;y++)
        {
            int w=cell_size;
            if((x+1)*(cell_size)>dst.cols)
                w=(x+1)*(cell_size)-dst.cols;
            int h=cell_size;
            if((y+1)*(cell_size)>dst.rows)
                h=(y+1)*(cell_size)-dst.rows;

            Rect roi(x*cell_size,y*cell_size,w,h);
            //cerr <<  roi.x << ":" << roi.width <<":" <<  dst.cols << endl;
            //cerr <<  roi.y << ":" << roi.height <<":" <<  dst.rows << ":" << (y+1)*(cell_size) << ":" << src.rows <<endl;
            //selecting the subregion
            Mat r=dst(roi);
            //selecting the maximum eigen value pixel in the subregion
            //double maxVal;
            Point minLoc;
            double m1=0;
            minMaxLoc( r, 0, &m1,0,&minLoc, Mat());
            if(m1>=maxVal*qualityLevel)
            {
            //threshold(r,r,m1, 0, THRESH_TOZERO );
            minLoc.x=minLoc.x+x*cell_size;
            minLoc.y=minLoc.y+y*cell_size;

            cv::KeyPoint k;
            k.pt.x=minLoc.x;
            k.pt.y=minLoc.y;
            k.response=m1;
             _tmp_keypoints.push_back (k);
            }

        }

    }

    if( _tmp_keypoints.size()<maxCorners)
        _maxCorners=( _tmp_keypoints.size());

    //sorting the eigen value vector
    sort( _tmp_keypoints.begin(),  _tmp_keypoints.end(), pred());

    int ncorners=0;
    for (int i = 0; i <  _tmp_keypoints.size(); ++i)
    {
        bool good=true;
        //populating vectors of corners detected

        for(int k=0;k<corners.size();k++)
        {
            double dx=_tmp_keypoints[i].pt.x-corners[k].x;
            double dy=_tmp_keypoints[i].pt.y-corners[k].y;
            if( dx*dx + dy*dy < minDistance* minDistance )
            {
                good =false;
                break;

            }

        }


        if(good==true)
        {
            corners.push_back(_tmp_keypoints[i].pt);
            ncorners++;

        }

        if(ncorners >=maxCorners)
            break;
    }

}

