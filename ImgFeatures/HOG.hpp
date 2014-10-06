#ifndef HOG_HPP
#define HOG_HPP
#include "Common/OpenCVCommon.hpp"
#include "opencv2/nonfree/nonfree.hpp"

void computeDescriptors(Mat objectImg,Mat sceneImg)
{
    Mat outImg1,outImg2;
    cv::FeatureDetector * detector = new cv::OrbFeatureDetector();
    cv::DescriptorExtractor * extractor = new cv::OrbDescriptorExtractor();

    std::vector<cv::KeyPoint> objectKeypoints,sceneKeypoints;
    cv::Mat objectDescriptors,sceneDescriptors;

    detector->detect(objectImg, objectKeypoints);
    detector->detect(sceneImg, sceneKeypoints);

    extractor->compute(objectImg, objectKeypoints, objectDescriptors);
    extractor->compute(sceneImg, sceneKeypoints, sceneDescriptors);

    //drawKeypoints(objectImg, objectKeypoints, outImg1, Scalar(255,255,255)), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    //drawKeypoints(sceneImg, sceneKeypoints, outImg2, Scalar(255,255,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    //namedWindow("SURF detector img1");
    imshow("SURF detector img1", outImg1);

    //namedWindow("SURF detector img2");
    imshow("SURF detector img2", outImg2);

    cv::waitKey(0);
/*
    cv::Mat objectData;
            cv::Mat sceneData;
            if(objectDescriptors.type()!=CV_32F) {
                objectDescriptors.convertTo(objectData, CV_32F); // make sure it's CV_32F
            }
            else {
                objectData = objectDescriptors;
            }
            if(sceneDescriptors.type()!=CV_32F) {
                sceneDescriptors.convertTo(sceneData, CV_32F); // make sure it's CV_32F
            }
            else {
                sceneData = sceneDescriptors;
            }

            // Create Flann index
            cv::flann::Index treeFlannIndex(sceneData, cv::flann::KDTreeIndexParams());


 int k=2; // find the 2 nearest neighbors
cv::Mat results(objectData.rows, k, CV_32SC1); // Results index
cv::Mat dists(objectData.rows, k, CV_32FC1); // Distance results are CV_32FC1
treeFlannIndex.knnSearch(objectData, results, dists, k, cv::flann::SearchParams() ); // maximum number of leafs checked
*/


}


/*
void matchShapes(Mat image,Mat sample)
{


}



void HOG3(IplImage *Im,vector<float>& descriptors)
{


    int nwin_x=1; //number of windows in x directions
    int nwin_y=1; //number of windows in y directions
    int B=12; //number of orientations




    Mat im1=Mat(Im);

    int L=im1.rows; //image height
    int C=im1.cols; //image widht

    descriptors.resize(nwin_x*nwin_y*B); //allocating memory for descriptors

    im1.convertTo(im1,CV_32FC1,1,0);




    Mat hx; //x derivative
    cv::Sobel(im1,hx,CV_32FC1,1,0,3,1);
    Mat hy; //y derivative
    cv::Sobel(im1,hy,CV_32FC1,1,0,3,1);


    int step_x=floor(C/(nwin_x+1));
    int step_y=floor(L/(nwin_y+1));
    int cont=0;

    Mat magnitude,orientation; //magnitude and orientation image




    cv::cartToPolar(hx,hy,magnitute,orientation,false);
    //cartesian to polar transformations

    Mat magnitude1,orientation1;
    magnitude.copyTomagnitude1;
    magnitude1.setTo(cv::Scalar::all(0));
    cvtColor(magnitude1,magnitude1,CV_BGR2GRAY);
    magnitude1.setTo(cv::Scalar::all(0));
    magnitude1.copyTo(orientation1);

    int step = I1->widthStep/sizeof(uchar);


    for(int i=0;i<I.rows;i++)
    {
        float *ptr=magnitude.ptr<float>(i);
        float *optr=orientation.ptr<float>(i);
        float *ptr1=magniture1.ptr<float>(i);
        float *ptr2=orientation1.ptr<float>(i);
        for(int j=0;j<I.cols;j++)
        {
            float max = ptr[j];

            if (ptr[j+1] > max) {
                ptr1[j]=ptr1[j+1];
                ptr2[j]=optr[j+1];
            }
            //consider only H and S channels.
            if (ptr[j+2] > max) {
                ptr1[j]=ptr1[j+2];
                ptr2[j]=optr[j+2];
            }

        }


    }

    int x, y;

    int m=0,n=0;


    //for each subwindow computing the histogram

    for(int n=0;n<nwin_x;n++)
    {
        for(int m=0;m<nwin_y;m++)
        {

            cont=cont+1;

            Mat mroi=magnitude1(Rect((m*step_x),(n*step_y),2*step_x,2*step_y));
            Mat orio=orientation1(Rect((m*step_x),(n*step_y),2*step_x,2*step_y));
            int K=0;

            vector<float> descriptors1;
            descriptors1.resize(B);


            MatConstIterator_<float> it = mroi.begin<float>(), it_end = mroi.end<float>();
            MatConstIterator_<float> it1 = orio.begin<float>(), it_end = orio.end<float>();
            for(; it != it_end; ++it,++it1)
            {
                for(int k=0;k<K;k++)
                {
                if ( ((2*k*pi)/B) <= *it1 < (2*(k+1)*pi/B)) {
                     descriptors1[k]=descriptors1[bin]+(*it);
                     break;
                  }
                }
            }

            Mat matAngles(descriptors1.size(), descriptors1.at(0).size(), CV_32FC1, descriptors1.data());
            cv::normalize(matAngles,matAngles,1,0,cv::NORM_L2);

            for(int y1=0;y1<descriptors1.size();y1++)
            {
                descriptors[(cont-1)*B+y1]=matAngles.ptr<float>(y1);
            }

        }
    }

    magnitude1.release();
    orientation1.release();
    magnitude.release();
    orientation.release();

}
*/

#endif // HOG_HPP
