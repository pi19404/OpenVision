#include "denseof.hpp"

DenseOF::DenseOF()
{
    N=2;
    _images.resize(N);
    init=false;
    res.resize(N);

    _index=0;
    win=Size(15,15);


}


int DenseOF::getIndex(int index)
{
    if(index<0)
        index=index+N;
    if(index>N)
        index=index-N;
    return index;
}


//function to get pixel val at subpixel location by performing bilinear interpolation
Mat DenseOF::getSubPixel(const float *ptr,Point2f p,int ch,Size s,int step1)
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


//this function computes the optical flow displacement vector
//ptr1 and ptr2 are pointers to the array containing the polynomial basis
//corresponding to prev and curr images.
//x,y is the point at which the flow is computed
//M store the result of the computed displacement

/**
 * @brief updatePoly : The function computes the optical flow displacement vector
 * @param ptr1 : pointer to array containing the polynomial basis components
 * @param ptr2 : pointer to array containing the polynomial basis components
 * @param d    : estimate of displacement vector at the current point
 * @param flag : the flag indicating if the point is border pixels or not
 * @param M    : pointer to output array returning the computed coefficients
 * @param x    : co-ordinate at the current point where the coefficients is evaluated
 * @param y    :
 * @param s    : windows size for averaging
 */
void DenseOF::updatePoly(const float *ptr1,const float *ptr2,Point2f d,bool flag,float *M,int x,int y,Size s)
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
        r6=ptr1[4]*0.5;


    }
    // -(b1-b2)
    r2=(ptr1[0]-ptr2[0])*0.5;
    r3=(ptr1[1]-ptr2[1])*0.5;

    r2 += r4*d.y + r6*d.x;
    r3 += r6*d.y + r5*d.x;

//      r2 += r4*d.x + r6*d.y;
//    r3 += r6*d.x + r5*d.y;


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


//this function computes the optical flow displacement at each point in the image
//R0 and R1 are mat containing the polynomial basis of prev and curr frame
//M contains the initial estimate of the computed flow
//flow is input inital flow provided as input

/**
 * @brief EstimateFlow : This function computes the displacement field
 * @param _R0
 * @param _R1
 * @param _flow
 * @param matM
 * @param _y0
 * @param _y1
 */
void DenseOF::EstimateFlow( const Mat& _R0, const Mat& _R1, const Mat& _flow, Mat& matM)
{

    int i, j, width = _flow.cols, height = _flow.rows;
    const float* R1 = (float*)_R1.data;
    int ch=_R0.channels();



    size_t step1 = _R1.step/sizeof(R1[0]);

    Size s=Size(_R0.cols,_R0.rows);
    matM.create(height, width, CV_32FC(5));


    //rows to be processed
    for( i = 0; i < height; i++ )
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



/**
  * @brief AverageFlow
  * @param _R0   : Polynomial basis of prev frame
  * @param _R1   : Polynomial basis coefficients of current frame
  * @param _flow :estimate of current displacement field
  * @param matM :containing coefficients of polynomial basis for computing displacemnt field
  */
 void
DenseOF::AverageFlow( const Mat& _R0, const Mat& _R1,Mat& _flow, Mat& matM,int winsize)
{
    int x, y, width = _flow.cols, height = _flow.rows;

    int m = winsize/2;
    double sigma = m*0.3, s = 0;

    cv::GaussianBlur(matM,matM,Size(15,15),sigma);

    // compute blur(G)*flow=blur(h)
    for( y = 0; y < height; y++ )
    {
        double g11, g12, g22, h1, h2;
        float* flow = (float*)(_flow.data + _flow.step*y);
        float *coeff=(float *)(matM.data+matM.step*y);
        // update borders
        for( x = 0; x < width; x++ )
        {
            g11 = coeff[x*5];
            g12 = coeff[x*5+1];
            g22 = coeff[x*5+2];
            h1 = coeff[x*5+3];
            h2 = coeff[x*5+4];

            double idet = 1./(g11*g22 - g12*g12 + 1e-3);

            flow[x*2] = (float)((g11*h2-g12*h1)*idet);
            flow[x*2+1] = (float)((g22*h1-g12*h2)*idet);
        }


    }

    EstimateFlow(_R0,_R1,flow,matM);


}





void DenseOF::compute(Mat image,int levels,float pyr_scale,int iterations)
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
    //multiscale iterative loop
    for(int l=levels;l>=0;l--)
    {

        for(m = 0, scale = 1; m < l; m++ )
        {
            scale *= pyr_scale;

        }

        //image size at lowest scale
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

        //smoothing the image
        GaussianBlur(i1,i1, Size(smooth_sz, smooth_sz), sigma, sigma);
        GaussianBlur(i2,i2,Size(smooth_sz, smooth_sz), sigma, sigma);

        //resizing the image to scale
        cv::resize(i1,i1,pyr_size,0,0,INTER_LINEAR );
        cv::resize(i2,i2,pyr_size,0,0,INTER_LINEAR );


        //applying the polynomial basis filter
        poly.filter.apply(i1,res[prev_index]);
        poly.filter.apply(i2,res[cur_index]);

        //applying channes filter to get least square estimate
        Mat dst1,dst2;
        poly.cfilter.apply(res[prev_index],dst1);
        poly.cfilter.apply(res[cur_index],dst2);


        cv::split(dst1,channels);
        channels.erase(channels.begin());
        cv::merge(channels,dst1);

        cv::split(dst2,channels1);
        channels1.erase(channels1.begin());
        cv::merge(channels1,dst2);


\

        res[prev_index]=dst1;
        res[cur_index]=dst2;


        Mat M;
        int winsize=15;
        //estimating the coefficients for computation of displacement field
        EstimateFlow(res[prev_index],res[cur_index],flow,M);

        for( int i = 0; i < iterations; i++ )
        {
            //averaging the coefficients and computing the displacement field
            AverageFlow(res[prev_index],res[cur_index], flow,M,winsize);
            //EstimateFlow(res[prev_index],res[cur_index],flow,M,0,flow.rows);
        }


        flow.copyTo(prevflow);
    }


    }
    else
    {
        poly.computeFilters(7);
        s=Size(image.cols,image.rows);
  //      initFlow();
        o1.resize(5);
        o2.resize(5);
        row=Mat(6,1,CV_32F);

    }

    init=true;
    update_index();
}


/*
void DenseOF::initFlow()
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
}
}
*/

void DenseOF::update_index()
{
    _index=_index+1;
    _index=_index%N;
}
