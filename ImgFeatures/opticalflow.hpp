#ifndef OPTICALFLOW_HPP
#define OPTICALFLOW_HPP
#include "Common/OpenCVCommon.hpp"
#include "ImgProc/convolution.hpp"
#include "ImgProc/channelfilter.hpp"
typedef Vec<float,6> Vec6f;
typedef Vec<float,5> Vec5f;
class DenseFlow
{
public:
    vector<Mat> _images;
    int N;
    int _index;
    bool init;
    Mat x,y,xx,yy,xy,c,kernel;
    vector<Mat> res;
    Size s;
    Size win;
    Mat G;
    Mat flow;

    SeperableSConvolution filter;
    channelFilter cfilter;


    Ptr<FilterEngine> fx,fy,fxy,fxx,fyy,fc,cf;

    DenseFlow()
    {
        N=2;
        _images.resize(N);
        init=false;
        res.resize(N);

        _index=0;
        win=Size(3,3);

    }

    int getIndex(int index)
    {
        if(index<0)
            index=index+N;
        if(index>N)
            index=index-N;
        return index;
    }

    void meshGrid(Mat &x,Mat &y,int size)
    {
        x.create(size,size,CV_32FC1);
        y.create(size,size,CV_32FC1);
        for (int i = 0;i<size; i++) {
            x.at<float>(0,i) = i-floor(size/2);
            y.at<float>(i,0) = i-floor(size/2);
        }


        x = repeat(x.row(0),size,1);
        y = repeat(y.col(0),1,size);


    }

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

    void gaussian2Dkernel(Mat &kernel,int size)
    {
        Mat x,y;
        meshGrid(x,y,size);
        Mat k=cv::getGaussianKernel(size,1,CV_32F);
        k=k.t();
        cv::mulTransposed(k,kernel,true);



    }


    void computeFilters()
    {
        meshGrid(x,y,7);
        cv::pow(x,2,xx);
        cv::pow(y,2,yy);
        cv::multiply(x,y,xy);
        xy.copyTo(c);
        c.setTo(cv::Scalar::all(1));
        gaussian2Dkernel(kernel,7);


        polyBasis(G,x,y,xx,yy,xy,kernel);

        cv::multiply(x,kernel,x);
        cv::multiply(y,kernel,y);
        cv::multiply(xx,kernel,xx);
        cv::multiply(yy,kernel,yy);
        cv::multiply(xy,kernel,xy);
        cv::multiply(c,kernel,c);



        G.copyTo(cfilter.kernel);
        fy=cv::createLinearFilter(CV_32F,CV_32F,x,Point(-1,-1),0,BORDER_REPLICATE);
        fx=cv::createLinearFilter(CV_32F,CV_32F,y,Point(-1,-1),0,BORDER_REPLICATE);
        fyy=cv::createLinearFilter(CV_32F,CV_32F,xx,Point(-1,-1),0,BORDER_REPLICATE);
        fxx=cv::createLinearFilter(CV_32F,CV_32F,yy,Point(-1,-1),0,BORDER_REPLICATE);
        fxy=cv::createLinearFilter(CV_32F,CV_32F,xy,Point(-1,-1),0,BORDER_REPLICATE);
        fc=cv::createLinearFilter(CV_32F,CV_32F,c,Point(-1,-1),0,BORDER_REPLICATE);

        computeSeparableFilters(7);
    }

