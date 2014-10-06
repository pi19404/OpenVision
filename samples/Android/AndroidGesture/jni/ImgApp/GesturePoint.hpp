#ifndef GESTUREPOINT_HPP
#define GESTUREPOINT_HPP
#include "Common/OpenCVCommon.hpp"
#include <iostream>

using namespace cv;


class GesturePoint
{
public:
Point2f position;
double time;
double getTime();
Point2f getPosition();

};


#endif
