#include "channelfilter.hpp"

channelFilter::channelFilter()
{
}
void channelFilter::apply(Mat &src,Mat &dst)
{
    int ch=src.channels();
    Mat row=Mat(ch,1,CV_32F);
    dst.create(src.rows,src.cols,CV_32FC(ch));
    for(int i=0;i<src.rows;i++)
    {
        float *ptr=src.ptr<float>(i);
        float *dptr=dst.ptr<float>(i);
        for(int j=0;j<src.cols*ch;j=j+ch)
        {
            #pragma unroll
            for(int k=0;k<ch;k++)
            {
                row.at<float>(k)=ptr[j+k];
            }

            Mat res=kernel*row;
            #pragma unroll
            for(int k=0;k<ch;k++)
            {
                dptr[j+k]=res.at<float>(k);
            }
        }
    }

}
