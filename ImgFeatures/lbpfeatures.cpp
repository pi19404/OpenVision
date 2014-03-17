#include "lbpfeatures.hpp"



LBPFeatures::LBPFeatures()
{
    sizeb=58;
}


// Brian Kernighan's method to count set bits
int LBPFeatures::countSetBits(int code)
{
  int count=0;
  int v=code;
  for(count=0;v;count++)
  {
  v&=v-1; //clears the LSB
  }
  return count;
}

int LBPFeatures::rightshift(int num, int shift)
{
    return (num >> shift) | ((num << (8 - shift)&0xFF));
}


bool LBPFeatures::checkUniform(int code)
{
    int b = rightshift(code,1);
  ///int d = code << 1;
  int c = code ^ b;
  //d= code ^d;
  int count=countSetBits(c);
  //int count1=countSetBits(d);
  if (count <=2 )
      return true;
  else
      return false;
}


void LBPFeatures::initUniform()
{
    lookup.resize(255);
    int index=0;
    for(int i=0;i<=255;i++)
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

    initHistogram();

}


void LBPFeatures::compute(Mat image,Mat &dst)
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

void LBPFeatures::initHistogram()
{
    vector<int> channel;
    channel.push_back(0);
    hist.setChannel(channel);
    vector<int> size;
    size.push_back(sizeb+1);
    hist.setHistSize(size);

}

Mat LBPFeatures::computeHistogram(Mat cell)
{
    Mat tmp_hist;
    hist.BuildHistogram(cell);
    hist.getHist().copyTo(tmp_hist);
    //tmp_hist=tmp_hist/tmp_hist.total();
    return tmp_hist;
}



vector<float> LBPFeatures::spatialHistogram(Mat lbpImage,Size grid)
{
    vector<float> histogram;
    histogram.resize(grid.width*grid.height*sizeb);
    int width=lbpImage.cols/grid.width;
    int height=lbpImage.rows/grid.height;
    int cnt=0;
    //#pragma omp parallel for
    for(int i=0;i<grid.height;i++)
    {
        for(int j=0;j<grid.width;j++)
        {
            Mat cell=lbpImage(Rect(j*width,i*height,width,height));
            Mat cell_hist=computeHistogram(cell);
            //imshow("FFF",cell_hist);
            Mat tmp_feature;
            cell_hist.reshape(1,1).convertTo(tmp_feature,CV_32FC1);
            float * ptr=tmp_feature.ptr<float>(0);
            for(int k=0;k<tmp_feature.cols-1;k++)
            {
                if(ptr[k]==0)
                    ptr[k]=1.0/sizeb;
                histogram[(cnt*sizeb)+k]=ptr[k];
              //  cerr << ptr[k] << endl;
            }
            cnt++;
        }
    }

    return histogram;
}


void LBPFeatures::computeBlock(Mat image,Mat & dst,int block)
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
                    y=i-block;
                    x=j;
                break;
                case 2:
                    y=i-block;
                    x=j+block;
                break;
                case 3:
                    y=i;
                    x=j+block;
                break;
                case 4:
                    y=i+block;
                    x=j+block;
                break;
                case 5:
                    y=i+block;
                    x=j;
                break;
                case 6:
                    y=i+block;
                    x=j-block;
                break;
                case 7:
                    y=i;
                    x=j-block;
                break;
                default:
                break;
                }
                Rect r1=Rect(x,y,block,block);
                int val=(int)ix.calcMean(r1);
                code|=(meanv >= val)<<(7-k);


            }
            code=lookup[code];
            Mat roi=dst(r);
            roi.setTo(cv::Scalar::all(code));


        }
    }


}