    void FarnebackPolyExp( const Mat& src, Mat& dst, int n, double sigma )
    {
        int k, x, y;

        assert( src.type() == CV_32FC1 );
        int width = src.cols;
        int height = src.rows;
        AutoBuffer<float> kbuf(n*6 + 3), _row((width + n*2)*3);
        float* g = kbuf + n;
        float* xg = g + n*2 + 1;
        float* xxg = xg + n*2 + 1;
        float *row = (float*)_row + n*3;

        if( sigma < FLT_EPSILON )
            sigma = n*0.3;

        double s = 0.;
        for( x = -n; x <= n; x++ )
        {
            g[x] = (float)std::exp(-x*x/(2*sigma*sigma));
            s += g[x];
        }

        s = 1./s;
        for( x = -n; x <= n; x++ )
        {
            g[x] = (float)(g[x]*s);
            xg[x] = (float)(x*g[x]);
            xxg[x] = (float)(x*x*g[x]);
        }

        Mat_<double> G = Mat_<double>::zeros(6, 6);

        for( y = -n; y <= n; y++ )
            for( x = -n; x <= n; x++ )
            {
                G(0,0) += g[y]*g[x];
                G(1,1) += g[y]*g[x]*x*x;
                G(3,3) += g[y]*g[x]*x*x*x*x;
                G(5,5) += g[y]*g[x]*x*x*y*y;
            }

        //G[0][0] = 1.;
        G(2,2) = G(0,3) = G(0,4) = G(3,0) = G(4,0) = G(1,1);
        G(4,4) = G(3,3);
        G(3,4) = G(4,3) = G(5,5);

        // invG:
        // [ x        e  e    ]
        // [    y             ]
        // [       y          ]
        // [ e        z       ]
        // [ e           z    ]
        // [                u ]
        Mat_<double> invG = G.inv(DECOMP_CHOLESKY);
        double ig11 = invG(1,1), ig03 = invG(0,3), ig33 = invG(3,3), ig55 = invG(5,5);

        dst.create( height, width, CV_32FC(5));

        for( y = 0; y < height; y++ )
        {
            float g0 = g[0], g1, g2;
            float *srow0 = (float*)(src.data + src.step*y), *srow1 = 0;
            float *drow = (float*)(dst.data + dst.step*y);

            // vertical part of convolution
            for( x = 0; x < width; x++ )
            {
                row[x*3] = srow0[x]*g0;
                row[x*3+1] = row[x*3+2] = 0.f;
            }

            for( k = 1; k <= n; k++ )
            {
                g0 = g[k]; g1 = xg[k]; g2 = xxg[k];
                srow0 = (float*)(src.data + src.step*std::max(y-k,0));
                srow1 = (float*)(src.data + src.step*std::min(y+k,height-1));

                for( x = 0; x < width; x++ )
                {
                    float p = srow0[x] + srow1[x];
                    float t0 = row[x*3] + g0*p;
                    float t1 = row[x*3+1] + g1*(srow1[x] - srow0[x]);
                    float t2 = row[x*3+2] + g2*p;

                    row[x*3] = t0;
                    row[x*3+1] = t1;
                    row[x*3+2] = t2;
                }
            }



            // horizontal part of convolution for border pixels
            for( x = 0; x < n*3; x++ )
            {
                row[-1-x] = row[2-x];
                row[width*3+x] = row[width*3+x-3];
            }

            for( x = 0; x < width; x++ )
            {
                g0 = g[0];
                // r1 ~ 1, r2 ~ x, r3 ~ y, r4 ~ x^2, r5 ~ y^2, r6 ~ xy
                double b1 = row[x*3]*g0, b2 = 0, b3 = row[x*3+1]*g0,
                    b4 = 0, b5 = row[x*3+2]*g0, b6 = 0;

                for( k = 1; k <= n; k++ )
                {
                    double tg = row[(x+k)*3] + row[(x-k)*3];
                    g0 = g[k];
                    b1 += tg*g0;
                    b4 += tg*xxg[k];
                    b2 += (row[(x+k)*3] - row[(x-k)*3])*xg[k];
                    b3 += (row[(x+k)*3+1] + row[(x-k)*3+1])*g0;
                    b6 += (row[(x+k)*3+1] - row[(x-k)*3+1])*xg[k];
                    b5 += (row[(x+k)*3+2] + row[(x-k)*3+2])*g0;
                }

                // do not store r1
                drow[x*5+1] = (float)(b2*ig11);
                drow[x*5] = (float)(b3*ig11);
                drow[x*5+3] = (float)(b1*ig03 + b4*ig33);
                drow[x*5+2] = (float)(b1*ig03 + b5*ig33);
                drow[x*5+4] = (float)(b6*ig55);
            }
        }

        row -= n*3;
    }

