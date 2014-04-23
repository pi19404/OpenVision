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

    _transition=EigenUtils::setDataf(transition);
    _initial=EigenUtils::setDataf(initial);
    _emission=EigenUtils::setDataf(emission);
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
void ocv::CHMM::setData(Mat transition,Mat initial,vector<GaussianMixture> emission,float _lthresh1,int maxseq)
{
    _transition=EigenUtils::setData(transition);
    _initial=EigenUtils::setData(initial);

    _initial=_initial.array().log().matrix();
    _transition=_transition.array().log().matrix();
    _emission=emission;
    _nstates=_transition.rows();
    _nobs=_emission.size();
    _seqlen=0;
    _maxseqlen=maxseq;
    _lthresh=_lthresh1;

    _alpha=MatrixXd(_nstates,_maxseqlen);
    _scale=MatrixXd(1,_maxseqlen);

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

    for(int i=0;i<len;i++)
    {


        for(int j=0;j<_nstates;j++)
        {
            if(i==0)
            {
            _alpha(j,i)=_emission[j].Prob(sequence.row(i))*_initial(0,j);
            }
            else
            {
                float s=0;
                for(int k=0;k<_nstates;k++)
                s=s+_transition(k,j)*_alpha(k,i-1);

                //changed from sequence.row(i-1) to sequence.row(i)
                //need to verifyt the forward algorithm
                _alpha(j,i)=_emission[j].Prob(sequence.row(i))*s;
            }

    }



    float scale=0;
    for(int j=0;j<_nstates;j++)
    {
        scale=scale+_alpha(j,i);
    }
    //scale=scale+std::numeric_limits<float>::epsilon();
    if(scale==0)
        scale=std::numeric_limits<float>::epsilon();
    scale=1.f/(scale);
    //commented the below code need to verify
    //if(i==0)
//       _scale(0,i)=1;
  //  else
        _scale(0,i)=scale;

    for(int j=0;j<_nstates;j++)
    {
        _alpha(j,i)=scale*_alpha(j,i);
    }


    }



}



/**=
 * @brief forwardMatrix : method computes probability //compute p(x1 ... xn,zn)
 *                        using the forward algorithm
 * @param sequence : is input observation sequence
 */
void ocv::CHMM::forwardMatrixIterative(Mat &sequence,bool init,int i)
{



    int dim=sequence.cols;



        for(int j=0;j<_nstates;j++)
        {
            if(init==true||i==0)
            {
            _alpha(j,0)=_emission[j].Prob(sequence)+(_initial(0,j));
            }
            else
            {
                float s=0;
                vector<float> work;
                for(int k=0;k<_nstates;k++)
                work.push_back((_transition(k,j))+_alpha(k,i-1));

\
                float r=EigenUtils::logsumexp(work);
                //changed from sequence.row(i-1) to sequence.row(i)
                //need to verifyt the forward algorithm
                _alpha(j,i)=_emission[j].Prob(sequence)+r;
            }
            //cerr << _emission[j].Prob(sequence) << endl;


        }


    float scale=0;
    vector<float> alpha;
    for(int j=0;j<_nstates;j++)
    {
        if(_alpha(j,i)<=-1e200)
            _alpha(j,i)=-1*std::numeric_limits<float>::infinity();
        alpha.push_back(_alpha(j,i));
    }
    scale=EigenUtils::logsumexp(alpha);
    //cerr << scale << ":" ;
    //if(scale==0)
    //    scale=std::numeric_limits<float>::epsilon();
    //scale=1.f/(scale);



    //commented the below code need to verify
    //if(i==0)
//       _scale(0,i)=1;
  //  else
    _scale(0,i)=scale;

    /*for(int j=0;j<_nstates;j++)
    {
        _alpha(j,i)=scale*_alpha(j,i);
    }*/



}



bool CHMM::loadConf(Config &cfg,char *model_str)
{


    const Setting& root = cfg.getRoot();
    Setting &hmm = root["HMM"];
    Setting &_nclasses=hmm["nclasses"];
    Setting &_models=hmm["models"];
    int ndim=(int)hmm["ndim"];
    Setting &model=hmm[(const char *)model_str];



        int nstates=model["nstates"];
        int ncomponents=model["ncomponents"];
        float lthresh=model["lthresh"];
        //lthresh=lthresh;




        Mat _transition=Mat(nstates,nstates,CV_32FC1);
        Mat _initial=Mat(1,nstates,CV_32FC1);
        vector<GaussianMixture> gauss;
        gauss.resize(nstates);

        Setting &transition = model["transition"];
        for(int j=0;j<transition.getLength();j++)
        {
            _transition.at<float>(j)=(float)transition[j];
        }
        Setting &initial = model["initial"];
        for(int j=0;j<initial.getLength();j++)
        {
            _initial.at<float>(j)=(float)initial[j];
        }


        for(int j=0;j<nstates;j++)
        {
            char str[100];
            sprintf(str,"emission%d",j);
            Setting &emission = model[(const char *)str];
            GaussianMixture g=gauss[j];
            g.setnmix(ncomponents);

            for(int k=0;k<ncomponents;k++)
            {
                Gaussian g1;
                sprintf(str,"gmm%d",k);
                Setting &mix=emission[(const char *)str];

                Setting &mean=mix["mean"];
                Mat _mean=Mat(1,ndim,CV_32FC1);
                //_mean.resize(ndim);
                for(int l=0;l<ndim;l++)
                {
                    _mean.at<float>(l)=(float)mean[l];
                }
                g1.setMean(_mean);


                Setting &covar=mix["covar"];


                Mat _covar=Mat(ndim,ndim,CV_32FC1);
                int cc=0;
                for(int l=0;l<ndim;l++)
                {
                    for(int m=0;m<ndim;m++)
                    {
                    _covar.at<float>(l,m)=(float)covar[cc];
                    cc++;
                    }
                }

                g1.setSigma(_covar);

                float weight=(float)mix["weight"];

                g.setGaussian(g1,weight);

            }
            gauss[j]=g;

        }


        this->setData(_transition,_initial,gauss,lthresh);



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

/**
 * @brief likelyhood : method to compute the log
 *                     likeyhood of observed sequence
 * @param sequence    :input observation sequence
 * @return
 */
float ocv::CHMM::predictIterative(Mat sequence,bool init)
{

    //computing the probability of observed sequence
    //using forward algorithm

    if(init==true)
    {
        count=0;
        prob=0;
        _alpha=MatrixXd(_nstates,_maxseqlen);
        _scale=MatrixXd(1,_maxseqlen);

    }
    int i=count;

      forwardMatrixIterative(sequence,init,i);
      prob=(_scale(0,i));

      //prob=-prob;
      //cerr << prob <<":" << count <<"--";
    count=count+1;

    return prob;
}
