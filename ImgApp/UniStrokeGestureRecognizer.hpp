#ifndef UNISTROKEGESTURERECOGNIZER_HPP
#define UNISTROKEGESTURERECOGNIZER_HPP
#include "ImgApp/GesturePoint.hpp"
#include <iostream>
#include <fstream>
#include "ImgApp/UniStrokeGesture.hpp"
#include "ImgUtils/ImgUtils.hpp"

using namespace std;

class UniStrokeGestureRecognizer
{
public:

//saving the gesture
void save(string name,vector<GesturePoint> points);

//loading all the gestures
void load();

//setting the user directory path for gesture recognizer
void setPath(string name);

private:
string _path;
//dictionary of unistroke gestures
vector<UniStrokeGesture> _gestures;
//loading template file
vector<GesturePoint> loadTemplateFile(string name,string dir);

//generate thumbnails for all the template files
void generateBitmap(string dir);

//get list of template classes
void getTemplateList(string dir);

//get a list of template files
void getTemplateFiles(string name);

//loading all the templates
void loadTemplates();


//scale the gesture points to bounding box
vector<GesturePoint> scale(vector<GesturePoint> p);

//get the bounding box for gestures
Rect boundingBox(vector<GesturePoint> points);
};
#endif // UNISTROKEGESTURERECOGNIZER_HPP
