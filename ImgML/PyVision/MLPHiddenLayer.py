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

rng = numpy.random.RandomState(200)
""" logistic function """
def sigmoid(X):
    '''Compute the sigmoid function '''
    den = 1.0 + math.e ** (-1.0 * X) 
    d = 1.0 / den 
    return d
    
    
def grad_sigmoid(X):
    s=sigmoid(X);
    return s*(1-s);
    
sigmoid_vec = np.vectorize(sigmoid)

grad_sigmoid_vec = np.vectorize(grad_sigmoid)

""" the class absracts the hidden layer in a Multi Layer perceptron feed forward neural network 
    and is essentially a collection of neurons """
     
class HiddenLayer:
    """
    Typical hidden layer of a MLP: units are fully-connected and have
    sigmoidal activation function. Weight matrix W is of shape (n_in,n_out)
    and the bias vector b is of shape (n_out,).
    :type n_in: int
    :param n_in: dimensionality of input

    :type n_out: int
    :param n_out: number of hidden units
    
    """
    def __init__(self,n_in=None,n_out=None,activation=None,W=None,b=None):
    
        if W is None:
             low=-4*numpy.sqrt(6. / (n_in + n_out));
             high=4*numpy.sqrt(6. / (n_in + n_out));
             size=(n_in, n_out);
             self.W=numpy.asarray(rng.uniform(low,high,size),dtype=float);
         
        if b is None:
              self.b = numpy.zeros((n_out,),dtype=float);
    
        
        self.n_out=n_out;
        self.n_in=n_in;
        self.activation=activation;
        self.params=numpy.zeros([n_out,n_in+1],dtype=float);
        self.params=self.params.flatten();
        self.nparam=self.n_in+1;

        param1=self.params.reshape(-1,self.nparam);
        
        param1[:,0:self.nparam-1]=self.W.T;
        param1[:,self.nparam-1]=self.b;   
        self.params=param1.flatten();
            
            
        
        
    """ function to compute the output of the hidden layer"""
    def compute(self,input):
        linout=numpy.dot(input,self.W)+self.b;     
        self.output=self.activation(linout);                 
        return self.output;


    """ the function computes the gradient of the activation function wrt input """        
    def activation_gradient(self):
        out1=np.multiply(self.output,(1-self.output));
        return out1;
        
    """ the function compues gradient of likelyhodd function wrt output"""       
    def linear_gradient(self,weights,error):   
        #print np.shape(weights),np.shape(error);
        if len(np.shape(error))==1:
            a=np.repeat(error[:,np.newaxis],np.shape(weights)[1],axis=1);            
            #print np.shape(a);
            return a
        else:
            return numpy.dot(error,weights);
                        
    """ function to compute the gradient of log likelyhood function wrt the parameters of the hidden layer """                        
    def cost_gradients(self,weights,activation,error):        
        current_grad=self.linear_gradient(weights,error)*self.activation_gradient();                
        gW=np.mean(np.multiply(current_grad,activation),axis=0);
        gb=np.mean(current_grad,axis=0);
        return [gW,gb];

    """ function to upgrade the parameters """        
    def update_gradients(self,grads,learning_rate):
        #print np.shape(grads[0]),np.shape(grads[1])
        param1=self.params.reshape(-1,self.nparam);
        self.W=param1[:,0:self.nparam-1].T;
        self.b=param1[:,self.nparam-1];
        
        self.W=self.W-learning_rate*grads[0];
        self.b=self.b-learning_rate*grads[1]               
        
        param1[:,0:self.nparam-1]=self.W.T;
        param1[:,self.nparam-1]=self.b;   
        self.params=param1.flatten();
        

    

