#include "edgedetector.hpp"

EdgeDetector::EdgeDetector()
{



}


void EdgeDetector::smoothing(Mat src,Mat &dst,int aperture)
{
    cv::GaussianBlur(src,src,Size(aperture,aperture),1);
}


void EdgeDetector::colorGradient(Mat src,Mat &fmag,Mat &fori,int aperture)
{


    Mat dx,dy,mag,orientation;
    vector<Mat> ch;
    vector<Mat> mc;
    Mat src1;
    src.copyTo(src1);
    cv::split(src1,ch);
    cv::Sobel(ch[0],ch[0],CV_32F,1,0,aperture, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(ch[1],ch[1],CV_32F,1,0,aperture, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(ch[2],ch[2],CV_32F,1,0,aperture, 1, 0, cv::BORDER_REPLICATE);

    //cv::cartToPolar(ch,mag,orientation,true);
    cv::merge(ch,dx);

    src.copyTo(src1);
    cv::split(src1,ch);
    cv::Sobel(ch[0],ch[0],CV_32F,0,1,aperture, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(ch[1],ch[1],CV_32F,0,1,aperture, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(ch[2],ch[2],CV_32F,0,1,aperture, 1, 0, cv::BORDER_REPLICATE);
    cv::merge(ch,dy);

    cv::cartToPolar(dx,dy,mag,orientation,true);






    fmag=Mat(dx.rows,dx.cols,CV_32FC1);
    fori=Mat(dx.rows,dx.cols,CV_32FC1);
    fmag.setTo(cv::Scalar::all(0));
    fori.setTo(cv::Scalar::all(0));
    MatConstIterator_<Vec3f> it = mag.begin<cv::Vec3f>(), it_end = mag.end<cv::Vec3f>();
    MatConstIterator_<Vec3f> it1 = orientation.begin<cv::Vec3f>();
    MatIterator_<float> it3 = fmag.begin<float>();
    MatIterator_<float> it4 = fori.begin<float>();

    for(; it != it_end; ++it,++it1,++it3,++it4)
    {
         const Vec3f& pix = *it;
         const Vec3f& ori=*it1;
         float tmp=pix[0];
         float ori1=ori[0];

         if(pix[1]>tmp)
         {
             tmp=pix[1];
             ori1=ori[1];
         }
         if(pix[2]>tmp)
         {
             tmp=pix[2];
             ori1=ori[2];
         }

         *it3=tmp;
         *it4=ori1;



    }


}



void EdgeDetector::nonMaxima(Mat &mag,Mat ori)
{

    int width=mag.cols;
    int height=mag.rows;

        Mat mag2;
        mag.copyTo(mag2);

        float *ptr=(float*)mag.data;
        float *ptr2=(float*)ori.data;
        float *ptr3=(float*)mag2.data;


        for(int i=1;i<height-1;i++)
        {
            for(int j=1;j<width-1;j++)
            {
                float ori1=ptr2[j+i*width];
                for(int k=0;k<4;k++)
                {

                    float i1=(k*45)+45;
                    float i2=i1-45;
                    float i3=(k*45)+180;
                    float i4=i3+45;
                    float ori2=((int)(ori1+(45.0/2)));
                    if(ori2 >360)
                        ori2=ori2-360;

                    if((ori2 < i1 && ori2 >=i2 ) ||(ori2 >= i3 && ori2 <=i4))
                    {
                        ori1=k*45;
                        break;
                    }
                }

                ptr2[j+i*width]=ori1;
            }
        }


        for(int i=1;i<height-1;i++)
        {
            for(int j=1;j<width-1;j++)
            {
                float center=ptr[j+i*width];
                int code=0;

                if(center==0)
                {
                    continue;
                }

                //pi/8
                //if(0.414213*abs(dx1[j+i*width])>abs(dy1[j+i*width]))
                if(ptr2[j+i*width]==0)
                {

                    float left=ptr[(j-1)+(i)*width];
                    float right=ptr[(j+1)+(i)*width];

                    code=(left <=center)  && (  right <=center) ;

                    //code=1;
                }
                //else if(3*0.414213*abs(dx1[j+i*width])>abs(dy1[j+i*width]))
                else if(ptr2[j+i*width]==90)
                {
                    code=(ptr[j+(i-1)*width] <center) && (ptr[j+(i+1)*width] <center);
                    code=1;

                }

                else //if(abs(dx1[j+i*width])==abs(dy1[j+i*width]) && abs(dy1[j+i*width])>0)
                {

                    if(ptr2[j+i*width]==45)
                        code=(ptr[(j-1)+(i-1)*width] <center) && (ptr[(j+1)+(i+1)*width] <center);
                    else if(ptr2[j+i*width]==135)
                        code=(ptr[(j+1)+(i-1)*width] <center) && (ptr[(j-1)+(i+1)*width] <center);
                    else
                    {
                        cerr << "FFF" << ptr2[j+i*width] << endl;
                    }



                }

                if(code==0)
                    ptr3[j+i*width]=0;

            }
        }

        mag2.copyTo(mag);
}


void EdgeDetector::connectedComponent(Mat hout)
{

    _lable=Mat(hout.rows,hout.cols,CV_8UC1);
    _lable.setTo(cv::Scalar::all(0));

    float *ptr1=(float *)hout.data;
    nMinx=1;
    nMaxx=s.width-2;
    nMiny=1;
    nMaxy=s.height-2;

    for(int i=nMiny;i<nMaxy;i++)
    {
        for(int j=nMinx;j<nMaxx;j++)
        {
            if(ptr1[j+i*s.width]>0 )
            {
            stackfill(j,i);
            }

        }
    }

}




void EdgeDetector::canny(Mat src,vector<Mat> &dst,vector<double> lower,vector<double> higher,int aperture)
{

    Mat fmag,fori,disp;

    s=Size(src.cols,src.rows);
    smoothing(src,src,aperture);


    if(src.channels()>1)
    colorGradient(src,fmag,fori,aperture);
    else
    {
    Mat dx,dy;
    //computing gradients along x and y direction
    cv::Sobel(src,dx,CV_32F,1,0,aperture,1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(src,dy,CV_32F,0,1,aperture,1, 0, cv::BORDER_REPLICATE);
    //compute magnitude and orientation of gradient
    cv::cartToPolar(dx,dy,fmag,fori,true);
    }


    //perform non maxima suppression
    nonMaxima(fmag,fori);
    Mat hout;


    dst.resize(lower.size());
    for(int i=0;i<lower.size();i++)
    {
    //apply lower and higher gradient thresholds
    cv::threshold(fmag,hout,lower[i],255,CV_THRESH_BINARY);
    cv::threshold(fmag,lout,higher[i],255,CV_THRESH_BINARY);
    connectedComponent(hout);
    _lable.copyTo(dst[i]);
    }


}


int EdgeDetector::getPixel(Point p)
{
     uchar * data=_lable.data;
     int val=data[p.x+p.y*s.width];
     return val;
}

bool EdgeDetector::check_color(Point p)
{
    int x=p.x;
    int y=p.y;
    if(x<nMinx || x >nMaxx || y<nMiny || y >nMaxy)
        return false;

    float *pl=(float*)lout.data;
    float val2=pl[x+y*s.width];


    int val=getPixel(p);
    if(val>0)
        return false;

    //check if pixel is possibly an edge
    if(val2>0)
        return true;
    else
        return false;

    return false;

}


void EdgeDetector::setPixel(Point p)
{
     uchar * data=_lable.data;
     data[p.x+p.y*s.width]=255;
}



void EdgeDetector::stackfill(int x,int y)
{
    points.push(Point(x,y));
    while(points.empty()==false)
    {
        Point p=points.top();
        points.pop();
        setPixel(p);


        for(int i=-1;i<=1;i++)
        {
            for(int j=-1;j<=1;j++)
            {
                if(i==0 && j==0)
                    continue;

                Point p1=Point(p.x+i,p.y+j);
                if(check_color(p1)==true)
                {
                    points.push(p1);
                }

            }
        }
    }

}
