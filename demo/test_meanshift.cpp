#include <iostream>


using namespace std;

#include "ImgProc/meanshift.h"
using namespace ocv;
ocvmeanShift m;

cv::Rect* sArea;
bool areaDefined = false;
Mat img;
static bool pressed=false;
bool model=false;
void mouseEventHandler(int event, int x, int y,int flags, void *params) {
    static int x1,y1;


    if(pressed) {
        //rectangle(*(Mat*)params,Point(x1,y1),Point(x,y),Scalar(255,0,0) );
        sArea = new Rect(x1,y1,x-x1,y-y1);
    }
    if(event == CV_EVENT_LBUTTONDOWN) {
        x1 = x;
        y1 = y;
        pressed = true;
        areaDefined=false;
        model=false;
    }
    if(event == CV_EVENT_LBUTTONUP) {
        sArea = new Rect(x1,y1,x-x1,y-y1);

        pressed = false;
        areaDefined = true;
        model=false;
        //cerr << x1 <<":" << y1 <<":" << sArea->width <<":" << sArea->height << endl;
        //sArea->x=80;
        //sArea->y=88;
        //sArea->width=56;
        //sArea->height=82;
        rectangle(*(Mat*)params,*sArea,Scalar(0,255,0) );
    }
}



void callbackButton2(int state,void* userdata)
{

    vector<int> c;
    c.push_back(0);c.push_back(1);

    vector<int> size;
    size.push_back(50);size.push_back(50);
    size.push_back(255);
    vector<float> range;
    range.push_back(0);range.push_back(180);
    range.push_back(0);range.push_back(255);
    range.push_back(0);range.push_back(255);

    m.h.setChannel(c);
    m.h.setRange(range);
    m.h.setHistSize(size);

    cv::cvtColor(img,img,CV_BGR2HSV);
    m.buildModel(img,*sArea);
    cv::cvtColor(img,img,CV_HSV2BGR);
    model=true;

}

int test_meanshift(int argc,char **argv)
{


        VideoCapture cam;
	VideoWriter outputVideo;                                        // Open the output
	int mode=0;
        if(argc<2)
	mode=0;
	else
	mode=1;


	if(mode==0)
        cam = *new VideoCapture(0);
	else
	cam = *new VideoCapture(argv[1]);

        if( !cam.isOpened() ) {
            cout << "Failed to open camera/video "<<argv[1]  << endl;
            return 0;
        }

        Mat img1;//=imread("/media/software/Dropbox/Dropbox/repository/im/documents/images/image020.jpg",1);

if(mode==1)
{
 int ex = static_cast<int>(cam.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

    // Transform from int to char via Bitwise operators
    char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000)>> 24), 0};

    Size S = Size((int) cam.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) cam.get(CV_CAP_PROP_FRAME_HEIGHT));

 

    outputVideo.open("out.avi", CV_FOURCC('D','I','V','X'), cam.get(CV_CAP_PROP_FPS), S, true);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " <<endl;
        return -1;
    }
}
        Mat hsvImg;
        MatND hist1;
        char key = 10;
        cam >> img;
	img.copyTo(img1);

        //img1.copyTo(img);
        namedWindow("tracker",0);
        cvSetMouseCallback("tracker",mouseEventHandler,&img);
        cvCreateButton("Build Model",callbackButton2,0, CV_PUSH_BUTTON, 0);
        char text[100];
        sprintf(text,"similarity is %f",0);
	 model=false;
        while(key != 27) {
	   if(mode==0)	
	   {	
            cam >> img;
            cv::flip(img,img,1);
	    }
	    if(mode==1 && model==true)
	    {
		cam >> img;
	    }
	    if(img.rows==0)
		break;
	    if(mode==1 && model==false )
	    {
		img1.copyTo(img);
	    }
            if((pressed==true || areaDefined==true) && model==false)
            {
                rectangle(img,*sArea,Scalar(0,255,0),3);
            }
            if(model==true)
            {
                //Mat roi=img(*sArea);
                cv::cvtColor(img,img,CV_BGR2HSV);
                Rect r=m.run(img);
                cv::cvtColor(img,img,CV_HSV2BGR);
                cv::rectangle(img,r,Scalar(0,255,255),3,8);
            }
            imshow("tracker",img);
	   if(mode==1)
	   outputVideo << img;
            key = cvWaitKey(10);

        }
	if(mode==1)
	outputVideo.release();
}

int main(int argc,char **argv)
{
    test_meanshift(argc,argv);
    return 0;
}



