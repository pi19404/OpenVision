#include "randomferms.h"

RandomFerns::RandomFerns()
{
    _numFeatures=13;
    _numFerns=10;
    init();
    initProb();
    //readFromFile("/home/pi19404/config_oc.txt");

}


void RandomFerns::initProb()
{
   _numIndices = pow(2.0f, _numFeatures);
   posteriors.resize(0);
   positive.resize(0);
   negatives.resize(0);

for(int i=0;i<_numFerns;i++)
{
    for(int j=0;j<_numIndices;j++)
    {
        posteriors.push_back(0);
        positive.push_back(0);
        negatives.push_back(0);
    }
}
}

float RandomFerns::calcConfidence(vector<int> features)
{

    float conf=0.0;
    for(int i=0;i<features.size();i++)
    {
        conf=conf+posteriors[i*_numIndices+features[i]];
    }
    return conf/(features.size());
}



void RandomFerns::updatePosterior(vector<int> features,bool class1,int ammount)
{
    for(int i=0;i<features.size();i++)
    {
        int arrayIndex=(i*_numIndices)+features[i];
        class1?positive[arrayIndex]+=ammount:negatives[arrayIndex]+=ammount;
        posteriors[arrayIndex]=(((float)positive[arrayIndex])+0.1)/((float)positive[arrayIndex]+(float)negatives[arrayIndex]+0.1);
    }
 //   writeToFile("/home/pi19404/config_oc.txt");
}


//compute the location points for the binary tests
void RandomFerns::init()
{

    points.resize(_numFerns);
    int toggle=0;
    for(int i=0;i<_numFerns;i++)
    {
        vector<Point2f> px=points[i];
        for(int j=0;j<2*_numFeatures;j++)
        {
            Point2f p;
            p.x=((float)std::rand())/(float)RAND_MAX;
            p.y=((float)std::rand())/(float)RAND_MAX;
            p.x=p.x/2;
            p.y=p.y/2;

            toggle=((float)std::rand())/(float)RAND_MAX;

            if(toggle<0.25)
            {
              p.x=0.5-p.x;
              p.y=0.5-p.y;
            }
            else if(toggle<0.5)
            {
                p.x=0.5+p.x;
                p.y=0.5+p.y;
            }
            else if(toggle < 0.75)
            {
                p.x=0.5-p.x;
                p.y=0.5+p.y;

            }
            else if(toggle <1)
            {
                p.x=0.5+p.x;
                p.y=0.5-p.y;
            }
            px.push_back(p);
            toggle=!toggle;


        }
        points[i]=px;
    }
}





vector<int> RandomFerns::computeFeatures(const Rect r,const Mat image,Mat avg=Mat())
{


    vector<int> features;
    features.resize(0);

    Mat roi=image(r);
    //if(avg.empty()==false)
    Mat roi2;
    if(!avg.empty())
        roi2=avg(r);
    for(int i=0;i<points.size();i++)
    {
        int index=0;
        vector<Point2f> pp=points[i];

        for(int j=0;j<pp.size();j=j+2)
        {

            index <<=1;
            Point2f p=pp[j]*r.width;
            Point2f p1=pp[j+1]*r.height;
            uchar val1=roi.at<uchar>(p.x,p.y);
            uchar val2=roi.at<uchar>(p1.x,p1.y);

            uchar val3;
            if(!avg.empty())
                val3=roi2.at<uchar>(p.x,p.y);
            else
                val3=val2;
            /*if((int)val1 >(int)val2 && avg.empty())
            {
                index|=1;
            }
            else*/ if((int)val1 >(int)val3)
            {
                index|=1;
            }

        }

        features.push_back(index);
    }
    return features;
}



