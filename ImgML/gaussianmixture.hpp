#ifndef GAUSSIANMIXTURE_HPP
#define GAUSSIANMIXTURE_HPP
#include "ImgML/gaussian.hpp"
/**
 * @brief The GaussianMixture class : encapsulates the gaussian mixture model
 * which is defined by a set of multivariate gaussians and associated weights
 */
class GaussianMixture
{
    int _nmix;
    vector<Gaussian> _components;
    vector<float> _weights;
    int index;
public:
    /**
     * @brief GaussianMixture : constructor for class
     */
    GaussianMixture();


    /**
     * @brief setnmix : method sets the number of mixture components
     * @param n : is input to the method containing the number of mixture components
     */
    void setnmix(int n);

    /**
     * @brief setGaussian : method add the gaussian density to moxture and associatedd weights
     * The method is required to be called each time we want to add a component to the mixture
     * and maximum number of times it can be called is equal to the input passed in the setMix routine
     * @param a : input object which encapsulates the multivariate gaussian PDF
     * @param prior : input parameters which consists of assiciated weights
     */
    void setGaussian(Gaussian a,float prior);


    /**
     * @brief clearModel : method resets the parameter of the model
     * The setMix and setGaussian methods need to be called again
     */
    void clearModel();


    /**
     * @brief ProbMix:method returns the probability that
     *        vector X belongs to each of mixture gaussian
     * @param X : input vector X
     * @return  :output vector of probabilities
     */
    vector<float> ProbMix(Mat X);


    /**
     * @brief Prob : method computes the probability that vector X
     *               is drawn from the gaussian mixture model
     * @param X     : input vector X
     * @return
     */
    float Prob(Mat X);


};

#endif // GAUSSIANMIXTURE_HPP