    //function to get pixel val
    Mat getSubPixel(const float *ptr,Point2f p,int ch,Size s,int step1)
    {
        //top left corner
        Point pl=Point(cvFloor(p.x),cvFloor(p.y));
        Mat res=Mat(1,1,CV_32FC(ch));
        //res.setTo(cv::Scalar::all(0));
        if(pl.x<1 ||pl.y<1 || pl.x >=s.width-1 || pl.y >=s.height-1)
        {
            res.at<float>(0)=0;
            res.at<float>(1)=0;
            res.at<float>(2)=0;
            res.at<float>(3)=0;
            res.at<float>(4)=0;
            return res;
        }
        float dx=pl.x-p.x;
        float dy=pl.y-p.y;
        dx=-dx;
        dy=-dy;
        float a00 = (1.f-dx)*(1.f-dy), a01 = dx*(1.f-dy),
              a10 = (1.f-dx)*dy, a11 = dx*dy;


        int lt=0;
        int rt=lt+ch;
        int lb=step1+lt;
        int rb=step1+rt;


        //performing bilinear interpolation on all the channels
        for(int i=0;i<ch;i++)
        {
         res.at<float>(i) = a00*ptr[lt] + a01*ptr[rt] + a10*ptr[lb] + a11*ptr[rb];
         lt=lt+1;
         rt=lt+ch;
         lb=step1+lt;
         rb=step1+rt;
        }
        return res;
    }

    void updatePoly(const float *ptr1,const float *ptr2,Point2f d,bool flag,float *M,int x,int y,Size s)
    {
        //x y x2 x2 xy
        //float *ptr1=r1.ptr<float>(0);
        //float *ptr2=r2.ptr<float>(0);
        const int BORDER = 5;
        static const float border[BORDER] = {0.14f, 0.14f, 0.4472f, 0.4472f, 0.4472f};

        float r[5];
        float r2,r3,r4,r5,r6;
        if(flag==true)
        {
        r4=(ptr1[2]+ptr2[2])*0.5;
        r5=(ptr1[3]+ptr2[3])*0.5;
        r6=(ptr1[4]+ptr2[4])*0.25;

        }
        else
        {
            r[0]=0.f;
            r[1]=0.f;
            r[2]=ptr1[2];
            r[3]=ptr1[3];
            r[4]=ptr1[4]*0.5;
            r2=r[0];
            r3=r[1];
            r4=r[2];
            r5=r[3];
            r6=r[4];


        }
        r2=(ptr1[0]-ptr2[0])*0.5;
        r3=(ptr1[1]-ptr2[1])*0.5;

        r2 += r4*d.x + r6*d.y;
        r3 += r6*d.x + r5*d.y;


        if( (unsigned)(x - BORDER) >= (unsigned)(s.width - BORDER*2) ||
            (unsigned)(y - BORDER) >= (unsigned)(s.height - BORDER*2))
        {
            float scale = (x < BORDER ? border[x] : 1.f)*
                (x >= s.width - BORDER ? border[s.width - x - 1] : 1.f)*
                (y < BORDER ? border[y] : 1.f)*
                (y >= s.height - BORDER ? border[s.height - y - 1] : 1.f);

            r2 *= scale; r3 *= scale; r4 *= scale;
            r5 *= scale; r6 *= scale;
        }


       //final displacement
        M[0]   = r4*r4 + r6*r6; // G(1,1)
        M[1] = (r4 + r5)*r6;  // G(1,2)=G(2,1)
        M[2] = r5*r5 + r6*r6; // G(2,2)
        M[3] = r4*r2 + r6*r3; // h(1)
        M[4] = r6*r2 + r5*r3; // h(2)

    }



    void FarnebackUpdateMatrices( const Mat& _R0, const Mat& _R1, const Mat& _flow, Mat& matM, int _y0, int _y1 )
    {

        int i, j, width = _flow.cols, height = _flow.rows;
        const float* R1 = (float*)_R1.data;
        int ch=_R0.channels();
        const int BORDER = 5;
        static const float border[BORDER] = {0.14f, 0.14f, 0.4472f, 0.4472f, 0.4472f};

        size_t step1 = _R1.step/sizeof(R1[0]);

        Size s=Size(_R0.cols,_R0.rows);
        matM.create(height, width, CV_32FC(5));


        //rows to be processed
        for( i = _y0; i < _y1; i++ )
        {
            const float* flow = (float*)(_flow.data + i*_flow.step); //optical flow pervious
            const float* R0 = (float*)(_R0.data + i*_R0.step);  //polynomial coeff of image 1
            float* M = (float*)(matM.data + i*matM.step);

            //process all the columns
            for( j = 0; j < width; j++ )
            {

                float dx = flow[j*2], dy = flow[j*2+1];
                //getting the displacement,dislaced point
                float fx = j + dx, fy = i + dy;
                int x1 = cvFloor(fx), y1 = cvFloor(fy);
                const float* ptr = R1 + y1*step1 + x1*ch;


                Mat res1=getSubPixel(ptr,Point2f(fx,fy),ch,s,step1);

                const float *t=res1.ptr<float>(0);
                int x=j;
                int y=i;

                if(x1<1 || y1<1 || x1>=s.width-1 || y1 >=s.height-1)
                {

                       updatePoly(R0+(j*ch),t,Point2f(dx,dy),false,M+(j*ch),j,i,s);
                }
                else
                {
                        updatePoly(R0+(j*ch),t,Point2f(dx,dy),true,M+(j*ch),j,i,s);
                }



            }
        }
    }



