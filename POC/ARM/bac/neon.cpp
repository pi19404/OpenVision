#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "helloneon-intrinsics.h"
#ifdef HAVE_NEON
#include <arm_neon.h>
#endif

#define DEBUG 1

#if DEBUG
#  define  D(x...)  printf("helloneon:%s",x)
#endif

using namespace std;
using namespace cv;
/* return current time in milliseconds */
static double now_ms(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0*res.tv_sec + (double)res.tv_nsec/1e6;
}






/**
 * @brief The Interleave class : This class encapsulates methods that perform the interleaving
 * and de-interleaving operations for a 3 channel BGR image .
 */
class Interleave
{
public:
    Interleave()
    {

    }


    /**
     * @brief process : this is the main function that performs interveaving or deinterleaving operation
     *                  based on the suppplied input operations code
     * @param src     : this is output/input multichannel image depending on operation code
     * @param dst     : this output/input array of channels that are required to be interleaved/de-interleaved
     * @param code    : 0-deinterleaving 1-interleaving
     */
    void process(Mat &src,vector<Mat> &dst,int code)
    {

        bool status=neon_process(src,dst,code);
    }

    bool neon_process(Mat &src,vector<Mat> &dst,int code)
    {
      #ifdef HAVE_NEON
        int width=src.cols;
        int height=src.rows;
        if(src.type()==CV_8UC3)
        {
        dst.resize(src.channels());
        for(int i=0;i<dst.size();i++)
        {
      	  dst[i]=Mat(height,width,CV_8UC1);
        }
        }
      uchar *d3=(uchar *)src.data;

      neon_interlace(d3,dst[0].data,dst[1].data,dst[2].data,width,height);

      return true;
      #endif
      return false;
    }

};



int main()
{

   char*  str;
   double  t0, t1, time_c, time_neon;


    Mat data1=imread("/mnt/sdcard/ReadingGlass/ReadingGlass_11.jpg",1);
    Mat a1;
    data1.copyTo(a1);
//    cv::cvtColor(data1,a1,CV_BGRA2BGR);
    cvtColor(a1,a1,CV_BGR2HSV);

    Interleave n1;
    vector<Mat> dst;
    t0 = now_ms();
    n1.process(a1,dst,0);
    t1 = now_ms();
    time_c = t1 - t0;
    asprintf(&str, "NEON : time take to load the image: %g ms\n", time_c);
    D(str);

    vector<Mat> ch;
    t0 = now_ms();
    split(a1,ch);
    t1 = now_ms();
    time_c = t1 - t0;
    asprintf(&str, "OPENCV : time take to load the image: %g ms\n", time_c);
    D(str);




}