""" Class with implements the Multi layer perceptron feed forward neural networks"""
class MLP(object):
   def __init__(self,n_in,n_hidden_layers,n_hidden_units,n_out):       
            self.n_hidden_layers=n_hidden_layers;
            self.n_hidden_units=n_hidden_units;
            self.n_in=n_in;
            self.n_out=n_out;
            if n_hidden_layers==0:
                n_hidden_units=n_in;
            self.hiddenLayer = [HiddenLayer(n_in=n_in, n_out=n_hidden_units,activation=sigmoid) for i in range(n_hidden_layers)];    
            self.logRegressionLayer = LogisticRegression.LogisticRegression(n_hidden_units,n_out);

   """ mapping functions for output label and probability """
   def lable(self,y):
      return self.labels[y];

   """ mapping functions for output label and probability """
   def probability(self,y):
       return self.temp_output[y];
   
   """ function to compute the validation score """    
   def validate(self,y_test,result):      
        accuracy=met.accuracy_score(y_test,result)    
        accuracy=1.0-accuracy;
        print "Validation error   " + `accuracy`
        return accuracy;
            
   """the function computer the output of the forward algorithm"""
   def propagate_forward(self,input):
       self.predict(input)
   
   
   """ the function computer the prediction error given the training and predicted input"""
   """ the function coputer the error probability p_i """
   def compute_error(self,output,train_output):                   
       indices=output.argmax(axis=1);
       y_predict=map(self.lable, indices);                                  
       result=((y_predict-train_output)==0);                     
       y_predict=np.array(y_predict,dtype=int);
       #print indices,y_predict,result
       """ getting the probability corresponding to the correct input """
       out1=output;       
       out2=out1[result];
       index=[range(0,np.shape(out2)[0]),y_predict[result]];    
       out2[index]=out2[index]-1;
       out1[result]=out2;       
       self.temp_output=out1;
       
       index=[range(0,np.shape(out1)[0]),y_predict];
       res=out1;
       #print "AA",np.shape(res);
       
       return [res,out1];

   """ the function that executes the backward propagation loop """            
   def propagate_backward(self,error,weights,input):                                               
                    """ performing the operation on the hidden layers """
                        
                    input1=input;
                    for i in range(self.n_hidden_layers):                        
                        """ computing the derivative of the parameters of the hidden layers"""
                        hidden_layer=self.hiddenLayer[self.n_hidden_layers-i-1];
                        hidden_layer.compute(input1);
                      
                        """ computing the gradient of likelyhood function wrt the parameters of the hidden layer """
                        #print "weights",np.shape(hidden_layer.W);
                        
                        grad=hidden_layer.cost_gradients(weights,hidden_layer.output,error);
                        """ update the parameters """
                        hidden_layer.update_gradients(grad,0.001);
                        
                        weights=hidden_layer.W;
                        self.hiddenLayer[self.n_hidden_layers-i-1]=hidden_layer;
                        error=grad[1];
                        input1=hidden_layer.output;
                    
                    
                    
   """ The primary training function that implements the backprogation algorithm """          
   def train(self,x,y):

        self.labels =np.unique(y);    

        batch_size=1000;
        n_train_batches=np.shape(x)[0]/batch_size;
        best_validation_loss = numpy.inf
        patience = 10000
        patience_increase = 2
        improvement_threshold = 0.9
        validation_frequency = min(n_train_batches, patience / 2)
        epoch=0;
        done_looping=False;
        n_epochs=1000
        best_iter=0;
        start_time = time.clock()
        #number of loops for backpropagation algorithm
        while (epoch < n_epochs) and (not done_looping):
            epoch=epoch+1;

            print "Starting with loop iteration",epoch
 
        
            
            
            #stochastic gradient descent algorithm
            for index in xrange(n_train_batches):
                
                iter = (epoch - 1) * n_train_batches + index
                #print ".\r" #"starting iteration",iter
                
                if (iter + 1) % validation_frequency == 0:
                        labels=self.classify(x);
                        self.logRegressionLayer.x=self.hidden_output;
                        self.logRegressionLayer.y=y;
                        accuracy=np.mean((y-labels)==0);
                        error=1-accuracy;
                        print "validation accuracy is",error;
                        print "best accuracy is ",best_validation_loss,"at iteration",best_iter
                        L=self.logRegressionLayer.negative_log_likelihood(self.logRegressionLayer.params);
                        print "the data likelyhood is ",L
                        end_time = time.clock()
                        print "time taken for ",validation_frequency," iterations ",(end_time-start_time),"sec"
                        start_time=end_time;
                        
                        if error < best_validation_loss:
                            if error < best_validation_loss *improvement_threshold:
                                patience = max(patience, iter * patience_increase)
                            best_validation_loss = error
                            best_iter = iter
                        
                if patience <= iter:
                    done_looping = True
                    break
                
                """ get the training data batch for stochastic gradient optimization """
                train_input=x[index * batch_size:(index + 1) * batch_size];
                train_output=y[index * batch_size:(index + 1) * batch_size];
                train_output=np.array(train_output,dtype=int);
                
                
                """ forward algorithm loop """
                self.propagate_forward(train_input)
           
                """ performing the operation on the output layer """
                activation=self.hidden_output;                   
                #training the logistic regression classifier on the batch
                self.logRegressionLayer.x=activation;
                self.logRegressionLayer.y=train_output;                                                           
                #p1=self.logRegressionLayer.gradients(self.logRegressionLayer.params);
                [error,t1]=self.compute_error(self.output,train_output); 
                #print np.shape(error),np.shape(self.output);
                #dddd
                #weights=np.ones(np.shape(self.logRegressionLayer.W),dtype=float);       
                #$current_grad=np.dot(weights,error)*self.activation_gradient(); 
                #print np.shape(error),np.shape(activation)
