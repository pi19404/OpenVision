#ifndef LBPFEATURES_H
#define LBPFEATURES_H
#include "Common/OpenCVCommon.hpp"
#include "ImgFeatures/integralImage.h"
class LBPFeatures
{
public:
    LBPFeatures()
    {
     initUniform();
    }
    Mat image;
    vector<uchar> features;
    Mat mask;
    IntegralImage ix;
    vector<int> lookup;


    //naive count number of set bits
    bool countSetBits1(int code)
    {
      int count=0;
      while(code!=0)
      {
      if(code&&0x01)
      count++;
      code=code>>1;
      }
      return count;
    }

    // Brian Kernighan's method to count set bits
    bool countSetBits(int code)
    {
      int count=0;
      int v=code;
      for(count=0;v;count++)
      {
      v&=v-1; //clears the LSB
      }
      return count;
    }


    //checking if code is uniform or not
    bool checkUniform(int code)
    {
      int b = code >> 1;
      int c = code ^ b;
      int count=countSetBits(c);
      if ((count%2)==0 || count==0 || count==1)
          return 1;
      else
          return 0;
    }

    //3x3 neighborhood will have 8 neighborhood pixels
    //all non uniform codes are assigned to 59
    void initUniform()
    {
        lookup.resize(59);
        int index=0;
        for(int i=0;i<255;i++)
        {
            bool status=checkUniform(i);
            if(status==true)
            {
                lookup[i]=index;
                index++;
            }
            else
            {
                lookup[i]=59;
            }
        }
    }


    //function to compute LBP image
    //TBD : check if grayscale encoding can be done
    //can be used to encode orientation changes of gradient
    //lbp encodes information about different types of gradients
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

                //for(int k=7;k>=0;k++)
                {
                code|=((int)ptr[(j-1)+(i-1)*width] >=center)<<7;
                code|=((int)ptr[j+(i-1)*width] >=center)<<6 ;
                code|=((int)ptr[(j+1)+(i-1)*width] >=center)<<5 ;
                code|=((int)ptr[(j+1)+(i)*width] >=center)<<4 ;
                code|=((int)ptr[(j+1)+(i+1)*width] >=center)<<3 ;
                code|=((int)ptr[j+(i+1)*width] >=center)<<2 ;
                code|=((int)ptr[j-1+(i+1)*width] >=center)<<1 ;
                code|=((int)ptr[j-1+(i)*width] >=center)<<0 ;
                }
                int check=0;
                check=code;
                //heck if the code is uniform code
                //encode only if it is a uniform code else
                //assign it a number 255
                optr[j+i*width]=lookup[code];

            }
        }

    }

    void spatialHistogram(Mat lbpImage)
    {
        uchar *ptr=lbpImage.data;
        vector<float> histogram;
        histogram.resize(58);
    /*    for(int i=0;i<lbpImage.rows;i++)
        {
            for(int j=0;j<lbpImage.cols;j++)
            {
                if(code==128)
                {

                }
            }
        }
*/

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
