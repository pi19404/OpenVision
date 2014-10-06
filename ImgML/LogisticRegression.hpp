#ifndef LOGISTICREGRESSION_H
#define LOGISTICREGRESSION_H
#include "Common/OpenCVCommon.hpp"
#include <ImgConfig/ConfigLib/lib/libconfig.h++>
#include "ImgML/eigenutils.hpp"

using namespace libconfig;


class logisticRegression
{
public:
    logisticRegression();


    MatrixXf _params;

    int _ndim;
    int _nclasses;


    int classify(Mat x)
    {
        Mat res=predict(x);
        int min=0;
        cv::minMaxIdx(res,0,0,0,&min);
        return (int)min;
    }

    Mat predict(Mat x)
    {
        MatrixXf input=EigenUtils::setDataf(x);
        input=_params*input;
        input=input.array().exp();
        float sum=input.sum();
        input=input/sum;
        cv::eigen2cv(input,x);
        return x;
    }

    bool loadConf(Config &cfg)
    {

        const Setting& root = cfg.getRoot();
        Setting &logistic = root["logistic"];
        _nclasses=logistic["nclasses"];
        Setting &_models=logistic["models"];
        _ndim=(int)logistic["ndim"];

        Mat matrix1=Mat(_nclasses,_ndim,CV_32FC1);
        for(int k=0;k<_nclasses;k++)
        {
            Setting &mix=logistic[(const char *)_models[k].c_str()];

            for(int i=0;i<_ndim;i++)
            {
                matrix1.at<float>(k,i)=mix[i];
            }

        }
        _params=EigenUtils::setDataf(matrix1);
    }

};

#endif // LOGISTICREGRESSION_H
