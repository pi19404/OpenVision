#include "FeatureDetector.hpp"

using namespace FeatureDetection;

FeatureDetection::FeatureDetector::FeatureDetector()
{
    maxCorners=50;
    qualityLevel=0.01;
    minDistance=5;
}

void FeatureDetection::FeatureDetector::setMaxCorners(uint value)
{
    maxCorners=value;
}

void FeatureDetection::FeatureDetector::setminDistance(uint value)
{
    minDistance=value;
}

void FeatureDetection::FeatureDetector::filter_corners(Mat dst)
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
    int ncorners=0;

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
            if(m1>maxVal*qualityLevel)
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

    if( _tmp_keypoints.size()<_maxCorners)
        _maxCorners=( _tmp_keypoints.size());

    //sorting the eigen value vector
    sort( _tmp_keypoints.begin(),  _tmp_keypoints.end(), pred());


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

        if(ncorners >=_maxCorners)
            break;
    }

    //cerr << ncorners << endl;
        corner_count=ncorners;
}
