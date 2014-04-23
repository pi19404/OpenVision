#ifndef OPTICALFLOW_HPP
#define OPTICALFLOW_HPP





    double roundToDecimal(double d, short DecimalPlaces)
    {

         std::stringstream ss;

         ss << std::fixed << std::setprecision( DecimalPlaces ) << d;

         return atof(ss.str().c_str());
    }

    void approx(Mat &A)
    {

    for (int y = 0; y < A.rows; y++) {
        float *ptr=A.ptr<float>(y);
      for (int x = 0;x < A.cols*A.channels(); x=x++) {
          double val=(float)ptr[x];
          ptr[x]=roundToDecimal(val,4);
      }
    }


    }













class OpticalFlow
{
public:
    vector<Mat> _images;
    int N;
    int _index;
    bool init;

    vector<vector<Mat> > _pyramids;
    vector<vector<Mat> > dx,dy,tensor;
    vector<vector<Mat> > dt,dttensor;
    vector<vector<Point2f> > points;
    int levels;
    Size win;
    TermCriteria criteria;


    OpticalFlow()
    {
        N=2;
        _images.resize(2);
        _pyramids.resize(2);
        _index=0;
        init=false;
         criteria.maxCount = 30;
         criteria.epsilon = 0.01;
         criteria.type=TermCriteria::EPS||TermCriteria::COUNT;
         win=Size(15,15);
         levels=1;
         dx.resize(N);
         dy.resize(N);
         tensor.resize(N);
         points.resize(N);
    }



    void createGaussianPyramid(Mat image,vector<Mat> &pyramids,int &levels,Size win)
    {
        Mat tmp,prev,out,tmp1;
        Size s;

        //allocating number of pyramid levels
        pyramids.resize(levels);


        s=image.size();
        image.copyTo(prev);
        for(int i=0;i<levels;i++)
        {
            if(i!=0)
            {
            //perform image down sampling

            out=Mat(s.height+2*win.height,s.width+2*win.width,image.type());
            Rect r=Rect(win.width,win.height,s.width,s.height);
            tmp=out(r);
            prev.copyTo(tmp);
            pyrDown(prev,tmp1,s);
            prev=out;
            }
            prev.copyTo(pyramids[levels-1-i]);
            s = Size((s.width+1)/2, (s.height+1)/2);
            if(s.width<=win.width||s.height<=win.height)
            {
                levels=i+1;
                return ;
            }

        }

    }

    void computePyrDerivaite(vector<Mat> pyr,int levels)
    {

    }

    void computeDerivative(Mat image,Mat &dx,Mat &dy)
    {
        cv::Sobel(image,dx,CV_32F,1,0,3,1,0,BORDER_REPLICATE);
        cv::Sobel(image,dy,CV_32F,0,1,3,1,0,BORDER_REPLICATE);
    }

    void computeTensor(Mat &dx,Mat &dy,Mat &tensor,Size win)
    {
        tensor=Mat(dx.rows,dx.cols,CV_32FC3);
        MatConstIterator_<float> it = dx.begin<float>(), it_end = dx.end<float>();
        MatConstIterator_<float> it1 = dy.begin<float>();
        MatIterator_<Vec3f> it2 = tensor.begin<Vec3f>();
        for(; it != it_end; ++it,++it1,++it2)
        {
            Vec3f& val = *it2;
            val[0]=(*it)*(*it);
            val[1]=(*it1)*(*it1);
            val[2]=(*it)*(*it1);
        }
        cv::GaussianBlur(tensor,tensor,win,1,1);
    }

    void computeOfMatrix(Mat temporal,Mat dx,Mat dy,Mat &tensor)
    {
        tensor=Mat(dx.rows,dx.cols,CV_32FC2);
        MatConstIterator_<float> it = dx.begin<float>(), it_end = dx.end<float>();
        MatConstIterator_<float> it1 = dy.begin<float>();
        MatConstIterator_<float> it2 = temporal.begin<float>();
        MatIterator_<Vec2f> it3 = tensor.begin<Vec2f>();
        for(; it != it_end; ++it,++it1,++it2,++it3)
        {
            Vec2f& val = *it3;
            val[0]=(*it)*(*it2);
            val[1]=(*it1)*(*it2);
        }
        cv::GaussianBlur(tensor,tensor,win,1,1);
    }

