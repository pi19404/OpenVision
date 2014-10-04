# -*- coding: utf-8 -*-
"""
Created on Mon Sep 22 01:44:17 2014

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
import LoadDataSets
import LogisticRegression
import cPickle as pickle
import pyvision_common as pyvision
from itertools import izip 
import Optimizer
from sklearn.datasets import load_digits
rng = numpy.random.RandomState(200)
""" logistic function """
def sigmoid(X):
    '''Compute the sigmoid function '''
    den = 1.0 + math.e ** (-1.0 * X) 
    d = 1.0 / den 
    return d
    
def sigmoid_stable(x):
    "Numerically-stable sigmoid function."
    if x >= 0:
        z = np.exp(-x)
        return 1 / (1 + z)
    else:
        # if x is less than zero then z will be small, denom can't be
        # zero because it's 1+z.
        z = np.exp(x)
        return z / (1 + z)
        
def grad_sigmoid(X):
    s=sigmoid(X);
    return s*(1-s);
    
sigmoid_vec = np.vectorize(sigmoid)

grad_sigmoid_vec = np.vectorize(grad_sigmoid)

""" the class absracts the hidden layer in a Multi Layer perceptron feed forward neural network 
    and is essentially a collection of neurons """
     
class HiddenLayer:

    def __init__(self,n_in=None,n_out=None,activation=None,Reg=2,W=None,b=None):
        """Initialization function for the hidden layer.
      
        All the initialization parameters are optional 
        
        Parameters
        ----------
        n_in   :  dimension of input vector
        n_out  :  dimension of the output vector
        activation : activation function typicall sigmoid or tanh
        Reg : regularization option 1=L1 and 2=L2
        W,b : intial weight matrix and bias vector
        
        W is matix of dimension n_inxn_out and b is vector of size n_outx1
        """    
        if n_in ==None:
            return;
        if W is None:
             low=-4*numpy.sqrt(6. / (n_in + n_out));
             high=4*numpy.sqrt(6. / (n_in + n_out));
             size=(n_out, n_in);
             self.W=numpy.asarray(rng.uniform(low,high,size),dtype=float);
         
        if b is None:
              self.b = numpy.zeros((n_out,),dtype=float);
    
        self.Regularization=Reg;
        self.n_out=n_out;
        self.n_in=n_in;
        self.activation=activation;
        self.params=numpy.zeros([n_out,n_in+1],dtype=float);
        self.params=self.params.flatten();
        self.nparam=self.n_in+1;

        param1=self.params.reshape(-1,self.nparam);
        
        param1[:,0:self.nparam-1]=self.W;
        param1[:,self.nparam-1]=self.b;   
        self.params=param1.flatten();
        self.labels=np.array(xrange(0,n_out));
        self.eta=0.001    
     
       

    
    def compute(self,input):
        """function computers the output of the hidden layer.
      
        Parameters
        ----------
        input   :  input is a vector of dimension n_in

        Returns
        -----------
        output  : output is a vector of dimension n_out
        """        
        
        
        
        linout=numpy.dot(self.W,input.T)+np.reshape(self.b,(self.b.shape[0],1));     
        
        self.output=self.activation(linout).T;                 
        return self.output;


       
    def activation_gradient(self):
        """ function computes the value of gradient of activation funcnction 
        """
        out1=np.multiply(self.output,(1-self.output));
        return out1;


    """ function to set the training data for current computation loop"""
    """ useful in running algorithms for batch processing """
    def set_training_data(self,args):
        self.args=args;

       
    """ the function compues gradient of likelyhodd function wrt output"""       
    def linear_gradient(self,weights,error):   
 
            return numpy.dot(error,weights);

    def compute_error(self,x,w,y):      
        x=x*w;
        
        
        res1=x.reshape(x.shape[0],1);

        res=np.dot(res1,y.reshape(y.shape[0],1).T);
        self.eta=0.0001
        if self.Regularization==2:
           res=res+self.eta*self.W;
        if self.Regularization==1:
           res=res+self.eta*np.sign(self.W);
            
        res=np.hstack((res,res1));
        
        return res.T;
        
                        
    """ function to compute the gradient of log likelyhood function wrt the parameters of the hidden layer """                        
    def cost_gradients(self,weights,activation,error):        
        we=self.linear_gradient(weights,error);
        e=[ self.compute_error(a,we,b) for a,b in izip(self.activation_gradient(),activation)]
        gW=np.mean(e,axis=0).T;        
        return gW;

    """ function to upgrade the parameters """        
    def update_gradients(self,grads,learning_rate):
        #print np.shape(grads[0]),np.shape(grads[1])
        self.params=grads;
        param1=self.params.reshape(-1,self.nparam);
        self.W=param1[:,0:self.nparam-1];
        self.b=param1[:,self.nparam-1];
        

        

    


class MLP(object):
   """ Class with implements the Multi layer perceptron feed forward neural networks"""    
   def __init__(self,n_in,n_hidden_layers,n_hidden_units,n_out):       
            self.n_hidden_layers=n_hidden_layers;
            self.n_hidden_units=n_hidden_units;
            self.n_in=n_in;
            self.n_out=n_out;
            if n_hidden_layers==0:
                n_hidden_units=n_in;
            self.hiddenLayer = [HiddenLayer(n_in=n_in, n_out=n_hidden_units,activation=sigmoid_stable) for i in range(n_hidden_layers)];    
            self.logRegressionLayer = LogisticRegression.LogisticRegression(n_hidden_units,n_out);

   
   def lable(self,y):
      """ mapping functions for output label and probability """
      return self.labels[y];

  
   def probability(self,y):
       """ mapping functions for output label and probability """
       return self.temp_output[y];   
            
   
   def propagate_forward(self,input):
       """the function that performs forward iteration to compute the output"""
       self.predict(input)
   
 

   def set_training_data(self,args):
        """ function to set the training data for current computation loop"""
        """ useful in running algorithms for batch processing """       
        self.args=args;   

               
   def propagate_backward(self,error,weights,input):                 
        """ the function that executes the backward propagation loop on hidden layers"""                              
            
        input1=input;
        for i in range(self.n_hidden_layers):                        
            prev_error=np.inf;
            best_grad=[];
            for k in range(1):
                """ computing the derivative of the parameters of the hidden layers"""
                hidden_layer=self.hiddenLayer[self.n_hidden_layers-i-1];
            #args=(input1);
            #hidden_layer.set_training_data(args)
                hidden_layer.compute(input1);
          
                """ computing the gradient of likelyhood function wrt the parameters of the hidden layer """
            #print "weights",np.shape(hidden_layer.W);
            
            ##returns flattened structure,can get error seperately
                grad=hidden_layer.cost_gradients(weights,input1,error);

                #e1=np.mean(np.abs(grad[:,hidden_layer.n_in]))
            #print np.shape(grad),np.shape(hidden_layer.params)
                res=self.update(hidden_layer.params,grad.flatten(),0.13);
                #if e1 < prev_error:
                #    e1=prev_error;
                #    best=res;
                    
                
            #print np.shape(grad),hidden_layer.n_in+1
            
                """ update the parameters """
                hidden_layer.update_gradients(res,0.9);
            #hidden_layer.update_gradients(best,0.9);
            #grad=hidden_layer.cost_gradients(weights,input1,error);
            weights=hidden_layer.W;
            error=grad[:,hidden_layer.n_in];                        
            self.hiddenLayer[self.n_hidden_layers-i-1]=hidden_layer;
            ###need to change this
            #error=grad[1];
            input1=hidden_layer.output;
                    
                    
   def callback(self,w,num,x,y,flag,eta):    
        """ The callback function from optimizer,can be used to display periodic updates """                               
        #self.eta=eta;
        #self.params=w;
        #self.update_params(self.params);
        if flag==0:
            #print "iteration   : ",num,":",np.shape(w);
            l=self.cost();
            print "Loss function   : ",l;       
            #file_name=self.__class__.__name__+"_"+str(num)+".pyvision";
            file_name=self.__class__.__name__+".pyvision1";
            self.save(file_name);       

          

               
   def cost(self):
       """ the function computer the likelyhood taking into account regularization over all hidden layers """
       l=self.logRegressionLayer.negative_log_likelihood()
       self.eta=0.0001
       if self.n_hidden_layers>0:
           for i in range(self.n_hidden_layers):
               hidden_layer=self.hiddenLayer[self.n_hidden_layers-i-1];
               l=l+self.eta*np.mean(np.log(hidden_layer.W**2));
      
                    
       return l;
    
                    
                
  
   def learn(self,update):
        """ the function that performs learning,computing gradients and updating parameters """
        x,y=self.args;
        self.update=update;                        
        """ forward algorithm loop """
        self.propagate_forward(x)        
        args1=(self.hidden_output,y);
        self.logRegressionLayer.set_training_data(args1);
        [params,grad]=self.logRegressionLayer.learn(update);
        self.logRegressionLayer.update_params(params);
        #[params,grad]=self.logRegressionLayer.learn();
        """ initialize the error and weights for backpropagation """           
        
        error=grad;#[:,self.logRegressionLayer.n_dimensions+1];
        weights=self.logRegressionLayer.W;
        #print np.shape(weights),"::"
        """ performing operations on hidden layers """
        if self.n_hidden_layers >0:   
             weights=self.logRegressionLayer.W;
             self.propagate_backward(error,weights,x)
             
        return [None,None];                

                          
   """ the function predicts the output of the MLP feed forward network given the input X """                          
   def predict(self,x):
       input=x;
       for i in range(self.n_hidden_layers):
       #    print "computing hidden layer",i
           hidden=self.hiddenLayer[i];
           input=hidden.compute(input)
           self.hiddenLayer[i]=hidden;

       self.hidden_output=input;
      
       o=self.logRegressionLayer.predict(input);
       self.output=o;
       
       return o;
       
   """ the method performs classificaiton by assigning each input vector x to one of defined class lables """      
   def classify(self,x):
        output=self.predict(x);        
        indices=output.argmax(axis=1);            
        labels=map(self.lable, indices); 
        return labels;
    
   """ the function saves the trainied model parameters to output file """
   def save(self,file_name):
       with open(file_name, 'wb') as output:
           pickle.dump( self.n_hidden_layers, output )
           pickle.dump(self.labels,output);
           for i in range(self.n_hidden_layers):
               hidden_layer=self.hiddenLayer[i];
           #file_name1=file_name+"_"+i+".pkl";
               print "saving hidden layer ",i,"in file",file_name
               pickle.dump( hidden_layer, output )
        
           output_layer=self.logRegressionLayer;
       #file_name1=file_name+"_"+i+".pkl";
           pickle.dump( output_layer, output)
    
   """ the method loads the trained model parameters from output file """
   def load(self,file_name):
       with open(file_name, 'rb') as input:
           self.n_hidden_layers=pickle.load(input);
           #print self.n_hidden_layers
           self.labels=pickle.load(input);
           #print self.labels
           self.hiddenLayer = [HiddenLayer() for i in range(self.n_hidden_layers)]; 
       
           
           for i in range(self.n_hidden_layers):
               print i
               self.hiddenLayer[i]=pickle.load(input);
               print self.hiddenLayer[i].__class__.__name__
           
           self.logRegressionLayer=pickle.load(input)
           print self.logRegressionLayer.__class__.__name__
           self.n_in=self.hiddenLayer[0].n_in;
           self.n_out=len(self.labels);
           self.n_hidden_units=self.hiddenLayer[0].n_out;
           
    
   """ the function performs training for logistic regression classifier """        
   def train(self,train,test,validate):
        self.labels=np.unique(train[1]);            
                
        opti=Optimizer.Optimizer(1000,"SGD",1,200,0.13,100*0.001);    
        opti.set_datasets(train,test,validate);
        opti.set_functions(self.logRegressionLayer.negative_log_likelihood,self.set_training_data,self.classify,self.callback,self.learn,None,None);
        opti.run();
     
     
if __name__ == "__main__":    

     model_name1="/home/pi19404/Documents/mnist.pkl.gz"
     data=LoadDataSets.LoadDataSets();
     [train,test,validate]=data.load_pickle_data(model_name1);
     x=train[0].get_value(borrow=True);
     y=train[1].eval();     
     train=[x,y];
     #train=[x,y];

     x=test[0].get_value(borrow=True);
     y=test[1].eval();
     test=[x,y];
     
     x=validate[0].get_value(borrow=True);
     y=validate[1].eval();
     validate=[x,y];

     labels =np.unique(y);        
     n_classes = len(labels);
     n_dimensions=np.shape(x)[1];
                
     classifier=MLP(n_dimensions,1,100,n_classes);
     classifier.load("MLP.pyvision1");
     classifier.train(train,test,validate);
       