# -*- coding: utf-8 -*-
"""
Created on Wed Oct  1 17:34:57 2014

@author: pi19404
"""
from sklearn.metrics import confusion_matrix
import sklearn.metrics as met
import numpy as np
import math
from scipy import optimize
""" this is optimizer class,that encapsulates various methods to peform gradient based
    optimization of the cost function.Presently interfaces for conjugate gradient descent and
    minibatch stochastic gradient descent are implemened.
"""    
class Optimizer(object):
    def __init__(self,maxiter=1000,method="SGD",validation_iter=1,batch_size=600,learning_rate=0.13):
        self.maxiter=maxiter;
        self.iter=0;        
        self.training=[];
        self.testing=[];
        self.validation=[];
        self.method=method;
        #learning rate parameter used for stochastic gradient descent
        self.learning_rate=learning_rate;
        #batchsize parameter used for stochastic gradient descent
        self.batch_size=batch_size;
        
        #some initializations for optimizers
        self.validation_iter=validation_iter;
   
        
    """ function to set the optimzation method """
    def set_method(self,method):
        self.method=method;

    """ function to reset the iteration count """
    def reset(self):
        self.iter=0;           
            
    """function to set the training ,testing and validation databsets"""
    def set_datasets(self,training,testing,validation):
        self.training=training;
        self.testing=testing;
        self.validation=validation;
        
        #some optimization parameters adjusted based on data
        self.patience = 10000
        self.n_train_batches=np.shape(self.training[0])[0]/self.batch_size;   
        

        self.validation_iter = min(self.n_train_batches, self.patience / 2)
        self.best_validation_loss = np.inf
        
        self.patience_increase = 2
        self.improvement_threshold = 0.9   
        self.done_looping=False;
        self.best_iter=0;
        
        print "Total number of training samples ",np.shape(self.training[0])[0]
        print "batch sizes for training",        self.batch_size
        print "total number of training batches ",self.n_train_batches
        print "Validation freuency is ",self.validation_iter        
        
    """ passing to optimizer ,classifier functions to classify,compute gradients and callback"""
    def set_functions(self,cost,setdata,classify,gradient,init,callback):
        self.cost=cost;
        self.setdata=setdata;
        self.classify=classify;
        self.gradient=gradient;
        self.init=init;
        self.callback=callback;
        
    """ function to compute error on dataset """    
    def error(self,data):
        x=data[0];
        y=data[1]        
        result=self.classify(x);        
        y_test=y;                
        accuracy=met.accuracy_score(y_test,result)    
        accuracy=1.0-accuracy;
        return accuracy*100;    
        
    def local_callback(self,w):
            print "Iteration :",self.iter
            self.params=w;
            x=self.training[0];
            y=self.testing[0];        
            self.iter=self.iter+1;            
            
            if self.iter % self.validation_iter==0:        
                flag=0;
                self.callback(w,self.iter,x,y,flag);
                error1=self.error(self.training);
                error2=self.error(self.testing);
                error3=self.error(self.validation);
                print "training accuracy is",error1, " %";
                print "testing accuracy is",error2," %";
                print "validation accuracy is",error3," %";                                       
                
                
                print "best accuracy is ",self.best_validation_loss,"at iteration",self.best_iter
                if error3 < self.best_validation_loss:
                    self.best_validation_loss=error3;
                    if error3 < self.best_validation_loss *self.improvement_threshold:
                                self.patience = max(self.patience, iter * self.patience_increase)
                    self.best_validation_loss = error3
                    self.best_iter = self.iter
            
            #done looping flag presently used only for minibatch SGD algorithm
            if self.patience <= self.iter:
                self.done_looping = True
                return          
            
            flag=1;
            self.callback(w,self.iter,x,y,flag);

            """ get the training data batch for stochastic gradient optimization """
            x=self.training[0];
            y=self.training[1];
            train_input=x[self.iter * self.batch_size:(self.iter + 1) * self.batch_size];
            train_output=y[self.iter * self.batch_size:(self.iter + 1) * self.batch_size];
            train_output=np.array(train_output,dtype=int);            
            self.args=(train_input,train_output);            
            args=self.args;
            self.setdata(args);
            return;
            
        
    def run(self):
        if self.method == "CGD":
            self.iter=0;
            index=self.iter;
            batch_size=self.batch_size;            
            x=self.training[0];
            y=self.training[1];
            train_input=x[index * batch_size:(index + 1) * batch_size];
            train_output=y[index * batch_size:(index + 1) * batch_size];
            train_output=np.array(train_output,dtype=int);            
            self.args=(train_input,train_output);            
            args=self.args;
            self.setdata(args);
            self.params=self.init();
            print "**************************************"
            print "starting with the optimization process"
            print "**************************************"
            print "Executing nonlinear conjugate gradient descent optimization routines ...."
            res=optimize.fmin_cg(self.cost,self.params,fprime=self.gradient,maxiter=self.maxiter,callback=self.local_callback);
            print "**************************************"
            print "completed with the optimization process"
            print "**************************************"            
            
        if self.method=="SGD":
            self.iter=0;
            index=self.iter;
            batch_size=self.batch_size;            
            x=self.training[0];
            y=self.training[1];
            train_input=x[index * batch_size:(index + 1) * batch_size];
            train_output=y[index * batch_size:(index + 1) * batch_size];
            train_output=np.array(train_output,dtype=int);            
            self.args=(train_input,train_output);            
            args=self.args;
            self.setdata(args);
            self.params=(self.init())                 
            epoch=0;
            while (epoch < self.maxiter) and (not self.done_looping):     
                 epoch=epoch+1;
                 for index in xrange(self.n_train_batches):
                     train_input=x[index * batch_size:(index + 1) * batch_size];
                     train_output=y[index * batch_size:(index + 1) * batch_size];
                     train_output=np.array(train_output,dtype=int);   
                     self.args=(train_input,train_output);    
                     self.setdata(self.args)
                     grads=self.gradient();
                     self.params=self.params-self.learning_rate*grads;
                     self.local_callback(self.params);
                     #self.iter = (epoch - 1) * self.n_train_batches + index
                      
                         
                 
                 