#                shape=(np.shape(error)[1],np.shape(activation)[1]);
#                GW = [numpy.array(shape,dtype=float) for i in range(np.shape(error)[0])];    
#                #print "AAA",np.shape(t1),np.shape(error),np.shape(activation)
#                gW=np.zeros(shape,dtype=float);
#                for i in range(len(GW)):
#                    a1=np.reshape(error[i,:].T,(1,np.shape(error)[1]));
#                    a2=np.reshape(activation[i,:],(1,np.shape(activation)[1]));
#                    #print np.shape(a1),np.shape(a2),"::::"
#                    GW[i]=np.dot(a1.T,a2);
#                    #print GW[i]
#                    
#                    gW=gW+GW[i];
#                    #print "---",np.shape(GW[i]);
#                #rr=np.multiply(error.T,activation.T);
#                gW=gW/len(GW);
#                gb=np.mean(error,axis=0);   
                #print self.logRegressionLayer.b
                #print "",np.shape(gW),np.shape(gb)
                
                """ updating the parameters of the output layer """
                grads=self.logRegressionLayer.gradients(self.logRegressionLayer.params);
                #print np.shape(self.logRegressionLayer.params);
                #grads=self.logRegressionLayer.update_params_grads(gW,gb);
                self.logRegressionLayer.params=self.logRegressionLayer.params-(0.001*grads);
                #self.logRegressionLayer.update_params(self.logRegressionLayer.params);
                #print "change in parameters",np.mean(0.01*grads)
                
                #weights=self.logRegressionLayer.W;
                """ performing operations on hidden layers """

                if self.n_hidden_layers >0:   
                    
                    weights=self.logRegressionLayer.W;
                    """ initialize the error and weights for backpropagation """                     
                    self.propagate_backward(error,weights,train_input)
    
                    
                
                

                          
   """ the function predicts the output of the MLP feed forward network given the input X """                          
   def predict(self,x):
       input=x;
       for i in range(self.n_hidden_layers):
       #    print "computing hidden layer",i
           hidden=self.hiddenLayer[i];
           input=hidden.compute(input);
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
           for i in range(self.n_hidden_layers+1):
               hidden_layer=self.hiddenLayer[i-1];
           #file_name1=file_name+"_"+i+".pkl";
               print "saving hidden layer ",i-1,"in file",file_name
               pickle.dump( hidden_layer, output )
        
           output_layer=self.logRegressionLayer;
       #file_name1=file_name+"_"+i+".pkl";
           pickle.dump( output_layer, output)
    
   """ the method loads the trained model parameters from output file """
   def load(self,file_name):
       with open('company_data.pkl', 'rb') as input:
           self.n_hidden_layers=pickle.load(input);
           self.labels=pickle.load(input);
           self.hiddenLayer = [HiddenLayer() for i in range(self.n_hidden_layers+1)]; 
       
           for i in xrange(self.n_hidden_layers+1):
               self.hiddenLayer[i-1]=pickle.load(input);
           
           self.logRegressionLayer=pickle.load(input)
           
           self.n_in=self.hiddenLayer[0].n_in;
           self.n_out=len(self.labels);
           self.n_hidden_units=self.hiddenLayer[0].n_out;
    
if __name__ == "__main__":    

     model_name1="/home/pi19404/Documents/mnist.pkl.gz"
     data=LoadDataSets.LoadDataSets();
     [train,test,validate]=data.load_pickle_data(model_name1);
     x=train[0].get_value(borrow=True);
     y=train[1].eval();

     labels =np.unique(y);        
     n_classes = len(labels);
     n_dimensions=np.shape(x)[1];
                
     classifier=MLP(n_dimensions,1,100,n_classes);
     classifier.train(x,y);
     print np.shape(x),np.shape(y);   