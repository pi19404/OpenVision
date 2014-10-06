#ifndef LINEDETECTOR_HPP
#define LINEDETECTOR_HPP
#include "Common/OpenCVCommon.hpp"
#include "others/ACE/localACE.hpp"
#include "others/fastSegment/segment.hpp"
using namespace cv;
class lineDetector
{
public:
    int N;
    int thresh;
    LocalACE p;
    segment s;
    lineDetector()
    {
    N=50;
    thresh=10;
    }
    void preProcess(Mat &image)
    {

        Mat im1;
        cv::resize(image,im1,Size(640,480));

        image=p.run(im1,20);
        imshow("original1",image);
        image=s.proces(image,1,300,20);
        imshow("r",image);
    }



    Mat CreateGaborFilterImage(char* demoName,   float lambda, float theta, float psi, float gaussvar, float gamma,Size size)
    {

        Mat ima1=cv::getGaborKernel(size,gaussvar,theta,lambda,gamma,psi,CV_32F);
        Mat d1=Mat(size.width,size.height,CV_8UC(3));
        d1.setTo(cv::Scalar::all(0));
        int mx=d1.cols/2;
        int my=d1.rows/2;
        Mat ima;
        cv::normalize(ima1,ima,-1,1,cv::NORM_MINMAX);
        float *ptr1=ima.ptr<float>();
        uchar *ptr2=d1.ptr<uchar>();
        float MinGaborValue = std::numeric_limits<float>::max();
        float MaxGaborValue = std::numeric_limits<float>::min();
        for (int dy=0; dy<size.height; dy++)
            {
                for (int dx=0; dx<size.width; dx++)
                {

                    double val=ptr1[dx+dy*size.width];
                    //cerr << val <<endl;
                    int R=0;
                    if (val>=0.1f)
                    {
                        R = (int)floor((val * 255.0f));

                        int sx=(size.width/2+(dx))%size.width;
                        sx=dx;
                        int sy=dy;//(size.height/2+(dy))%size.height;
                        ptr2[3*(sx)+(3*(sy*d1.cols))+0]=R;
                    }
                    else if(val <=-0.1f)
                    {
                        R = (int)floor((-val * 255.0f));
                        int sx=(size.width/2+(dx))%size.width;
                        int sy=dy;//(size.height/2+(dy))%size.height;
                        sx=dx;
                        //int sy=(size.width/2+(dy))%size.height;
                        ptr2[3*(sx)+(3*(sy*d1.cols))+2]=R;
                    }
                    if (val<MinGaborValue)
                    MinGaborValue = val;
                      if (val>MaxGaborValue)
                       MaxGaborValue = val;

                }
            }

        Mat d2;
        cv::resize(d1,d2,Size(256,256));
        imshow(demoName,d2);
        //imshow("image",ima1);
        waitKey(1000);
        return ima1;

    }

    Mat multiEdge(Mat image)
    {

        Mat pyr, timg, gray0(image.size(), CV_8U), gray;
        Mat gray1,gray2,gray3;
        // down-scale and upscale the image to filter out the noise
        pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
        pyrUp(pyr, timg, image.size());
        vector<vector<Point> > contours;
        vector<Mat> channels;

        cv::split(timg,channels);

        Mat disp;
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(channels[0], gray1, 0, thresh, 5);
                Canny(channels[1], gray2, 0, thresh, 5);
                Canny(channels[2], gray3, 0, thresh, 5);

                cv::max(gray1,gray2,gray1);
                cv::max(gray1,gray3,gray);
                dilate(gray, gray0, Mat(), Point(-1,-1));
                gray0.copyTo (gray);

            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0

                Canny(channels[0], gray1, 0, thresh+((l+1)*1000/N), 5);
                Canny(channels[1], gray2, 0, thresh+((l+1)*1000/N), 5);
                Canny(channels[2], gray3, 0, thresh+((l+1)*1000/N), 5);

                cv::max(gray1,gray2,gray1);
                cv::max(gray1,gray3,gray2);
               // Mat kernel=cv::getStructuringElement (cv::MORPH_CROSS,Size(3,3));
                //dilate(gray2, gray0,Mat(), Point(-1,-1),1);
                cv::normalize(gray2,gray2,0,10,cv::NORM_MINMAX);
                gray2.copyTo (gray);
                if(l==1)
                    gray.copyTo(disp);
                else
                    cv::add(gray,disp,disp);
            }


            //cv::resize(gray,disp,Size(320,240));
            //imshow("DDDD",disp);
       //     imshow("image",image);

         //  cv::waitKey(0);
        }

        return disp;

    }

public:
    Mat run(Mat image,float lambda=0,float theta=0,float psi=0,float gaussvar=0,float gamma=0,Size size=Size(0,0))
    {

        Mat dest,dest1;
        Mat ddd;
        image.copyTo(ddd);
        preProcess(image);
        /*image.convertTo(image, CV_32F, 1.0/255.0, 0);
        /*lambda=0;
        lambda=0.5+lambda/100.0;
        theta=30*CV_PI/180;
        psi=0*CV_PI/180;
        gaussvar=3;
        gamma=1;
        //size=Size(15,15);
        Mat kernel=CreateGaborFilterImage("AA",lambda,theta,psi,gaussvar,gamma,size);
        cv::filter2D(image, dest, CV_32F, kernel);
        //theta=120*CV_PI/180;
        Mat kernel1=CreateGaborFilterImage("AA",lambda,theta,psi,gaussvar,gamma,size);
       cv::filter2D(image, dest1, CV_32F, kernel1);
        cv::add(dest,dest1,dest);

         imshow("DDD",dest);
         //cv::filter2D(image, dest, CV_32F, kernel);
         //
         */




         Mat disp=multiEdge(image);
         imshow("DDD1",disp);
         Mat disp1;
         Ptr<CLAHE> clahe = createCLAHE();
         clahe->setClipLimit(4);
         Mat dx;
         cvtColor(ddd,dx,CV_BGR2GRAY);
         clahe->apply(dx,disp1);
         cv::threshold(disp1,disp1,200,255,cv::THRESH_BINARY);

         imshow("DDD2",disp1);


       //  cv::waitKey(0);

        //
         return disp;


    }


};

#endif // LINEDETECTOR_HPP
