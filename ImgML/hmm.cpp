#include "ImgML/hmm.hpp"


/**
 * @brief setData : method to set the parameters of the model
 * @param transition
 * @param emission
 * @param initial
 * @param seqlen  : the maximum sequence length of sequence,for
 *                  allocating matrices
 */
void ocv::DHMM::setData(Mat transition,Mat emission,Mat initial)
{

    _transition=EigenUtils::setData(transition);
    _initial=EigenUtils::setData(initial);
    _emission=EigenUtils::setData(emission);
    _nstates=_transition.rows();
    _nobs=_emission.rows();
    _seqlen=0;


}


//computing p(xn.....xN,zn)
void ocv::DHMM::backwardMatrix(vector<int> &sequence)
{
    _beta=MatrixXf(_nstates,sequence.size()+1);
    int len=sequence.size()+1;
    for(int i=len-1;i>=0;i--)
    {
        for(int j=0;j<_nstates;j++)
        {
            if(i==len-1)
            {
                _beta(j,i)=1;
            }
            else
            {
                float s=0;
                for(int k=0;k<_nstates;k++)
                    s=s+_beta(k,i+1)*_emission(k,sequence[i])*_transition(j,k);
                _beta(j,i)=s*_scale(0,i+1);

            }


        }

    }
}

/**
 * @brief forwardMatrix : method computes probability //compute p(x1 ... xn,zn)
 *                        using the forward algorithm
 * @param sequence : is input observation sequence
 */
void ocv::DHMM::forwardMatrix(vector<int> &sequence)
{

    int len=sequence.size()+1;
    _seqlen=len-1;
    _alpha=MatrixXf(_nstates,_seqlen+1);
    _scale=MatrixXf(1,_seqlen+1);
    for(int i=0;i<len;i++)
    {
    for(int j=0;j<_nstates;j++)
    {
        if(i==0)
            _alpha(j,i)=_emission(j,sequence[i])*_initial(0,j);
        else
        {
            float s=0;
            for(int k=0;k<_nstates;k++)
            s=s+_transition(k,j)*_alpha(k,i-1);
            _alpha(j,i)=_emission(j,sequence[i-1])*s;
        }

    }
    float scale=0;
    for(int j=0;j<_nstates;j++)
    {
        scale=scale+_alpha(j,i);
    }
    scale=1.f/scale;
    if(i==0)
        _scale(0,i)=1;
    else
        _scale(0,i)=scale;

    for(int j=0;j<_nstates;j++)
    {
        _alpha(j,i)=scale*_alpha(j,i);

    }


    }


}


void ocv::DHMM::forwardMatrixI(vector<int> &sequence)
{
    int len=sequence.size()+1;
    for(int i=0;i<len;i++)
    {
    for(int j=0;j<_nstates;j++)
    {
        if(i==0)
            if(j==0)
            _alpha(j,i)=1;//_emission(j,sequence[i])*_initial(0,j);
             else
             _alpha(j,i)=0;
        else
        {
            float s=0;
            for(int k=0;k<_nstates;k++)
            s=s+_transition(k,j)*_alpha(k,i-1);
            _alpha(j,i)=_emission(j,sequence[i-1])*s;
        }

    }
    float scale=0;
    for(int j=0;j<_nstates;j++)
    {
        scale=scale+_alpha(j,i);
    }
    scale=1.f/scale;
    if(i==0)
        _scale(0,i)=1;
    else
        _scale(0,i)=scale;

    for(int j=0;j<_nstates;j++)
    {
        _alpha(j,i)=scale*_alpha(j,i);

    }


    }


}


/**
 * @brief likelyhood : method to compute the log
 *                     likeyhood of observed sequence
 * @param sequence    :input observation sequence
 * @return
 */
float ocv::DHMM::likelyhood(vector<int> sequence)
{
    float prob=0;
    //computing the probability of observed sequence
    //using forward algorithm
    forwardMatrix(sequence);
    backwardMatrix(sequence);

    //computing the log probability of observed sequence
    for(int i=0;i<sequence.size()+1;i++)
    {
        //for(int j=0;j<_nstates;j++)
        {
            prob=prob+std::log(_scale(0,i));
        }



    }

    return -prob;
}




//methods for continuous emission hidden markov models
void ocv::CHMM::setData(Mat transition,Mat initial,vector<GaussianMixture> emission)
{
    _transition=EigenUtils::setData(transition);
    _initial=EigenUtils::setData(initial);
    _emission=emission;
    _nstates=_transition.rows();
    _nobs=_emission.size();
    _seqlen=0;


}



/**
 * @brief forwardMatrix : method computes probability //compute p(x1 ... xn,zn)
 *                        using the forward algorithm
 * @param sequence : is input observation sequence
 */
void ocv::CHMM::forwardMatrix(Mat &sequence)
{


    int len=sequence.rows;
    int dim=sequence.cols;
    _seqlen=len-1;
    _alpha=MatrixXf(_nstates,_seqlen+1);
    _scale=MatrixXf(1,_seqlen+1);

    for(int i=0;i<len;i++)
    {
        //vector<float> val=emission.ProbMix(sequence.row(i));

        for(int j=0;j<_nstates;j++)
        {
            if(i==0)
            _alpha(j,i)=_emission[j].Prob(sequence.row(i)*_initial(0,j));
            else
            {
                float s=0;
                for(int k=0;k<_nstates;k++)
            s=s+_transition(k,j)*_alpha(k,i-1);

            _alpha(j,i)=_emission[j].Prob(sequence.row(i-1))*s;
        }

    }
    float scale=0;
    for(int j=0;j<_nstates;j++)
    {
        scale=scale+_alpha(j,i);
    }
    scale=1.f/scale;
    if(i==0)
        _scale(0,i)=1;
    else
        _scale(0,i)=scale;

    for(int j=0;j<_nstates;j++)
    {
        _alpha(j,i)=scale*_alpha(j,i);

    }


    }


}




/**
 * @brief likelyhood : method to compute the log
 *                     likeyhood of observed sequence
 * @param sequence    :input observation sequence
 * @return
 */
float ocv::CHMM::likelyhood(Mat sequence)
{
    float prob=0;
    //computing the probability of observed sequence
    //using forward algorithm
    forwardMatrix(sequence);    

    //computing the log probability of observed sequence
    for(int i=0;i<sequence.rows;i++)
    {
        //for(int j=0;j<_nstates;j++)
        {
            prob=prob+std::log(_scale(0,i));
        }
    }

    return -prob;
}

