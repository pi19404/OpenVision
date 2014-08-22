#include "UniStrokeGestureRecognizer.hpp"


using namespace cv;
/**
 *  function that stores the Gesture to a specified directory
 */

Rect UniStrokeGestureRecognizer::boundingBox(vector<GesturePoint> points)
 {
     double minX =  9999;
     double maxX = -9999;
     double minY =  9999;
     double maxY = -9999;


     for (int i = 0;i<points.size();i++)
     {
         Point2f point=points[i].position;
         if (point.x < minX)
             minX = point.x;
         if (point.x > maxX)
             maxX = point.x;
         if (point.y < minY)
             minY = point.y;
         if (point.y > maxY)
             maxY = point.y;
     }
     Rect bounds(minX, minY, (maxX - minX), (maxY - minY));
     return bounds;
 }

vector<GesturePoint> UniStrokeGestureRecognizer::scale(vector<GesturePoint> p)
{
    double minx=9999;
    double miny=9999;
    double maxx=-9999;
    double maxy=-9999;
    vector<GesturePoint> p1;
    p1.resize(p.size());
    for(int i=0;i<p.size();i++)
    {
        minx=std::min(minx,(double)p[i].position.x);
        miny=std::min(miny,(double)p[i].position.y);
        maxx=std::max(maxx,(double)p[i].position.x);
        maxy=std::max(maxy,(double)p[i].position.y);
    }
    double s=std::max(maxx-minx,maxy-miny);

    for(int i=0;i<p.size();i++)
    {
        p1[i].position.x=(p[i].position.x - minx)/s;
        p1[i].position.y=(p[i].position.y - miny)/s;
    }

    return p1;

}
void UniStrokeGestureRecognizer::save(string dir,vector<GesturePoint> points)
{

    char abspath1[1000],abspath2[1000];
    sprintf(abspath1,"%s/%s",_path.c_str(),dir.c_str());

    int count=0;

    //check if directory exists else create it
    int ret=ImgUtils::createDir((const char *)abspath1);

     count=ImgUtils::getFileCount(abspath1,"csv");

     //if(count!=0)
     //    count++;

     sprintf(abspath2,"%s/%d.csv",abspath1,count);

     ofstream file(abspath2,std::ofstream::out);
     for(int i=0;i<points.size();i++)
     {
         GesturePoint p=points[i];
         file << p.position.x <<",";
         file << p.position.y <<",";
     }

     file.close();

     generateBitmap(abspath2);


}





void UniStrokeGestureRecognizer::generateBitmap(string file)
{
   string basedir=ImgUtils::getBaseDir(file);
   string name=ImgUtils::getBaseName(file);
   string line;
   //ifstream classFile(file.c_str());

   float x,y;
   cv::Mat image=cv::Mat(640,480,CV_8UC3);
   image.setTo(cv::Scalar::all(0));
   Point x1,x2,x3;
   int first=-1;
   int delta=20;
   vector<GesturePoint> points;
   points=loadTemplateFile(file.c_str(),"AA");

   Rect R=boundingBox(points);


   int i=0;
   cv::circle(image,cv::Point((int)points[i].position.x,(int)points[i].position.y),3,cv::Scalar(255,255,0),-1,CV_AA);
   for(i=1;i<points.size();i++)
   {
       Point x1=cv::Point((int)points[i].position.x,(int)points[i].position.y);
       Point x2=cv::Point((int)points[i-1].position.x,(int)points[i-1].position.y);
       cv::circle(image,x1,3,cv::Scalar(255,255,0),-1,CV_AA);
       cv::line(image,x2,x1,Scalar(0,255,255),3,CV_AA);
   }

   R=Rect(max(0,R.x-delta),max(0,R.y-delta),R.width+2*delta,R.height+2*delta);
   if(R.x+R.width>image.cols)
       R.width=image.cols-R.x-1;
   if(R.y+R.height>image.rows)
       R.height=image.rows-R.y-1;


   Mat roi=image(R);
   Mat dst;
   cv::resize(roi,dst,Size(640,480));
   string bmpname=basedir+"/"+name+".bmp";
   cv::imwrite(bmpname,dst);



}


void UniStrokeGestureRecognizer::getTemplateList(string dir)
{
  vector<string> templates=ImgUtils::getSubDirs((char *)_path.c_str());


}


//get a list of template files
void getTemplateFiles(string name);
/*void UniStrokeGestureRecognizer::generateBitMaps()
{


}*/
/**
 *  The functions loads all the gesture templates
 */
void UniStrokeGestureRecognizer::load()
{
    loadTemplates();
}

/**
 * load the gestures corresponding to a specified gesture class,defined by the sub-directory name
 */
vector<GesturePoint> UniStrokeGestureRecognizer::loadTemplateFile(string name,string dir)
{


    vector<GesturePoint> p;
    ifstream file(name.c_str());
    string value;
    GesturePoint tmp;
    int i=0;
    while(file.good())
    {
         p.resize(0);
         getline(file,value);
         std::istringstream s(value.c_str());
         std::string field;
         while (getline(s, field,','))
         {
             if(i==0)
             {
                 tmp.position.x=atof(field.c_str());
                 i++;
                 continue;
             }
             else if(i==1)
             {
                tmp.position.y=atof(field.c_str());
                p.push_back(tmp);
                i=0;
             }
         }

    break;
    }
    file.close();
    return p;
}

/**
 *  load all the gesture templates
 */
void UniStrokeGestureRecognizer::loadTemplates()
{

      DIR           *d;
      struct dirent *dir;

      DIR           *d1;
      struct dirent *dir1;

      vector<string> dirlist;
      int i=0;
      d = opendir(_path.c_str());
      if (d)
      {
        while ((dir = readdir(d)) != NULL)
        {
          UniStrokeGesture sample;
          if(!(strcmp(dir->d_name,".") && strcmp(dir->d_name,"..")))
          continue;

          char abspath[1000],abspath1[1000];
          sprintf(abspath,"%s/%s",_path.c_str(),dir->d_name);

          d1 = opendir(abspath);
          if(d1)
          {

          while ((dir1 = readdir(d1)) != NULL)
          {
              if(!(strcmp(dir1->d_name,".") && strcmp(dir1->d_name,"..")))
              continue;

           sprintf(abspath1,"%s/%s",abspath,dir1->d_name);

          vector<GesturePoint> t=loadTemplateFile(string(abspath1),dir->d_name);

          sample._gesturePoint.push_back(t);

          }
          closedir(d1);
          }
          else
          {
              cerr << "error in opening dir " << abspath << endl;
          }
          sample._name=dir->d_name;
          _gestures.push_back(sample);
          i++;
        }
        closedir(d);
      }



}

/**
 * set the name of the template gesture directory
 */
void UniStrokeGestureRecognizer::setPath(string name)
{
    _path=name;
}
