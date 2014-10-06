#include "gaussian.hpp"

Gaussian::Gaussian()
{
    t.t=gtype::TYPE_FULL;
}

void Gaussian::validate(float &res)
{
    //if(res<std::numeric_limits<float>::epsilon())
      //  res=0;

    if(res>1)
        res=1;
    if(isnan(res))
        res=0;
    if(isinf((res)))
        res=1;
    if(isinf(-res))
        res=0;
    if(isnan(-res))
        res=1;


}


float Gaussian::Prob(Mat &x)
{
    MatrixXd tmp;
    tmp=setData(x);

    float res=0;
    if(t.t==gtype::TYPE_FULL)
    {

    MatrixXd tmp1=(tmp-_mu).transpose();
    //cerr << "AAAAAAAa" << endl;
    //cerr << tmp1.transpose() << endl;

    MatrixXd tmp2=_LI.matrixL();
    tmp2=tmp2.inverse()*tmp1;
    tmp2=tmp2.transpose()*tmp2;
    //MatrixXd tmp2=tmp1.transpose()*tmp1;
    //cerr << x << endl;
//    cerr << tmp2.transpose() << endl;
    res=tmp2(0,0);
    //cerr << "AAAAAAAAAAaa" << -0.5*res << endl;
    //cerr << tmp1 << endl;
    //cerr << tmp << endl;
    //cerr << _mu << endl;
    //cerr << _invsigma << endl;
    //MatrixXf tmp2=tmp1.transpose()*_invsigma*tmp1;
    //res=tmp2(0,0);


    }
    else if(t.t==gtype::TYPE_SPHERICAL)
    {


    }
    else if(t.t==gtype::TYPE_DIAGONAL)
    {

    }
    //cerr << _mu << endl;
    //cerr << _Sigma <<endl;
    //cerr << -0.5*res <<":" << std::log(_scale) << endl;
    //cerr << _mu << endl;

    res=(_scale)+(-0.5*res);
    //cerr << res << "[]" ;
    //res=std::exp(res);

    //cerr << "LL-" << res << ":" << std::exp(res) << endl;
    //res=std::exp(res);

    //validate(res);
    return res;
}



float Gaussian::logProb(Mat &x)
{
    /*MatrixXd tmp;
    tmp=setData(x);

    double res=0;
    if(t.t==gtype::TYPE_FULL)
    {

    MatrixXd tmp1=(tmp-_mu).transpose();
    L
    //tmp1=_LI*tmp1;
    MatrixXd tmp2=tmp1.transpose()*tmp1;
    res=tmp2(0,0);
    }
    else if(t.t==gtype::TYPE_SPHERICAL)
    {


    }
    else if(t.t==gtype::TYPE_DIAGONAL)
    {

    }

    cerr << -0.5*res << ":" << endl;
    res=std::log(_scale)+(-0.5*res);
    res=std::exp(res);
    //cerr << res << "[]" ;
    //validate(res);
    return res;
    */
    return 0;
}


void Gaussian::setMean(vector<float> &v)
{

    //Map<Eigen::Matrix<double,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data(),1,v.size()) ;
    //_mu=mappedMat;
}

void Gaussian::setMean(Mat &v)
{
   // Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,1,v.cols) ;
    MatrixXd mappedMat=EigenUtils::setData(v);
    _mu=mappedMat;
}

MatrixXd Gaussian::setData(cv::Mat &v)
{
    //Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,1,v.cols) ;
    MatrixXd mappedMat=EigenUtils::setData(v);
    MatrixXd  ref=mappedMat;
    return ref;

}


void Gaussian::setSigma(cv::Mat &v)
{
    //Map<Eigen::Matrix<float,Dynamic,Dynamic,Eigen::RowMajor> > mappedMat((float *)v.data,v.rows,v.cols) ;

    MatrixXd mappedMat=EigenUtils::setData(v);
    //cerr << v << endl;
    //return;
    _Sigma=mappedMat;
    _dim=v.rows;
    //_det=_Sigma.determinant();
    ///if(_det<std::numeric_limits<double>::epsilon())
    //    _det=std::numeric_limits<double>::epsilon();
    //_Sigma=_Sigma+1e-7;
    LLT<MatrixXd> lltOfA;
    lltOfA.compute(_Sigma);
    MatrixXd L = lltOfA.matrixL();
    //cerr << L.diagonal() << endl;
    _det=2*L.diagonal().array().log().sum();

    _scale=-0.5*(_dim*std::log(2*PI)+_det);
    //_scale=1/((std::pow(2*PI,_dim*0.5)*std::pow(_det,0.5)));
    //cerr << _Sigma.diagonal() << endl;
    //cerr << _scale << endl;
    //cerr << _det << ":" << _scale << ":" << std::numeric_limits<float>::epsilon() << endl;
    //_invsigma=_Sigma.inverse();
    //cerr << "SCALE " << _scale << endl;



    _LI=lltOfA;
}
