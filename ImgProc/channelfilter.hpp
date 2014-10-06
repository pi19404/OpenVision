#ifndef CHANNELFILTER_H
#define CHANNELFILTER_H
#include "Common/OpenCVCommon.hpp"
//class that performs channel filtering
class channelFilter
{
public:
    channelFilter();
    Mat kernel;


    //function to apply the channel filter
    void apply(Mat &src,Mat &dst);
};

#endif // CHANNELFILTER_H
