#include <iostream>

#include "ImgProc/Histogram.hpp"
using namespace std;

int test_histogram(int argc, char** argv )
    {
      Mat i1,i2,i3,i4;

      /// Load three images with different environment settings
      if( argc < 3 )
        { printf("** Error. Usage: ./compareHist_Demo <image_settings0> <image_setting1> <image_settings2>\n");
          return -1;
        }

      //cerr<< argv[1] <<":" << argv[2] << endl;
      i1 = imread( argv[1], 1 );
      i2 = imread( argv[2], 1 );
      //cerr << i2.channels() << endl;
      //i3 = imread( argv[3], 1 );
      //i4 = imread( argv[4], 1 );

      cvtColor( i1,i3, CV_BGR2HSV );
      cvtColor( i2,i4, CV_BGR2HSV );
      //cvtColor( i3,i3, CV_BGR2HSV );
      //cvtColor( i4,i4, CV_BGR2HSV );

      Histogram h1,h2,h3,h4;
      vector<int> channel;
      channel.push_back(0);
      //channel.push_back(1);
      //channel.push_back(2);
      h1.setChannel(channel);
      h2.setChannel(channel);
      //h3.setChannel(channel);
      //h4.setChannel(channel);
      vector<int> size;
      size.push_back(50);
      size.push_back(60);
      size.push_back(40);
      h1.setHistSize(size);
      h2.setHistSize(size);

      h1.BuildHistogram(i3);
      h2.BuildHistogram(i4);
      //h3.BuildHistogram(i3);
      //h4.BuildHistogram(i4);
      Mat h11=h1.drawHist();
      imshow("Hist 1",h11);
      h11=h2.drawHist();
      imshow("Hist 2",h11);

       cv::waitKey(0);


      double r1=h1.compareHist(h2,Histogram::comparison_method::BHATTACHRYA);
      double r2=h1.compareHist(h2,Histogram::comparison_method::CHI_SQUARED);
      double r3=h1.compareHist(h2,Histogram::comparison_method::INTERSECTION);
      double r4=h1.compareHist(h2,Histogram::comparison_method::CORRELATION);

      cerr << r1 << ":" << r2 << ":" << r3 <<":" << r4 << endl;

      r1=cv::compareHist(h1.getHist(),h2.getHist(),CV_COMP_BHATTACHARYYA);
      r2=cv::compareHist(h1.getHist(),h2.getHist(),CV_COMP_CHISQR);
      r3=cv::compareHist(h1.getHist(),h2.getHist(),CV_COMP_INTERSECT);
      r4=cv::compareHist(h1.getHist(),h2.getHist(),CV_COMP_CORREL);

      cerr << r1 << ":" << r2 << ":" << r3 <<":" << r4 << endl;
       cv::waitKey(0);

}



int main(int argc,char **argv)
{
    cout << "Hello World!" << endl;
    test_histogram(argc,argv);
    return 0;
}


