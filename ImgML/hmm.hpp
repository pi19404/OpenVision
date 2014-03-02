#ifndef HMM_H
#define HMM_H

#include "ImgML/eigenutils.hpp"
#include "Common/OpenCVCommon.hpp"
#include "ImgML/gaussianmixture.hpp"
/**
 * @brief The HMM class :
 * This class encapsulates the representation of a Discrete Hidden Markov model
 * A hidden markov model is composed of transition matrix,emission matrix
 * and initial probability matrix
 */
namespace ocv
{
class DHMM
{
public:
    MatrixXf _transition;
    MatrixXf _initial;
    MatrixXf _emission;
    int _nstates;
    int _nobs;
    int _seqlen;
    MatrixXf _alpha;
    MatrixXf _beta;
    MatrixXf _scale;

    /**
     * @brief setData : method to set the parameters of the model
     * @param transition
     * @param emission
     * @param initial
     * @param seqlen  : the maximum sequence length of sequence,for
     *                  allocating matrices
     */
    void setData(Mat transition,Mat emission,Mat initial);



    //computing p(xn.....xN,zn)
    void backwardMatrix(vector<int> &sequence);


    /**
     * @brief forwardMatrix : method computes probability //compute p(x1 ... xn,zn)
     *                        using the forward algorithm
     * @param sequence : is input observation sequence
     */
    void forwardMatrix(vector<int> &sequence);



    void forwardMatrixI(vector<int> &sequence);


    /**
     * @brief likelyhood : method to compute the log
     *                     likeyhood of observed sequence
     * @param sequence    :input observation sequence
     * @return
     */
    float likelyhood(vector<int> sequence);


};



class CHMM
{
public:
    MatrixXf _transition;
    MatrixXf _initial;
    vector<GaussianMixture> _emission;


    int _nstates;
    int _nobs;

    int _seqlen;
    MatrixXf _alpha;
    MatrixXf _beta;
    MatrixXf _scale;

    /**
     * @brief setData : method to set the parameters of the model
     * @param transition : input transition matrix
     * @param emission : input emission gaussian mixture model
     * @param initial : input initial probability matrix
     * @param seqlen  : the maximum sequence length of sequence,for
     *                  allocating matrices
     */
    void setData(Mat transition,Mat initial,vector<GaussianMixture> emission);





    /**
     * @brief forwardMatrix : method computes probability //compute p(x1 ... xn,zn)
     *                        using the forward algorithm
     * @param sequence : is input observation sequence
     */
    void forwardMatrix(Mat &sequence);






    /**
     * @brief likelyhood : method to compute the log
     *                     likeyhood of observed sequence
     * @param sequence    :input observation sequence
     * @return
     */
    float likelyhood(Mat sequence);


};



}
using namespace ocv;
#endif // HMM_H
