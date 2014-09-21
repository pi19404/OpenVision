# -*- coding: utf-8 -*-
"""
Created on Fri Aug 29 17:12:50 2014

@author: pi19404
"""

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
# Minimization routines


""" logistic function """
def sigmoid(X):
    '''Compute the sigmoid function '''
    den = 1.0 + e ** (-1.0 * X) 
    d = 1.0 / den 
    return d
    
""" softmax function for multi class logistic regression """
def softmax(W,b,x):
     vec=numpy.dot(x,W.T);
     vec=numpy.add(vec,b);
     vec1=numpy.exp(vec);
     res=vec1.T/numpy.sum(vec1,axis=1);
     return res.T;

class LogisticRegression(object):
    """ initialize the parametes of the model """    

         
    """ function predicts the probability of input vector x"""
    """ the output y is MX1 vector (M is no of classse) """
    def predict(self,x):
        y=softmax(self.W,self.b,x);
        return y;

    def lable(self,y):
        return self.labels[y];

    """ function classifies the input vector x into one of output lables """
    """ input is NXD vector then output is NX1 vector """
    def classify(self,x):
        result=self.predict(x);
        indices=result.argmax(axis=1);
        #converting indices to lables
        lablels=map(self.lable, indices);
        return lablels;     
        

    def validate(self,x,y):
        result=self.classify(x);
        
        y_test=y
                
        accuracy=met.accuracy_score(y_test,result)    
        accuracy=1.0-accuracy;
        print "Validation error   " + `accuracy`
        return accuracy;
        
    """ function computes the negative log likelyhood over input dataset """
    def negative_log_likelihood(self,params):

        nparam=self.n_dimensions+1;
        param1=params.reshape(-1,nparam);
        
        self.W=param1[:,0:nparam-1];
        self.b=param1[:,nparam-1];

        #compute the activation function
        sigmoid_activation = softmax(self.W,self.b,self.x);
 
        params=np.zeros(self.n_classes*(self.n_dimensions+1));
        k=0;
        l1=0;
        #select the activation corresponding to the class lable
        for i in self.labels:
            indexes = [j for j,z in enumerate(self.y) if z == i]
            activation=sigmoid_activation[indexes];
            index=[range(0,np.shape(activation)[0]),self.y[indexes]];
            out=activation[index];
            #computing the negative log likelyhood
            l1=l1-np.sum(np.log(out));#+0.5*0.1*norm(param1[k,:],2
            k=k+1;

        print "Log likelyhood",l1/np.shape(self.y)[0],np.shape(self.y)[0]
        
        #computing the mean of log likelyhood of the loss function
        return l1/np.shape(self.y)[0];
     
##        self.validate(self.x,self.y)     
    ''' function to compute the gradient of loss function '''     
    def gradients(self,params):
 
        nparam=self.n_dimensions+1;
        param1=params.reshape(-1,nparam);

        self.W=param1[:,0:nparam-1];
        self.b=param1[:,nparam-1];
        
        #computing the activation function
        sigmoid_activation = softmax(self.W,self.b,self.x);

        params2=param1.flatten();
        k=0;
        ss=0;
        #selecting the activation correponding to class lable of y
        for i in self.labels:
            indexes = [j for j,z in enumerate(self.y) if z == i]
            activation=sigmoid_activation[indexes];
            index=[range(0,np.shape(activation)[0]),self.y[indexes]];
            ss=ss+np.shape(activation)[0];
            out=activation[index];
                        
            res=self.x[indexes].T*(1-out);
            N=np.shape(self.x[indexes].T)[1];
            
            #computing the derivatives
            gradw=np.mean(res,axis=1);#+0.1*(param1[k,0:nparam-1])[0];
            gradb=np.mean(1-out);#+0.01*(param1[k,nparam-1]

            #updating the gradient values
            params2[k*(self.n_dimensions+1):(k+1)*(self.n_dimensions+1)-1]=-gradw;
            params2[(k+1)*(self.n_dimensions)]=-gradb;
            k=k+1;
      
        return params2;
        

    def callback(self,w):
        print "iteration   : ",self.num;
        self.params=w;
        l=self.negative_log_likelihood(self.params);
        print "Loss function   : ",l;
        self.validate(self.x,self.y);
        self.num=self.num+1;
        
        
    def fit(self,x,y):
        self.num=0;
        self.labels =np.unique(y)
        self.n_classes = len(self.labels)        
        self.n_dimensions=np.shape(x)[1];
        self.n_samples=np.shape(x)[0];
        print "number of dimensions : ",self.n_dimensions;
        print "number of classes  :",self.n_classes;
        print "number of training samples :",self.n_samples;
        self.W=numpy.zeros([self.n_classes,self.n_dimensions],dtype=float);
        self.b=numpy.zeros([self.n_classes,1],dtype=float);
        self.params=numpy.zeros([self.n_classes,self.n_dimensions+1],dtype=float);
        self.params=self.params.flatten();
        self.x=x;
        self.y=y;
        res=self.params;

        res=optimize.fmin_cg(self.negative_log_likelihood,res,fprime=self.gradients,maxiter=2,callback=self.callback);
        

        self.target=self.y;
        
        

if __name__ == "__main__":    

     model_name1="/home/pi19404/Documents/mnist.pkl.gz"
     data=LoadDataSets.LoadDataSets();
     [train,test,validate]=data.load_pickle_data(model_name1);
     x=train[0].get_value(borrow=True);
     y=train[1].eval();
     classifier=LogisticRegression();
     classifier.fit(x,y);
     print np.shape(x),np.shape(y);
     
     
     
     
     
    
#softmax(numpy.array([1,1],[2,2],[3,3]),numpy.array([1,1,1]),numnp.array([1,2]))