#ifndef RANDOMFERMS_H
#define RANDOMFERMS_H
#include "Common/OpenCVCommon.hpp"
class RandomFerns
{
public:


 //number of features per ground
 int _numFeatures;
 int _numIndices ;
 //number of groups
 int _numFerns;
 //2D points at which features need to be computed
 vector<vector<Point2f> >points;

 int index;
 vector<float> posteriors;
 vector<float> positive;
 vector<float> negatives;



 //constructor for random ferns feature evaluator
 RandomFerns();

 //initialize the probabilities
 void initProb();


 //compute the posterior probability associated with the feature
 float calcConfidence(vector<int> features);

 //update posterior probabilities while learning
 void updatePosterior(vector<int> features,bool class1,int ammount);


//saving to file
 void writeToFile(const char *path);

 //loading
 void readFromFile(const char *path);


 //compute the location points for the binary tests
 void init();


 //function to compute the binary feature given a rectangular feature and a image
 //each fern is represented by a binary string
vector<int> computeFeatures(const Rect r,const Mat image);


 //main function that compute the binary feature corresponding to all the ROI
 //in the image
vector<Rect> compute(vector<Rect> r,const Mat image);


 //learn
 void learn(Rect r,Mat image,bool class1,bool force=0);


 //function to classify the window
 bool classify(Rect r,Mat image);





};


#endif // RANDOMFERMS_H
