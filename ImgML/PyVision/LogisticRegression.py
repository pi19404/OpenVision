"""
Logistic Regression
"""

# Author: pi19404 <pi19404@gmail.com>
#         
#       
#        
#         

import numpy
import numpy as np
import math
from scipy import optimize
from numpy.linalg import norm
import os
import sys
import time
from sklearn.metrics import confusion_matrix
import sklearn.metrics as met
import LoadDataSets
# Minimization routines
import pickle
import sgd;
import pyvision_common as pyvision
from itertools import izip 
import Optimizer
    

" The class encaspulates the Logistic Regression Classification Algorithms "
class LogisticRegression(object):
       
    def __init__(self,n_in,n_out,labels=None,Regularization=2,eta=0.0001):
        """ initialize the parametes of the model """ 
        if(n_in!=0):
            self.n_classes=n_out;
            self.n_dimensions=n_in;
            self.initialize_parameters(n_in,n_out);
        else:
            self.n_dimensions=0;
            self.n_classes=0;
            self.W=[];
            self.b=[];
            self.params=[];
            
        if(labels==None):
            self.labels=xrange(n_out);
            
        self.Regularization=Regularization;
        self.eta=eta;
    
      
    """function to initialize the parameters of the model """      
    def initialize_parameters(self,n_in,n_out):
        print "initializing the parameters of logistic regression"
        self.n_classes=n_out;
        self.n_dimensions=n_in;        
        self.W=numpy.zeros([n_out,n_in],dtype=float);
        self.b=numpy.zeros([n_out,1],dtype=float);
        self.params=numpy.zeros([n_out,n_in+1],dtype=float);
        self.params=self.params.flatten();
        self.nparams=self.n_dimensions+1;
        
        #print np.shape(self.W),np.shape(self.b)
       
       
    """ function predicts the probability of input vector x"""
    """ the output y is MX1 vector (M is no of classse) """
    def predict(self,x):  
        y=pyvision.softmax(self.W,self.b,x);
        self.output=y;
        return y;

    """ function used for label assigment and output probability for predicted index """
    def lable(self,y):
        return self.labels[y];

    def probability(self,y):
        return self.output[y];
        
        
    """ function classifies the input vector x into one of output lables """
    """ input is NXD vector then output is NX1 vector """
    def classify(self,x):
        result=self.predict(x);
        indices=result.argmax(axis=1);
        #converting indices to lables
        lablels=map(self.lable, indices);
        return lablels;     
        
    def vector_mul(x,y):
        return x*y;
        
    """ function computes the negative log likelyhood over input dataset 
         params is optional argument to pass parameter to classifier 
        ,useful in cases of iterative optimization routines for function evaluations like scipy.optimization package """
    def negative_log_likelihood(self,params=None):
        # args contains the training data
        x,y=self.args;
                 
        self.update_params(params);
        sigmoid_activation = pyvision.softmax(self.W,self.b,x);
        index=[range(0,np.shape(sigmoid_activation)[0]),y];
        p=sigmoid_activation[index]
        l=-np.mean(np.log(p));
        
        if self.Regularization==2:
            l=l+0.5*self.eta*np.sum(self.W**2);
        if self.Regularization==1:
            l=l+self.eta*np.sum(np.abs(self.W));
        
        
        return l;
