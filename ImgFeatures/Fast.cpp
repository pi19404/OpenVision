#include "Fast.hpp"

FeatureDetection::Fast::Fast()
{

    qualityLevel=0.1;
     maxCorners=100;
     minDistance=10;
    threshold=10;
    K=12;
    for( int i = -255; i <= 255; i++ )
        threshold_tab[i+255] = (uchar)(i < -threshold ? 1 : i > threshold ? 2 : 0);

    offset.push_back (Point2f(0,-3));
    offset.push_back (Point2f(1,-3));
    offset.push_back (Point2f(2,-2));
    offset.push_back (Point2f(3,-1));
    offset.push_back (Point2f(3,0));
    offset.push_back (Point2f(3,1));
    offset.push_back (Point2f(2,2));
    offset.push_back (Point2f(1,3));
    offset.push_back (Point2f(0,3));
    offset.push_back (Point2f(-1,3));
    offset.push_back (Point2f(-2,2));
    offset.push_back (Point2f(-3,1));
    offset.push_back (Point2f(-3,0));
    offset.push_back (Point2f(-3,-1));
    offset.push_back (Point2f(-2,-2));
    offset.push_back (Point2f(-1,-3));



init=false;
}

vector<Point2f> FeatureDetection::Fast::run (Mat src)
{

    if(init==false)
{
        int a=0;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=8;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=2;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=10;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=4;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=12;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=6;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=14;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=1;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=9;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=3;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=11;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=5;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=13;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=7;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        a=15;checks.push_back ((int)offset[a].y*src.step+(int)offset[a].x);
        init=true;

        for(int i=0;i<offset.size ();i++)
            checks1.push_back ((int)offset[i].y*src.step+(int)offset[i].x);
    }
    vector<Point2f> corners;
    vector<Point2f> corners1;

    Mat dst;
    src.copyTo (dst);

    dst.convertTo (dst,CV_32FC(1),1,0);
    dst.setTo (cv::Scalar::all (0));
    //int value[offset.size ()];
    //parsing the image rejecting the border pixels
        for(int i = 3; i < src.rows-2; i++)
        {

            float * ptr1=dst.ptr <float>(i);
            uchar * ptr=src.data+i*src.step;
            for(int j=3;j<src.cols-2;j++)
            {
                //getting the value of the test point
                int v1=ptr[j];
                int flag=1;
                int a,b;

                for(int k=0;k<checks.size ();k=k+2)
                {

                a=checks[k];
                b=checks[k+1];
                int value1=threshold_tab[-ptr[j+a]+v1+255];
                int value2=threshold_tab[-ptr[j+b]+v1+255];
                flag=flag&(value1|value2);
                if(flag==0)
                break;

                }

                if(flag==0)
                continue;



                int count=0;
                int response_tmp=0;
                for(int k=0;k<2;k++)
                {

                count=0;
                response_tmp=0;
                //check line segment
                for(int l=0;l<offset.size();l++)
                {
                    //if pixel < test_pixel - t
                    int index=j+checks1[l];
                    int x=ptr[index];

                    int flag1=(k==0)?(x<v1-threshold):(x>v1+threshold);
                    count=(flag1==1)?count+1:0; //set count to 0 if no suitable pixels found
                    //computing the response function
                    response_tmp=(flag1==1)?response_tmp+std::abs(x-v1-threshold):0;


                    //computing harris corner response


                    if(count > K ) break;	//12 continous pixel detected
                }

                if(count>K)
                {
                ///corners1.push_back(Point2f(j,i));

                ptr1[j]=response_tmp;
                //response.push_back (response_tmp);
                break;
                }

                }
            }
        }

       //computing the harris scores for corner filtering
      //using kepoint structure


        filter_corners (dst);


        _subPixel.RefineCorner (src,corners);


        return corners;

}

