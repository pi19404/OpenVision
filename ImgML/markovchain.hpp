#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP
#include "ImgML/eigenutils.hpp"
#include "Common/OpenCVCommon.hpp"
#include <time.h>       /* time */
/**
 * @brief The markovChain class : markovChain is a class
 * which encapsulates the representation of a discrete markov
 * chain.A markov chain is composed of transition matrix
 * and initial probability matrix
 */
class markovChain
{
public:
    markovChain()
    {
        srand(time(NULL));
    }

    /* _transition holds the transition probability matrix
     * _initial holds the initial probability matrix
     */
    MatrixXf _transition;
    MatrixXf _initial;

    /**
     * @brief setModel : function to set the parameters of the model
     * @param transition : NXN transition matrix
     * @param initial : 1XN initial probability matrix
     */
    void setModel(Mat transition,Mat initial)
    {
        _transition=EigenUtils::setData(transition);
        _initial=EigenUtils::setData(initial);
    }

    /**
     * @brief computeProbability : compute the probability that the sequence
     *        is generate from the markov chain
     * @param sequence : is a vector of integral sequence starting from 0
     * @return         : is probability
     */
    float computeProbability(vector<int> sequence)
    {
        float res=0;
        float init=_initial(0,sequence[0]);
        res=init;
        for(int i=0;i<sequence.size()-1;i++)
        {
            res=res*_transition(sequence[i],sequence[i+1]);
        }
        return res;

    }

    /**
     * @brief initialRand : function to generate a radom state
     * @param matrix      : input matrix
     * @param index       ; row of matrix to consider
     * @return
     */
    int initialRand(MatrixXf matrix,int index)
    {

        float u=((float)rand())/RAND_MAX;
        cerr << u << endl;
        float s=matrix(0,0);
        int i=0;
        //select the index corresponding to the highest probability
        //or if all the cols of matrix have transitioned
        while(u>s & (i<matrix.cols()))
        {
            i=i+1;
            s=s+matrix(index,i);
        }
        return i;
    }

    /**
     * @brief generateSequence is a function that generates a sequence of specified length
     * @param n : is the length of the sequence
     * @return : is a vector of integers representing generated sequence
     */
    vector<int> generateSequence(int n)
    {

        vector<int> result;
        result.resize(n);
        int i=0;
        int index=0;
        //select a random initial value of sequence
        int init=initialRand(_initial,0);
        result[i]=init;
        index=init;
        for(i=1;i<n;i++)
        {
        //select a random transition to next sequence state
        index=initialRand(_transition,index);
        result[i]=index;
        }
        return result;
    }



};

class MarkovSequenceClassifier
{
    vector<markovChain> models;




};


#endif // MARKOVCHAIN_HPP
