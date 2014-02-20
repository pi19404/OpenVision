#include "gaussian.hpp"

Gaussian::Gaussian()
{
    t.t=gtype::TYPE_SPHERICAL;
}

void Gaussian::validate(float &res)
{
    if(res<0)
        res=0;
    if(res>1)
        res=1;
    if(isnan(res))
        res=0;
}


float Gaussian::Prob(Mat &x)
{
    MatrixXf tmp;
    tmp=setData(x);

    float res=0;
    if(t.t==gtype::TYPE_FULL)
    {

    MatrixXf tmp1=(tmp-_mu).transpose();
    tmp1=_LI*tmp1;
    MatrixXf tmp2=tmp1.transpose()*tmp1;
    res=tmp2(0,0);
    }
    else if(t.t==gtype::TYPE_SPHERICAL)
    {


    }
    else if(t.t==gtype::TYPE_DIAGONAL)
    {

    }

    res=_scale*exp(-0.5*res);
    validate(res);
    return res;
}


void Gaussian::setMean(vector<float> &v)
{
    Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data(),1,v.size()) ;
    _mu=mappedMat;
}

void Gaussian::setMean(Mat &v)
{
    Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,1,v.cols) ;
    _mu=mappedMat;
}

MatrixXf Gaussian::setData(cv::Mat &v)
{
    Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,1,v.cols) ;
    MatrixXf  ref=mappedMat;
    return ref;

}


void Gaussian::setSigma(cv::Mat &v)
{
    Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,v.rows,v.cols) ;
    _Sigma=mappedMat;
    _dim=v.rows;
    _det=_Sigma.determinant();
    _scale=1.f/(pow(2*PI*_det,_dim*0.5));
    _invsigma=_Sigma.inverse();

    LLT<MatrixXf> lltOfA(_Sigma);
    MatrixXf L = lltOfA.matrixL();
    _LI=L.inverse();
}
