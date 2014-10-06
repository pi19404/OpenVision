
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



""" This class implements methods for stochastic gradient optimization """
class SGD(object):
    def __init__(self,learning_rate,batch_size,maxiter):
        self.learning_rate=learning_rate;
        self.batch_size=batch_size;
        self.maxiter=maxiter;
        self.iter=0;
        
        self.training=[];
        self.testing=[];
        self.validation=[];


    def set_datasets(self,training,testing,validation):
        self.training=training;
        self.testing=testing;
        self.validation=validation;
        
        
    def set_functions(self,classify,gradient,init,callback):
        self.classify=classify;
        self.gradient=gradient;
        self.init=init;
        self.callback=callback;
        
        
    def error(self,data):
        x=data[0];
        y=data[1]        
        result=self.classify(x);        
        y_test=y;                
        accuracy=met.accuracy_score(y_test,result)    
        accuracy=1.0-accuracy;
        return accuracy*100;    
        
    def loop(self,params,grad,input):
        self.iter=self.iter+1;
        if(self.iter >= self.maxiter):
            return -1;
        self.learn();
        return self.params;
        
    def reset(self):
        self.iter=0;
        
    
    def run(self):
        x=self.training[0];
        y=self.training[1]
        self.params=self.init();
        batch_size=self.batch_size;
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
        flag=1;
        #number of loops for backpropagation algorithm
        while (epoch < n_epochs) and (not done_looping):
            epoch=epoch+1;

            print "Starting with loop iteration",epoch
 
        
            
            
            #stochastic gradient descent algorithm
            for index in xrange(n_train_batches):
                
                iter = (epoch - 1) * n_train_batches + index
                #print ".\r" #"starting iteration",iter
                
                if (iter + 1) % validation_frequency == 0:          
                        error1=self.error(self.training);
                        error2=self.error(self.testing);
                        error3=self.error(self.validation);
                        print "training accuracy is",error1, " %";
                        print "testing accuracy is",error2," %";
                        print "validation accuracy is",error3," %";                         
                        flag=0;
                        
                        
                        print "best accuracy is ",best_validation_loss,"at iteration",best_iter
                        #L=self.logRegressionLayer.negative_log_likelihood(self.logRegressionLayer.params);
                        #print "the data likelyhood is ",L
                        end_time = time.clock()
                        print "time taken for ",validation_frequency," iterations ",(end_time-start_time),"sec"
                        start_time=end_time;
                        
                        if error3 < best_validation_loss:
                            if error3 < best_validation_loss *improvement_threshold:
                                patience = max(patience, iter * patience_increase)
                            best_validation_loss = error3
                            best_iter = iter
                        
                if patience <= iter:
                    done_looping = True
                    break
                
                """ get the training data batch for stochastic gradient optimization """
                train_input=x[index * batch_size:(index + 1) * batch_size];
                train_output=y[index * batch_size:(index + 1) * batch_size];
                train_output=np.array(train_output,dtype=int);
                grads=self.gradient(self.params,train_input,train_output);
                self.params=self.learn(self.params,grads);
                self.callback(self.params,iter,x,y,flag);
                if flag==0:
                    flag=1;
        
    
    def learn(self,params,grads):
        params=params-self.learning_rate*grads;
        return params;
        
