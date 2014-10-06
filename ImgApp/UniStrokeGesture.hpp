#ifndef UNISTROKEGESTURE_HPP
#define UNISTROKEGESTURE_HPP
#include "ImgApp/GesturePoint.hpp"
#include "iostream"
#include "dirent.h"
#include <fstream>

using namespace std;


class UniStrokeGesture
{
public:
    vector<vector<GesturePoint> > _gesturePoint;
    string _name;
};

#endif

