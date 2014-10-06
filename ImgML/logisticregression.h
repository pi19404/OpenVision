#ifndef LOGISTICREGRESSION_H
#define LOGISTICREGRESSION_H
#include "Common/OpenCVCommon.hpp"

// sigmoid function

inline double sigmoid(double x) { return 1.0 / (1 + exp(-x)); }

//class storing feature vector

class FeatureVector
{
public:
    int dim;
    vector<float> data;
    FeatureVector()
    {

    }
    FeatureVector(int dim)
    {
        data.resize(dim);
    }
};
//class for training/testing data
class Data
{
    int N;
    int dim;
    vector<FeatureVector> data;
    Data()
    {

    }
    Data(int _dim,int _N)
    {
        dim=_dim;
        N=_N;
        data.resize(_N);
        for(int i=0;i<N;i++)
        {
            data[i]=FeatureVector(dim);
        }
    }
};

class logisticRegression
{
public:
    logisticRegression();
    int iterations;
    double alpha;
    vector<double> linearRegressionStocDescGrad(Data & t)
    {

    }

    void predict()
    {

    }

    void load()
    {

    }

};

#endif // LOGISTICREGRESSION_H