vector<Rect> RandomFerns::compute(vector<Rect> r,const Mat image,const Mat avg=Mat())
{

    //cerr << "---" << r.size() << endl;
    vector<vector<int> >features;
    vector<Rect> rx;
    rx.resize(r.size());
    features.resize(r.size());
    vector<float> variance;
    variance.resize(r.size());


    #pragma omp parallel for
    for(int i=0;i<r.size();i++)
    {

        //Mat roi=image(r[i]);
        //computing fern features,a binary string  is retuned for each fern
        features[i]=computeFeatures(r[i],image,avg);
        float conf=calcConfidence(features[i]);

        //storing the features
        //cerr << conf <<":" << i <<"---";


        //if(conf > 0.5)
        ///cerr << variance[i] <<":" << vfilter.minVar << endl;
        if(conf >0.5)
        {
            rx[i]=r[i];
        }
        else
        {
            rx[i]=Rect(0,0,0,0);
        }
        //features.push_back(ret);
    }

    //learn(rx);
    //cerr << endl;

    return rx;

}


void RandomFerns::learn(Rect r,Mat image,bool class1,bool force=0,Mat avg=Mat())
{

    vector<int> features=computeFeatures(r,image,avg);

    /*for(int i=0;i<features.size();i++)
    {
        cerr << (int)features[i] <<":";
    }*/
    //cerr << endl;
    float conf=calcConfidence(features);
    //cerr << "Prev " << conf << endl;
    //if positive patch has low confidence
    //or if negative patch has high confidence update
    //if positive path has high confidence do not update
    if((class1 && conf <=0.5) || ((!class1) && conf >=0.5) ||force)
    {
        updatePosterior(features,class1,1);
    }
    conf=calcConfidence(features);
    //cerr << "Post " << conf << endl;

}


bool RandomFerns::classify(Rect r,Mat image,Mat avg=Mat())
{


    vector<int> features=computeFeatures(r,image,avg);
    if(calcConfidence(features)<0.5)
        return false;
    else
        return true;
}



void RandomFerns::writeToFile(const char *path)
{

    FILE *file = fopen(path, "w");
    fprintf(file, "#Tld ModelExport\n");
    fprintf(file, "%d\n", _numFerns);
    fprintf(file, "%d\n",_numFeatures);
    for(int i = 0; i < points.size(); i++)
    {
        vector<Point2f> pp=points[i];
        for(int j=0;j<pp.size();j++)
        {
        fprintf(file, "%f %f \n",pp[i].x,pp[i].y);
        }
    }

    for(int i=0;i<_numFerns;i++)
    {
        for(int index = 0; index < pow(2.0f,_numIndices); index++)
        {
            fprintf(file,"%f %f %f \n",positive[i*_numFerns+index],negatives[i*_numFerns+index],posteriors[i*_numFerns+index]);
        }
    }
    fclose(file);

}


void RandomFerns::readFromFile(const char *path)
{
    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        printf("Error: Model not found: %s\n", path);
        _numFeatures=13;
        _numFerns=10;
        init();
        initProb();
        return;
    }

    int MAX_LEN = 255;
    char str_buf[255];
    fgets(str_buf, MAX_LEN, file); /*Skip line*/
    fscanf(file, "%d \n", &_numFerns);
    fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/
    fscanf(file, "%d \n", &_numFeatures);
    fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/

    points.resize(0);
    float f1,f2,f3;
    points.resize(_numFerns);
    for(int i=0;i<_numFerns;i++)
    {
        vector<Point2f> p;
        for(int j=0;j<2*_numFeatures;j++)
        {
           fscanf(file, "%f %f", &f1,&f2);
           p.push_back(Point2f(f1,f2));
          fgets(str_buf, MAX_LEN, file); /*Skip line*/
        }
        points[i]=p;
    }

    positive.resize(pow(2.0f,_numIndices)*_numFerns);
    negatives.resize(pow(2.0f,_numIndices)*_numFerns);
    posteriors.resize(pow(2.0f,_numIndices)*_numFerns);
    for(int i=0;i<_numFerns;i++)
    {
        for(int index = 0; index < pow(2.0f,_numIndices); index++)
        {
            fscanf(file,"%f %f %f \n",&f1,&f2,&f3);
            positive[i*_numFerns+index]=f1;
            negatives[i*_numFerns+index]=f2;
            posteriors[i*_numFerns+index]=f3;
            fgets(str_buf, MAX_LEN, file); /*Skip line*/
        }
    }
    fclose(file);

}

