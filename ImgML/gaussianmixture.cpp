#include "ImgML/gaussianmixture.hpp"



GaussianMixture::GaussianMixture()
{
    index=0;
}



void GaussianMixture::setnmix(int n)
{
    _nmix=n;
    _components.resize(_nmix);
    _weights.resize(_nmix);
}

void GaussianMixture::setGaussian(Gaussian a,float prior)
{
    _components[index]=a;
    _weights[index]=prior;
    index=index+1;
}


void GaussianMixture::clearModel()
{
    index=0;
    _nmix=0;
    _components.clear();
    _weights.clear();
}


/**
 * @brief ProbMix:method returns the probability that
 *        vector X belongs to each of mixture gaussian
 * @param X : input vector X
 * @return  :output vector of probabilities
 */
vector<float> GaussianMixture::ProbMix(Mat X)
{
    vector<float> res;
    res.resize(_nmix);
    for(int i=0;i<_nmix;i++)
    {
    //    cerr << _weights[i] << endl;
        res[i]=(_components[i].Prob(X));
    }
    return res;

}


/**
 * @brief Prob : method computes the probability that vector X
 *               is drawn from the gaussian mixture model
 * @param X     : input vector X
 * @return
 */
float GaussianMixture::Prob(Mat X)
{
    float res=0;
    for(int i=0;i<_nmix;i++)
    {
    //    cerr << _weights[i] << endl;
        res=res+_weights[i]*(_components[i].Prob(X));
    }
    return res;
}



