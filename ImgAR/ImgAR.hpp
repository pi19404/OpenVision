#ifndef IMGAR_HPP
#define IMGAR_HPP

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#include "opencv.hpp"

using namespace cv;
class ImgAR
{
    int thresh;
    //camera calibration file
    char cparam_name[100];
    //structure to hold camera calibration information
    ARParam cparam;
    //pattern name
    char patt_name[100];
    //pattern id
    int patt_id;
    //pattern physical width
    int patt_width;
    //pattern center in the image
    double          patt_center[2];
    //camera pose transformation matrix
    double          patt_trans[3][4];
    //AR Marker tracking status
    int contF;
    //bool intialization flag
    bool flag;
    //threshold for marker detection
    int thresh;
    //image size
    Size size;

    ImgAR(char *cparam,char *pattern,int pwidth)
    {
        strcpy(cparam_name,cparam);
        strcpy(patt_name,pattern);
        patt_width=pwdith;
        patt_center[0]=0;
        patt_center[1]=0;
        patt_id=-1;
        size.width=320;
        size.height=240;
        initAR();
    }

    void run(Mat frame)
    {

        Mat image;
        //preprocessing to improve the image
        cv::resize(frame,image,size,0,0);
        cvtColor(image,image,CV_BGR2RGB);
        //perform contrast stretching
        cv::normalize(image,image,0,255,cv::NORM_MINMAX);
        //blurring to reduce noise
        cv::GaussianBlur(image,image,Size(5,5),1,0);

        //AR Marker Image Pointer
        ARUint8         *dataPtr;
        //Pointer to store the Marker info
        ARMarkerInfo    *marker_info;
        //number of markers detected
        int marker_num=0;

        int             j, k;
        k=-1;
        dataPtr = (ARUint8 *)image.data;

        //detect marker
        if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
               contF=-1;
               skip=true;

        }

        //validate the marker
        k = -1;
        for( j = 0; j < marker_num; j++ ) {
            if( patt_id == marker_info[j].id ) {
                   if( k == -1 ) k = j;
                   else if( marker_info[k].cf < marker_info[j].cf ) k = j;
              }
            }

        if( k == -1 ) {
             contF = 0;

             argDrawMode2D();
             argDispImage( dataPtr,0,0);

            return 0;
          }


        if(contF == 0 ) {
            //if tracking for the first time
            arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
            contF=1;
           }else{
            //if marker previously detected
              arGetTransMatCont(&marker_info[k], patt_trans, patt_center, patt_width, patt_trans);
          }

        ARMarkerInfo tmp=marker_info[k];

        cv::circle(image,center,5,Scalar(0,0,255),-1,8);
        for(int i=0;i<4;i++)
        {
            Point p1=Point(tmp.vertex[(tmp.dir+i)%4][0],tmp.vertex[(tmp.dir+i)%4][1]);
            Point p2=Point(tmp.vertex[(tmp.dir+i+1)%4][0],tmp.vertex[(tmp.dir+i+1)%4][1]);
            cv::line(image,p1,p2,Scalar(0,255,255),1,8);
        }

        dataPtr=(ARUint8 *)image.data;
        argDrawMode2D();
        argDispImage( dataPtr,0,0);

          display();
          cv::waitKey(10);



    }

    void initDisplay()
    {
        glutInit(0,0);
        /* open the graphics window */
        argInit( &cparam, 1.0, 0, 0, 0, 0 );
    }

     void initAR( void )
    {
        ARParam  wparam;


        /* set the initial camera parameters */
        if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
            printf("Cam era parameter load error !!\n");
            exit(0);
        }
        arParamChangeSize( &wparam, xsize, ysize, &cparam );
        arInitCparam( &cparam );
        //printf("*** Camera Parameter ***\n");
        //arParamDisp( &cparam );

        if( (patt_id=arLoadPatt(patt_name)) < 0 ) {
            printf("pattern load error !!\n");
            exit(0);
        }
        initDisplay();
    }

    /* cleanup function called when program exits */
    void cleanup(void)
    {
        argCleanup();
    }


    void display()
    {
        double    gl_para[16];
        GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
        GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
        GLfloat   mat_flash_shiny[] = {50.0};
        GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
        GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
        GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

        argDrawMode3D();
        argDraw3dCamera( 0, 0 );
        glClearDepth( 1.0 );
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        /* load the camera transformation matrix */
        argConvGlpara(patt_trans, gl_para);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd( gl_para );

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMatrixMode(GL_MODELVIEW);
        int size=50;

        glTranslatef(0,0,25);//world co-ordinate to pixel co-ordinate


        glutSolidCube(size);
        glDisable( GL_LIGHTING );

        glDisable( GL_DEPTH_TEST );
        argSwapBuffers();
    }

};


int xc,yc,zc;
int px,py,pz;


static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
Mat frame;
int main()
{

int             thresh = 100;
char           *cparam_name    = "/media/windows/pi19404/REPO/Infurnia/ARToolKit/bin/Data/camera_para.dat";
char           *patt_name      = "/media/windows/pi19404/REPO/Infurnia/ARToolKit/bin/Data/android.patt";

ImgAR ar1;
ar=ImgAR(cparam_name,patt_name,80);
cv::namedWindow("image1",1);

createTrackbar( "Translate x", "image1",&xc,2000,0);
createTrackbar( "Translate y ","image1",&yc,1000,0);
createTrackbar( "Translate z ","image1",&zc,1000,0);
createTrackbar( "threshold", "image1",&thresh,255,0);
VideoCapture cam;
cam = *new VideoCapture(0);
if( !cam.isOpened() ) {
     cout << "Failed to open camera" << endl;
     return 0;
}

int key=0;
while(key != 27) {
    cam >> frame;
    ar1.run(frame);
    key=waitKey(10);
    }

    cv::waitKey(0);


}






#endif // IMGAR_HPP
