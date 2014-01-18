#ifndef LBPFEATURES_H
#define LBPFEATURES_H
#include "Common/OpenCVCommon.hpp"
#include "ImgFeatures/integralImage.h"
class LBPFeatures
{
public:
    LBPFeatures(){};
    Mat image;
    vector<uchar> features;
    Mat mask;
    IntegralImage ix;


    //function to compute LBP image
    void compute(Mat image,Mat &dst)
    {
        uchar *ptr=image.data;
        image.copyTo(dst);
        uchar *optr=dst.data;
        int width=image.cols;
        int height=image.rows;

        for(int i=1;i<height-1;i++)
        {
            for(int j=0;j<width-1;j++)
            {
                int center=(int)ptr[j+i*width];
                unsigned char code=0;
                code|=((int)ptr[(j-1)+(i-1)*width] >=center)<<7 ;
                code|=((int)ptr[j+(i-1)*width] >=center)<<6 ;
                code|=((int)ptr[(j+1)+(i-1)*width] >=center)<<5 ;
                code|=((int)ptr[(j+1)+(i)*width] >=center)<<4 ;
                code|=((int)ptr[(j+1)+(i+1)*width] >=center)<<3 ;
                code|=((int)ptr[j+(i+1)*width] >=center)<<2 ;
                code|=((int)ptr[j-1+(i+1)*width] >=center)<<1 ;
                code|=((int)ptr[j-1+(i)*width] >=center)<<0 ;
                optr[j+i*width]=code;
            }
        }

    }

    void computeBlock(Mat image,Mat & dst,int block=2)
    {
        ix.compute(image);
        image.copyTo(dst);
        dst.setTo(cv::Scalar::all(0));
        int width=image.cols;
        int height=image.rows;
        for(int i=block;i<height-block;i=i+block)
        {
            for(int j=block;j<width-block;j=j+block)
            {
                int x=i;
                int y=j;
                Rect r=Rect(j,i,block,block);
                int meanv=ix.calcMean(r);
                int code=0;
                for(int k=0;k<8;k++)
                {
                    switch(k)
                    {
                    case 0:
                        y=i-block;
                        x=j-block;
                    break;
                    case 1:
                        y=i;
                        x=j-block;
                    break;
                    case 2:
                        y=i+block;
                        x=j-block;
                    break;
                    case 3:
                        y=i+block;
                        x=j;
                    break;
                    case 4:
                        y=i+block;
                        x=j+block;
                    break;
                    case 5:
                        y=i;
                        x=j+block;
                    break;
                    case 6:
                        y=i-block;
                        x=j+block;
                    break;
                    case 7:
                        y=i-block;
                        x=j;
                    break;
                    default:
                    break;
                    }
                    Rect r1=Rect(x,y,block,block);
                    int val=(int)ix.calcMean(r1);
                    code|=(meanv >= val)<<(7-k);

                }
                Mat roi=dst(r);
                roi.setTo(cv::Scalar::all(code));


            }
        }


    }





};

#endif // LBPFEATURES_H
