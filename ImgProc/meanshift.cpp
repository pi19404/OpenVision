#include "meanshift.h"


using namespace ocv;

ocvmeanShift::ocvmeanShift()
{




}


void ocvmeanShift::buildModel(Mat image,Rect rect)
{
    region=rect;
    p.x=region.x+region.width/2;
    p.y=region.y+region.height/2;
    Mat roi=image(rect);
    h.BuildHistogram(roi);
}



Rect ocvmeanShift::run(Mat image)
{

    criteria=cv::TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS,100,1.0);
    //computing the likelyhood of image
    Mat sim=h.likeyhoodImage(image);
    cv::normalize(sim,sim,0,1,CV_MINMAX);
    Mat dd;
    image.copyTo(dd);
    for(int i=0;i<criteria.maxCount;i++)
    {
    //extracting the region of interest
    Mat roi=sim(region);



    imshow("AA",roi);
    //computing the mean
    cv::Moments m;
    m=cv::moments(roi,false);

    //cerr << m.m00 << ":" << region.width << ":" << region.height << endl;

    //threshold for similarity
    if(fabs(m.m00)<region.width*region.height*0.05)
        break;

    //computing the mean values
    int x=cvRound(m.m10/m.m00);
    int y=cvRound(m.m01/m.m00);

//    cv::circle(dd,Point(region.x+x,region.y+y),10,Scalar(0,255,255),-1,8);
//    cv::circle(dd,Point(p.x,p.y),10,Scalar(0,255,255),-1,8);


    //computing the mean shift
    int dx=region.width/2-x;
    int dy=region.height/2-y;

    //displacement from current position
    int nx=p.x-dx;
    int ny=p.y-dy;

    //bounday of the image
    if(nx-region.width/2<=0) nx=region.width/2;
    if(nx+region.width/2>=image.cols) nx=image.cols-region.width/2-1;
    if(ny-region.height/2<=0) ny=region.height/2;
    if(ny+region.height/2>=image.rows) ny=image.rows-region.height/2-1;

    //recalculating the mean shift
    dx=-nx+p.x;
    dy=-ny+p.y;

    //checking magnitude of mean shift vector.

    float mag=dx*dx+dy*dy;

    //no change in mean
    if(mag<criteria.epsilon*criteria.epsilon)
        break;

  //  imshow("A1",dd);
    //updating the position
    p.x=nx;
    p.y=ny;
    //updating the region of interest
    region.x=p.x-region.width/2;
    region.y=p.y-region.height/2;

    }

    //return the new co-ordinates
    return region;
}