    void computeTemporalDerivative(Mat t1,Mat t2,Mat &res)
    {
        res=Mat(t1.rows,t1.cols,CV_32FC1);
        cv::subtract(t1,t2,res);
    }

    int getIndex(int index)
    {
        if(index<0)
            index=index+N;
        if(index>N)
            index=index-N;
        return index;
    }

    void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,const Scalar& color)
    {
        for(int y = 0; y < cflowmap.rows; y += step)
            for(int x = 0; x < cflowmap.cols; x += step)
            {
                const Point2f& fxy = flow.at<Point2f>(y, x);
                Point2f ff;
                ff.y=fxy.y;
                ff.x=fxy.x;
                if((ff.y*ff.y+ff.x*ff.x)>0.1)
                {
                    ff.y=fxy.y*20;
                    ff.x=fxy.x*20;


                line(cflowmap, Point(x,y), Point(cvRound(x+ff.x), cvRound(y+ff.y)),color);
                }
                //circle(cflowmap, Point(x,y), 2, color, -1);
            }
    }

    void computeOpticalFlow(Mat stensor,Mat ttensor,Mat &flow,float eigthreshold,Size win)
    {

        flow=Mat(stensor.rows,stensor.cols,CV_32FC(2));
        flow.setTo(cv::Scalar::all(0));
        MatIterator_<Vec3f> it1 = stensor.begin<Vec3f>(),it_end = stensor.end<Vec3f>();;
        MatIterator_<Vec2f> it2 = ttensor.begin<Vec2f>();
        MatIterator_<Vec2f> it3 = flow.begin<Vec2f>();
        for(; it1 != it_end; ++it1,++it2,++it3)
        {

            Vec3f& val = *it1;
            Vec2f& val1 = *it2;
            Vec2f& val2 = *it3;


            float D=((val[0]*val[1])-(val[2]*val[2]));
            float minEig = (val[1] + val[0] - std::sqrt(((val[0]-val[1])*(val[0]-val[1])) +4.f*val[2]*val[2]))/(2);//*win.width*win.height);
            minEig=minEig;


            if(minEig<eigthreshold || D <FLT_EPSILON || (abs(val1[1])<FLT_EPSILON && abs(val1[2])<FLT_EPSILON))
            {
                continue;
            }

            D=1.0f/D;
            val2[0]=(-val[1]*val1[0]+val1[1]*val[2])*D;
            val2[1]=(-val[0]*val1[1]+val1[0]*val[2])*D;

        }
    }


    ///main function to compute optical flow
    void compute(Mat image)
    {
        int prev_index=getIndex(_index-1);
        int cur_index=_index;

//        cvtColor(image,image,CV_BGR2GRAY);
//        image.copyTo(_images[cur_index]);
        image.convertTo(_images[cur_index],CV_32FC1,1.0/255.0,0);


  //      createGaussianPyramid(_images[cur_index],_pyramids[cur_index],levels,win);
        Mat flow,cflow;
        if(init==true)
        {


            for(int i=0;i<levels;i++)
            {
                Mat dx1,dy1,dt1,dttensor1,tensor1;

                //computing the derivatives
                computeDerivative(_images[prev_index],dx1,dy1);
                //computing the orientation tensor
                computeTensor(dx1,dy1,tensor1,win);


                //computing the iterative optical flow
                computeTemporalDerivative(_images[prev_index],_images[cur_index],dt1);
                computeOfMatrix(dt1,dx1,dy1,dttensor1);
                computeOpticalFlow(tensor1,dttensor1,flow,0.01,win);


            }
            cflow=Mat(flow.rows,flow.cols,CV_32FC(3));
            image.copyTo(cflow);
            drawOptFlowMap(flow,cflow,5,Scalar(0,255,0));
            imshow("ABCD",cflow);
        }


        init=true;
        update_index();
    }

    void update_index()
    {
        _index=_index+1;
        _index=_index%N;
    }




};

#endif // OPTICALFLOW_HPP

