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
import LogisticRegression
import cPickle as pickle


__author__ = "pi19404 <pi19404@gmail.com>"
__version__ = "0.0.1"

def sigmoid(X):
    """ This function applies sigmoid transformation on each element of input vector 
    :param X :Real Valued Input vector 
    :returns :Real Valued output Vector
    """
    den = 1.0 + math.e ** (-1.0 * X) 
    d = 1.0 / den 
    return d
    

def softmax(W,b,x):
     """ This function appliers softmax function to the input vector for multi class logistic regression """
     
     """ compute the linear transformation """
     #print np.shape(x),np.shape(W.T),np.shape(b)
     vec2=numpy.dot(x,W.T).T;                 
     
     vec=numpy.add(vec2,b);
     #step performed for numerical stability of softmax function
     #else for large values will get NaNs due to exp
     #normalizing the input provides numerical stability
     
     o1=numpy.add(vec,-vec.max(axis=0));
  
     vec1=numpy.exp(o1);

     res=(vec1)/(numpy.sum(vec1,axis=0));
     #print np.shape(res),res
     #ddd
     #added warning in case of numerical instability"""
     if np.sum(np.isnan(res)==True)>1:
         print "softmax has become numerically unstable",np.sum(np.isnan(res)==True)
         raise
     return res.T;

""" function can be used to compute the derivative of p norm for p>1"""
def derivative_L2norm(W,p):
#    den=np.apply_along_axis(np.linalg.norm ,1, W,p)    
#    den=pow(den,p-1);
#    num=W*pow(abs(W),p-2);
#    ret=(num.T/den).T;
#    if np.sum(np.isnan(ret)==True)>0:
#        print "XXX"
    num=W;
    den=np.sum(W**2,axis=1);
    r=(num.T/den).T;
    index=np.isnan(r)==True;
    r[index]=0;
    return r
