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

import numpy as np
import matplotlib
matplotlib.rcParams['backend'] = "GTK"
matplotlib.use('GTK')
import matplotlib.pyplot as plt

from sklearn import cross_validation
from sklearn.naive_bayes import GaussianNB
from sklearn.svm import SVC
from sklearn.datasets import load_digits
from sklearn.learning_curve import learning_curve

""" this is optimizer class,that encapsulates various methods to peform gradient based
    optimization of the cost function.Presently interfaces for conjugate gradient descent and
    minibatch stochastic gradient descent are implemened.
"""    
class Optimizer(object):
    def __init__(self,maxiter=1000,method="SGD",validation_iter=1,batch_size=600,learning_rate=0.13,eta=0.9):
        self.eta=eta;
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
        self.train_errors=[];
        self.test_errors=[];
        self.validation_errors=[];
        self.iteration=[];
        
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
        self.improvement_threshold = 0.995   
        self.done_looping=False;
        self.best_iter=0;
        
        print "Total number of training samples ",np.shape(self.training[0])[0]
        print "batch sizes for training",        self.batch_size
        print "total number of training batches ",self.n_train_batches
        print "Validation freuency is ",self.validation_iter        
        
    """ passing to optimizer ,classifier functions to classify,compute gradients and callback"""
    """ gradient and init are used in cases of conjugated gradient descent from scipy package where
        direct access to gradient computation is reuired 
        in the case of SGD algorithms and complex learning algorithm like MLP's the
        learning algorithm is enapsulated in the class and learn method is 
        called for computing gradients as well as updaing the parameters        
        """
    def set_functions(self,cost,setdata,classify,callback,learn=None,gradient=None,init=None):
        self.cost=cost;
        self.setdata=setdata;
        self.classify=classify;
        self.gradient=gradient;
        self.init=init;
        self.callback=callback;
        self.learn=learn;
        
    """ function to compute error on dataset """    
    def error(self,data):
        x=data[0];
        y=data[1]        
        result=self.classify(x);        
        y_test=y;                
        accuracy=met.accuracy_score(y_test,result)    
        accuracy=1.0-accuracy;
        return accuracy*100;    
        
    def plot_error(self,error1,error2,error3):
            
            self.train_errors.append(error1)
            self.test_errors.append(error2)
            self.validation_errors.append(error3)
            self.iteration.append(self.iter)
            
            trmean=np.mean(self.train_errors);
            trstd=np.std(self.train_errors);
            tmean=np.mean(self.test_errors);
            tstd=np.std(self.test_errors);
            vmean=np.mean(self.validation_errors);
            vstd=np.std(self.validation_errors);
            #plt.fill_between(self.iteration, trmean - trstd,
            #     trmean + trstd, alpha=0.1,
            #     color="r")
            #plt.fill_between(self.iteration, tmean - tstd,
            #     tmean + tstd, alpha=0.1, color="g")    
            #plt.fill_between(self.iteration, vmean - vstd,
            #     vmean + vstd, alpha=0.1, color="g")                      
            plt.plot(self.iteration, self.train_errors, 'o-', color="r",label="Training score")
            plt.plot(self.iteration, self.test_errors, 'o-', color="g",label="Cross-validation score")            
            plt.plot(self.iteration, self.validation_errors, 'o-', color="b",label="Testing-validation score")    
            
            #plt.draw()
            plt.savefig('save.png')
        
    def local_callback(self,w=None):
            print "Iteration :",self.iter
            if w!=None:
                self.params=w;
            x=self.training[0];
            y=self.testing[0];        
            self.iter=self.iter+1;            
            self.validation_iter=10;
            if self.iter % self.validation_iter==0:        
                flag=0;
                error1=self.error(self.training);
                error2=self.error(self.testing);
                error3=self.error(self.validation);                
                
                self.plot_error(error1,error2,error3);
                self.callback(w,self.iter,x,y,flag,self.eta/self.batch_size);
                print "training accuracy is",error1, " %";
                print "testing accuracy is",error2," %";
                print "validation accuracy is",error3," %";                                       
                
                
                print "best accuracy is ",self.best_validation_loss,"at iteration",self.best_iter
                if error3 < self.best_validation_loss:
                    self.best_validation_loss=error3;
                    if error3 < self.best_validation_loss *self.improvement_threshold:
                                self.patience = max(self.patience, self.iter * self.patience_increase)
                    self.best_validation_loss = error3
                    self.best_iter = self.iter
                else:
                     self.patience = min(self.patience,self.iter+self.iter/self.patience_increase)
            
            #done looping flag presently used only for minibatch SGD algorithm
            
            #if  self.patience <= self.iter  :
                #self.done_looping = True
                #return          
            
            flag=1;            
            self.callback(w,self.iter,x,y,flag,self.eta/self.batch_size);

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
            
    def update(self,params,grads,rate=0):
        if rate==0:
            rate=self.learning_rate;
        params=params-rate*grads;
        return params;
        
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
            plt.ion()
            plt.figure()
            plt.title("Learning curves");
            plt.xlabel("Iterations")
            plt.ylabel("Error")
            plt.grid()
            plt.plot(self.iteration, self.train_errors, 'o-', color="r",label="Training score")
            plt.plot(self.iteration, self.test_errors, 'o-', color="g",label="Cross-validation score")            
            plt.plot(self.iteration, self.validation_errors, 'o-', color="b",label="Testing-validation score")              
            plt.legend(loc="best")
        
            
            
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
            if self.init!=None:
                self.params=(self.init())   
            else:
                self.params=[]
            epoch=0;
            while (epoch < self.maxiter) and (not self.done_looping):     
                 epoch=epoch+1;
                 for index in xrange(self.n_train_batches):
                     train_input=x[index * batch_size:(index + 1) * batch_size];
                     train_output=y[index * batch_size:(index + 1) * batch_size];
                     train_output=np.array(train_output,dtype=int);   
                     self.args=(train_input,train_output);    
                     self.setdata(self.args)
                     [parmas,error]=self.learn(self.update);
                     #grads=self.gradient();
                     #self.params=self.params-self.learning_rate*grads;
                     self.local_callback(parmas);
                     #self.iter = (epoch - 1) * self.n_train_batches + index
                      
                         
                 
                 