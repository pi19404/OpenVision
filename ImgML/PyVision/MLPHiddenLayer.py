#!/usr/bin/python
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

     
class HiddenLayer:
    """ the class absracts the hidden layer in a Multi Layer perceptron feed forward neural network 
    and is essentially a collection of neurons 
           
    Parameters
    ----------
    n_in   :  dimension of input vector
    n_out  :  dimension of the output vector
    activation : activation function typicall sigmoid or tanh
    Reg : regularization option 1=L1 and 2=L2
    W,b : intial weight matrix and bias vector
    
    W is matix of dimension n_inxn_out and b is vector of size n_outx1

    
    Attributes        
    -----------
    `out` : array-like ,shape=[n_out]
    The output of hidden layer 
    
    `params`:array-like ,shape=[n_out,n_in+1]        
    contains the parameters in a flattened structure
    
    `W,b`:array-like,shape=[n_out,n_int],shape=[n_out,1]
     weight matrix and bias vector characterizing the hidden layer
     
     `activation`:function
     the non linear activation function that is applied after performing
     affine transformation over input vector.
    
    Examples
    -----------    
    >> hidden_layer=HiddenLayer(n_in=n_in, n_out=n_hidden_units,activation=sigmoid_stable)
    >> y=hidden_layer.compute(input);
    
    """    

    def __init__(self,n_in=None,n_out=None,activation=None,Reg=2,W=None,b=None):        
        if n_in ==None:
            return;
        """ random  initialization of weight matrix  """
        if W is None:
             low=-4*numpy.sqrt(6. / (n_in + n_out));
             high=4*numpy.sqrt(6. / (n_in + n_out));
             size=(n_out, n_in);
             self.W=numpy.asarray(rng.uniform(low,high,size),dtype=float);
         
        if b is None:
              self.b = numpy.zeros((n_out,),dtype=float);

        """ storing the  other initialization parameters """    
        
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
        #self.labels=np.array(xrange(0,n_out));
        self.eta=0.001    
     
       

    
    def compute(self,input):
        """function computes the output of the hidden layer for input matrix
      
        Parameters
        ----------
        input   :   numpy array,shape=(N,n_in)
                    :math:`h_{i-1}(x)` is the `input`

        Returns
        -----------
        output  : numpy array,shape=(N,n_out)
                    :math:`f(b_k + w_k^T h_{i-1}(x))`
        """                
        #performs affine transformation over input vector        
        linout=numpy.dot(self.W,input.T)+np.reshape(self.b,(self.b.shape[0],1));     
        #applies non linear activation function over computed linear transformation
        self.output=self.activation(linout).T;                 
        return self.output;


       
    def activation_gradient(self):
        """ function computes the value of gradient of activation function for output of hidden layer over all input samples N
        
        
            
        Returns
        ---------
        output : numpy array , shape=(N,n_out)
                :math:`h_k(x)=f(a_k)=\\begin{align} \\frac{\partial \mathbf{h}_{k-1,j} }{\partial \mathbf{a}_{k-1,j}} \end{align}`
        """
        out1=np.multiply(self.output,(1-self.output));
        return out1;


    def set_training_data(self,args):
        """ Function to set the training data for current computation loop
            useful in running algorithms for batch processing """
        self.args=args;

       
    
    def linear_gradient(self,weights,error):   
            """ The function compues gradient of likelyhodd function wrt output of hidden layer
                    
            \frac{\partial L }{\partial \mathbf{h}_{k-1,j}}
            
            Parameters 
            ------------
            weights : weights of next hidden layer
            error   : backpropagated error from next layer
        
            
            """            
            
            return numpy.dot(error,weights);


    def compute_error(self,x,w,y):      
        """                 
        This function computes the gradient of the likelyhood function wrt to parameters  of the hidden layer for single input
        

        Parameters 
        -------------
        x : numpy array,shape=(n_out,)
            `x` represents :math:`\\begin{align} \\frac{\partial \mathbf{h}_{k,j} }{\partial \mathbf{a}_{k,j}} \end{align}`,the gradient of activation function wrt to input
        w : numpy array,shape=(n_out,)
            `w` represents :math:`\\begin{align} \\frac{\partial L }{\partial \mathbf{h}_{k,i}}\end{align}` the gradient of the likelyhood fuction wrt output of hidden layer
        y : numpy array,shape=(n_in,)
            `y` represents :math:`h_{k-1,j}` the input hidden layer
        
        Returns
        ------------
        res : numpy array,shape=(n_in+1,n_out)        
              :math:`\\begin{align} \\frac{\partial L }{\partial \mathbf{W}_{k-1,i,j}}  \\text{ and } \\frac{\partial L }{\partial \mathbf{W}_{k-1,i}} \end{align}`
        """        
       
        
        x=x*w;                
        #gradient of likelyhood function wrt input activation
        res1=x.reshape(x.shape[0],1);
        #gradient of likelyhood function wrt weight matrix
        res=np.dot(res1,y.reshape(y.shape[0],1).T);
        self.eta=0.0001
        #code for L1 and L2 regularization 
        if self.Regularization==2:
           res=res+self.eta*self.W;
        if self.Regularization==1:
           res=res+self.eta*np.sign(self.W);

        #stacking the parameters and preparing for returning            
        res=np.hstack((res,res1));
        print np.shape(res.T)
        ddd
        return res.T;
        
                        
    
    def cost_gradients(self,weights,activation,error):        
        """ function to compute the gradient of log likelyhood function wrt the parameters of the hidden layer
        averaged over all the input samples.        
        """                        
        
        we=self.linear_gradient(weights,error);
        e=[ self.compute_error(a,we,b) for a,b in izip(self.activation_gradient(),activation)]
        gW=np.mean(e,axis=0).T;        
        return gW;


    def update_gradients(self,grads,learning_rate):
        """ function to updated the learn parameters to the model """
        
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
        #input matrix for the hidden layer    
        input1=input;
        for i in range(self.n_hidden_layers):                        
            prev_error=np.inf;
            best_grad=[];
            for k in range(1):
                """ computing the derivative of the parameters of the hidden layers"""
                hidden_layer=self.hiddenLayer[self.n_hidden_layers-i-1];
                hidden_layer.compute(input1);
          
                # computing the gradient of likelyhood function wrt the parameters of the hidden layer 
                grad=hidden_layer.cost_gradients(weights,input1,error);
                #update the parameter of hidden layer
                res=self.update(hidden_layer.params,grad.flatten(),0.13);
            
                """ update the parameters """
                hidden_layer.update_gradients(res,0.9);
            #set the weights ,inputs and error required for the back propagation algorithm
            #for the next layer
            weights=hidden_layer.W;
            error=grad[:,hidden_layer.n_in];                                    
            self.hiddenLayer[self.n_hidden_layers-i-1]=hidden_layer;
            input1=hidden_layer.output;
                    
                    
   def callback(self,w,num,x,y,flag,eta):    
        """ The callback function from optimizer,can be used to display periodic updates """                               
        #compute likelyhood function if  instructed by optimizer
        if flag==0:
            l=self.cost();
            print "Loss function   : ",l;       
            #save the model file
            file_name=self.__class__.__name__+".pyvision1";
            self.save(file_name);       

          

                  
   def cost(self):
       """ the function computer the likelyhood taking into account regularization over all hidden layers """
       #compute the cost of prediction
       l=self.logRegressionLayer.negative_log_likelihood()
       self.eta=0.0001
       #incorporate the prior likelihood of hidden layers
       if self.n_hidden_layers>0:
           for i in range(self.n_hidden_layers):
               hidden_layer=self.hiddenLayer[self.n_hidden_layers-i-1];
               l=l+self.eta*np.mean(np.log(hidden_layer.W**2));
      
       #return the compute cost             
       return l;
    
                    
                
  
   def learn(self,update):
        """ the main function that performs learning,computing gradients and updating parameters """
        x,y=self.args;
        self.update=update;                        
        #execute the forward iteration loop
        self.propagate_forward(x)        
        args1=(self.hidden_output,y);
        #set the input for the output logistic regression layer
        self.logRegressionLayer.set_training_data(args1);
        #gradient computation and parameter updation of output layer
        [params,grad]=self.logRegressionLayer.learn(update);
        self.logRegressionLayer.update_params(params);
        #[params,grad]=self.logRegressionLayer.learn();
                   
        #initialize the gradiients and weights for backward error propagation
        error=grad;
        weights=self.logRegressionLayer.W;
        
        #perform the backward iteration over the hidden layers
        if self.n_hidden_layers >0:   
             weights=self.logRegressionLayer.W;
             self.propagate_backward(error,weights,x)
             
        return [None,None];                

                          

   def predict(self,x):
       """ the function predicts the output of the MLP feed forward network given the input X """                                 
       input=x;
       #loop for computing output of each hidden layer
       for i in range(self.n_hidden_layers):
       #    print "computing hidden layer",i
           hidden=self.hiddenLayer[i];
           #setting the output of present hidden layer as input to the next
           input=hidden.compute(input)           
           self.hiddenLayer[i]=hidden;
       #the input to output layer    
       self.hidden_output=input;
       #ccompute the prediction output over output layer
       o=self.logRegressionLayer.predict(input);
       self.output=o;
       
       return o;
       
   
   def classify(self,x):
        """ the method performs classificaiton by assigning each input vector x to one of defined class lables """             
        #compute the prediction probability
        output=self.predict(x);        
        #get index if class with highest probability
        indices=output.argmax(axis=1);            
        #get the output label exhibiting highest probability
        labels=map(self.lable, indices); 
        return labels;
    
   
   def save(self,file_name):
       """ the function saves the trainied model parameters to output file """           
       with open(file_name, 'wb') as output:
           pickle.dump( self.n_hidden_layers, output )
           pickle.dump(self.labels,output);
           for i in range(self.n_hidden_layers):
               hidden_layer=self.hiddenLayer[i];
               print "saving hidden layer ",i,"in file",file_name
               pickle.dump( hidden_layer, output )
        
           output_layer=self.logRegressionLayer;
           pickle.dump( output_layer, output)
    

   def load(self,file_name):
       """ the method loads the trained model parameters from output file """       
       with open(file_name, 'rb') as input:
           self.n_hidden_layers=pickle.load(input);
           self.labels=pickle.load(input);
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
           
    
 
   def train(self,train,test,validate):
        """ the main training function,that initialzes the optimizer
        and starts the training process """
        self.labels=np.unique(train[1]);            
        #initialize the optimizer        
        opti=Optimizer.Optimizer(1000,"SGD",1,200,0.13,200*0.001);    
        #set the training,testing and validation datasets
        opti.set_datasets(train,test,validate);
        #set the cinoytat
        opti.set_functions(self.logRegressionLayer.negative_log_likelihood,self.set_training_data,self.classify,self.callback,self.learn,None,None);
        opti.run();
     
1     
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
       