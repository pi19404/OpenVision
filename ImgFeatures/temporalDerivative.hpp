#ifndef TERMPORALFILLTER_HPP
#define TERMPORALFILLTER_HPP
#include "Common/OpenCVCommon.hpp"
class temporalDerivative
{
public:

    int N;
    vector<Mat> _images;
    int index;
    bool init;


    temporalDerivative(){
        init=false;
        N=3;
        index=0;
        _images.resize(N);
    }

    int getIndex(int index)
    {
        if(index<0)
            index=index+N;
        if(index>N)
            index=index-N;
        return index;
    }

    void updateIndex()
    {
        index=index+1;
        index=index%N;
    }

    ///termporal derivative filter
    Mat temporalDerivativeFilter(Mat gray)
    {
        Mat tmp;
        gray.copyTo(_images[index]);
        if(init==false)
            return gray;
        if(index==1)
            init=true;

        int prev_index=getIndex(index-1);
        int next_index=getIndex(index+1);
        int curr_index=index;

        cv::absdiff(_images[prev_index],_images[next_index],tmp);
        cv::absdiff(tmp,-2*_images[curr_index],tmp);

        index=(index+1)%N;
        return tmp;
    }



};

#endif // TERMPORALFILLTER_HPP