    void FarnebackUpdateFlow_Blur( const Mat& _R0, const Mat& _R1,
                              Mat& _flow, Mat& matM, int block_size,
                              bool update_matrices )
    {
        int x, y, width = _flow.cols, height = _flow.rows;
        int m = block_size/2;
        int y0 = 0, y1;
        int min_update_stripe = std::max((1 << 10)/width, block_size);
        double scale = 1./(block_size*block_size);

        AutoBuffer<double> _vsum((width+m*2+2)*5);
        double* vsum = _vsum + (m+1)*5;

        // init vsum
        const float* srow0 = (const float*)matM.data;
        for( x = 0; x < width*5; x++ )
            vsum[x] = srow0[x]*(m+2);

        for( y = 1; y < m; y++ )
        {
            srow0 = (float*)(matM.data + matM.step*std::min(y,height-1));
            for( x = 0; x < width*5; x++ )
                vsum[x] += srow0[x];
        }

        // compute blur(G)*flow=blur(h)
        for( y = 0; y < height; y++ )
        {
            double g11, g12, g22, h1, h2;
            float* flow = (float*)(_flow.data + _flow.step*y);

            srow0 = (const float*)(matM.data + matM.step*std::max(y-m-1,0));
            const float* srow1 = (const float*)(matM.data + matM.step*std::min(y+m,height-1));

            // vertical blur anti symmetric filter ?
            for( x = 0; x < width*5; x++ )
                vsum[x] += srow1[x] - srow0[x];

            // update borders
            for( x = 0; x < (m+1)*5; x++ )
            {
                vsum[-1-x] = vsum[4-x];
                vsum[width*5+x] = vsum[width*5+x-5];
            }

            // init g** and h*
            g11 = vsum[0]*(m+2);
            g12 = vsum[1]*(m+2);
            g22 = vsum[2]*(m+2);
            h1 = vsum[3]*(m+2);
            h2 = vsum[4]*(m+2);

            //average over neighborhood
            for( x = 1; x < m; x++ )
            {
                g11 += vsum[x*5];
                g12 += vsum[x*5+1];
                g22 += vsum[x*5+2];
                h1 += vsum[x*5+3];
                h2 += vsum[x*5+4];
            }

            // horizontal blur
            for( x = 0; x < width; x++ )
            {
                g11 += vsum[(x+m)*5] - vsum[(x-m)*5 - 5];
                g12 += vsum[(x+m)*5 + 1] - vsum[(x-m)*5 - 4];
                g22 += vsum[(x+m)*5 + 2] - vsum[(x-m)*5 - 3];
                h1 += vsum[(x+m)*5 + 3] - vsum[(x-m)*5 - 2];
                h2 += vsum[(x+m)*5 + 4] - vsum[(x-m)*5 - 1];

                double g11_ = g11*scale;
                double g12_ = g12*scale;
                double g22_ = g22*scale;
                double h1_ = h1*scale;
                double h2_ = h2*scale;

                double idet = 1./(g11_*g22_ - g12_*g12_+1e-3);

                flow[x*2] = (float)((g11_*h2_-g12_*h1_)*idet);
                flow[x*2+1] = (float)((g22_*h1_-g12_*h2_)*idet);
            }

            y1 = y == height - 1 ? height : y - block_size;
            if( update_matrices && (y1 == height || y1 >= y0 + min_update_stripe) )
            {
                FarnebackUpdateMatrices( _R0, _R1, _flow, matM, y0, y1 );
                y0 = y1;
            }
        }
    }



