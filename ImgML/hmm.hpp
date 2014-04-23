#ifndef HMM_H
#define HMM_H

#include "ImgML/eigenutils.hpp"
#include "Common/OpenCVCommon.hpp"
#include "ImgML/gaussianmixture.hpp"
#include <ImgConfig/ConfigLib/lib/libconfig.h++>
using namespace std;
using namespace libconfig;
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
    MatrixXd _transition;
    MatrixXd _initial;
    vector<GaussianMixture> _emission;

    float _lthresh;
    int _nstates;
    int _nobs;

    int _seqlen;
    MatrixXd _alpha;
    MatrixXd _beta;
    MatrixXd _scale;


    /**
     * @brief _maxseqlen : this contains the maximim
     * expected sequence length that is to be analyzed.
     * This will be used to allocate required memory
     */
    int _maxseqlen;
    int count;
    float prob;
    /**
     * @brief setData : method to set the parameters of the model
     * @param transition : input transition matrix
     * @param emission : input emission gaussian mixture model
     * @param initial : input initial probability matrix
     * @param seqlen  : the maximum sequence length of sequence,for
     *                  allocating matrices
     */
    void setData(Mat transition,Mat initial,vector<GaussianMixture> emission,float lthresh,int maxseq=50);


    float predictIterative(Mat sequence,bool init);
    void  forwardMatrixIterative(Mat &sequence,bool init,int i);


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


    /**
     * @brief loadConf : the method the load the model parameters
     *from the configuration file
     * @param conf  :the input string containing the path of the
     *configuration files
     */
    bool loadConf(Config &,char *conf);


};

/**
 * @brief The CHMMClassifier class : The class encapsulates
 * methods that performs sequence classification using a
 * vector of Hidden markov models which represent each class
 * of gesture desired to be identified.
 */
class CHMMClassifier
{
public:
    vector<CHMM> _hmm;
    /**
     * @brief loadConf : the method loads the HMM configuration parameters
     * for all the hidden markov models specified in the configuration file.
     * @param str
     */
    bool loadConf(char *str)
    {
        Config cfg;

        // Read the file. If there is an error, report it and exit.
        try
        {
            cfg.readFile(str);
        }
        catch(const FileIOException &fioex)
        {
          std::cerr << "I/O error while reading file." << std::endl;
          return false;
        }
        catch(const ParseException &pex)
        {
          std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                    << " - " << pex.getError() << std::endl;
          return false;
        }

        const Setting& root = cfg.getRoot();
        Setting &hmm = root["HMM"];
        int _nclasses=hmm["nclasses"];
        Setting &_models=hmm["models"];

        for(int i=0;i<_nclasses;i++)
        {
            CHMM hmm1;
            hmm1.loadConf(cfg,(char *)_models[i].c_str());
            _hmm.push_back(hmm1);
        }

    }


    vector<float> classify(Mat x,bool init=false)
    {
        vector<float> result;
        result.resize(_hmm.size());
        for(int i=0;i<_hmm.size();i++)
        {

            result[i]=_hmm[i].likelyhood(x);
        }
        return result;
    }

    vector<float> classifyI(Mat x,bool init=false)
    {
        vector<float> result;
        result.resize(_hmm.size());
        for(int i=0;i<_hmm.size();i++)
        {

            result[i]=_hmm[i].predictIterative(x,init);
            //if(result[i]<=_hmm[i]._lthresh)
            //  result[i]=-1*std::numeric_limits<float>::infinity();
        }
        return result;
    }

    void threshold(vector<float> &lthresh)
    {
        float ls=EigenUtils::logsumexp(lthresh);
        for(int i=0;i<_hmm.size();i++)
        {
             lthresh[i]=std::exp(lthresh[i]-ls);
            //if(<_hmm[i]._lthresh)
              //  lthresh[i]=-1*std::numeric_limits<float>::infinity();
            //if(result[i]<=_hmm[i]._lthresh)
            //  result[i]=
        }
    }


};



}








using namespace ocv;
#endif // HMM_H