##        self.validate(self.x,self.y)     
    """ function to set the training data for current computation loop"""
    """ useful in running algorithms for batch processing """
    def set_training_data(self,args):
        self.args=args;

    """ function to compute the gradient of parameters for a single data sample """
    def compute_gradients(self,out,y,x):
        out=(np.reshape(out,(np.shape(out)[0],1)));                
        out[y]=out[y]-1;   
        W=out*x.T;
        if self.Regularization==2:
           W=W+self.eta*self.W;
        if self.Regularization==1:
            W=W+self.eta*np.sign(W);
           #print np.shape(r),np.shape(W)
           #dd
        res=np.vstack((W.T,out.flatten()))
        return res;
        

    """ function to compute the gradient of loss function over all input samples
        params is optional input parameter passed to the classifier,which is useful in cases 
        of iterative optimization routines,added for compatiblity with scipi.optimization package """
    def gradients(self,params=None):
        # args contains the training data
        x,y=self.args;
        self.update_params(params);
        sigmoid_activation = pyvision.softmax(self.W,self.b,x);        
        e = [ self.compute_gradients(a,c,b) for a, c,b in izip(sigmoid_activation,y,x)]                         
        mean1=np.mean(e,axis=0);        
        mean1=mean1.T.flatten();
        return mean1;

    """ function to retrieve the params """
    def get_params(self):
        return self.params;


    """ function to set the params """        
    def set_params(self,params):
        self.params=params;
        
    

    """ function to update the weights given parameter array """        
    def update_params(self,params=None):
        if params==None:
            return;
        self.params=params;
        nparam=self.n_dimensions+1;
        param1=params.reshape(-1,nparam);        
        self.W=param1[:,0:nparam-1];
        self.b=param1[:,nparam-1];        
        self.b=np.reshape(self.b,(np.shape(self.b)[0],1))
        
        
    """ function to update the parameter array given weights """
    def update_params_grads(self,W,b):
        nparam=self.n_dimensions+1;
        param1=self.params.reshape(-1,nparam);
        param1[:,0:nparam-1]=W;
        param1[:,nparam-1]=b;
        return param1.flatten();
   
    """ callback function from the optimizer ,can be used to display status,update parameters"""
    def callback(self,w,num,x,y,flag,eta,error=None):       
        self.eta=eta;
        self.params=w;
        self.update_params(self.params);
        if flag==0:
            print "iteration   : ",num,":",np.shape(w);
            l=self.negative_log_likelihood(w);
            print "Loss function   : ",l;
        
    """ the function that performs learning,computing gradients and updating parameters """
    def learn(self,update=None):
        grads=self.gradients();
        params=[];
        if update!=None:
            params=update(self.params,grads)
        nparam=self.n_dimensions+1;
        error=grads.reshape(-1,nparam)[:,nparam-1];  
        return [params,error];


    """ the function performs training for logistic regression classifier """        
    def train(self,train,test,validate):
        if self.n_dimensions==0:
            self.labels=np.unique(train[1]);            
            n_classes = len(self.labels) 
            n_dimensions=np.shape(x)[1];
            self.initialize_parameters(n_dimensions,n_classes);
                
        opti=Optimizer.Optimizer(1000,"SGD",1,2000,0.13,2000*0.99);    
        opti.set_datasets(train,test,validate);
        opti.set_functions(self.negative_log_likelihood,self.set_training_data,self.classify,self.callback,self.learn,None,None);
        opti.run();
            
        #self.labels =np.unique(train[1]);
        #sgd1=sgd.SGD(0.13,600,1000);
        #sgd1.set_datasets(train,test,validate);
        #sgd1.set_functions(self.classify,self.gradients,self.get_params,self.callback);
        
        #sgd1.run();
        
      
""" main test function """
if __name__ == "__main__":    
     model_name1="/home/pi19404/Documents/mnist.pkl.gz"
     data=LoadDataSets.LoadDataSets();
     #[train,test,validate]=data.load_pickle_data(model_name1);
     [train,test,validate]=data.load_sklearn_data("digits");
     
     x=train[0].get_value(borrow=True);
     y=train[1].eval();     
     train=[x[0:5000],y[0:5000]];
     #train=[x,y];

     x=test[0].get_value(borrow=True);
     y=test[1].eval();
     test=[x,y];
     
     x=validate[0].get_value(borrow=True);
     y=validate[1].eval();
     validate=[x,y];
     
     classifier=LogisticRegression(0,0);
     classifier.Regularization=1;
     classifier.train(train,test,validate);

     print np.shape(x),np.shape(y);
     
     
     
     
     
    
#softmax(numpy.array([1,1],[2,2],[3,3]),numpy.array([1,1,1]),numnp.array([1,2]))