    vector<Mat> o1,o2;
    vector<Mat> channels,channels1;
    Mat i1,i2;
    Mat prevflow;
    Mat row;
    void compute(Mat image,int levels=1,float pyr_scale=0.9,int iterations=3)
    {
        int prev_index=getIndex(_index-1);
        int cur_index=_index;
        int k=0;
        float scale=1;
        int m=0;
        image.convertTo(_images[cur_index],CV_32F);


        if(init==true)
        {


        Size pyr_size;
        for(int l=levels;l>=0;l--)
        {
            int64 t1=getTickCount();
            for(m = 0, scale = 1; m < l; m++ )
            {
                scale *= pyr_scale;
                cerr << pyr_scale << ":" << scale << endl;
            }


            pyr_size=Size(cvRound(scale*s.width),cvRound(scale*s.height));

            double sigma = (1./scale-1)*0.5;
            int smooth_sz = cvRound(sigma*5)|1;
            smooth_sz = std::max(smooth_sz, 3);
            if(!prevflow.data)
            {
            flow.create(pyr_size.height,pyr_size.width, CV_32FC2 );
            flow.setTo(cv::Scalar::all(0));
            }
            else
            {
            cv::resize(prevflow,flow,pyr_size,0,0,INTER_LINEAR);
            flow=flow*1./pyr_scale;
            }

            _images[prev_index].copyTo(i1);
            _images[cur_index].copyTo(i2);

            GaussianBlur(i1,i1, Size(smooth_sz, smooth_sz), sigma, sigma);
            GaussianBlur(i2,i2,Size(smooth_sz, smooth_sz), sigma, sigma);

            cv::resize(i1,i1,pyr_size,0,0,INTER_LINEAR );
            cv::resize(i2,i2,pyr_size,0,0,INTER_LINEAR );


            int ind=0;
            filter.apply(i1,res[prev_index]);
            filter.apply(i2,res[cur_index]);

            Mat dst1,dst2;
            cfilter.apply(res[prev_index],dst1);
            cfilter.apply(res[cur_index],dst2);


            cv::split(dst1,channels);
            channels.erase(channels.begin());
            cv::merge(channels,dst1);

            cv::split(dst2,channels1);
            channels1.erase(channels1.begin());
            cv::merge(channels1,dst2);

            int64 t2=getTickCount();
\

            res[prev_index]=dst1;
            res[cur_index]=dst2;


            Mat M;
            int winsize=15;
            FarnebackUpdateMatrices(res[prev_index],res[cur_index],flow,M,0,flow.rows);
            for( int i = 0; i < iterations; i++ )
            {

                    FarnebackUpdateFlow_Blur(res[prev_index],res[cur_index], flow, M,winsize, i < iterations - 1 );

            }


            flow.copyTo(prevflow);
        }


        }
        else
        {
            computeFilters();
            s=Size(image.cols,image.rows);
            initFlow();
            o1.resize(5);
            o2.resize(5);
            row=Mat(6,1,CV_32F);

        }

        init=true;
        update_index();
    }


    void initFlow()
    {
        int k=0;
        int levels=1;
        float scale=1;
        int i=0;
        float pyr_scale=0.9;

        for( k = levels; k >= 0; k-- )
        {
            for( i = 0, scale = 1; i < k; i++ )
                scale *= pyr_scale;

            double sigma = (1./scale-1)*0.5;
            int smooth_sz = cvRound(sigma*5)|1;
            smooth_sz = std::max(smooth_sz, 3);

            int width = cvRound(s.width*scale);
            int height = cvRound(s.height*scale);
            cerr << width <<":" << height << endl;
            cerr <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" <<endl;
/*
            if( k > 0 )
                flow.create( height, width, CV_32FC2 );
            else
                flow = flow0;

            if( !prevFlow.data )
            {
                if( flags & OPTFLOW_USE_INITIAL_FLOW )
                {
                    resize( flow0, flow, Size(width, height), 0, 0, INTER_AREA );
                    flow *= scale;
                }
                else
                    flow = Mat::zeros( height, width, CV_32FC2 );
            }
            else
            {
                resize( prevFlow, flow, Size(width, height), 0, 0, INTER_LINEAR );
                flow *= 1./pyr_scale;
            }
        }
*/
    }
    }


    void update_index()
    {
        _index=_index+1;
        _index=_index%N;
    }


};



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

