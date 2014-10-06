# -*- coding: utf-8 -*-
"""
Created on Thu Mar 27 16:57:38 2014

@author: pi19404
"""
import numpy
import numpy as np
import theano
import theano.tensor as T
import glob
import re
import cPickle, gzip, numpy
import pickle
import pickle
from sklearn.metrics import confusion_matrix
import sklearn.metrics as met
import sys  
import csv
import cPickle
import gzip
import os
import sys
import time
from sklearn import cross_validation
import numpy
from sklearn.utils import shuffle
import theano
import theano.tensor as T
import glob
import re
import pickle
from sklearn.metrics import confusion_matrix
import sklearn.metrics as met
import sys  
import csv
rng = numpy.random

#loading the data
#num_classes=2;
#feature_length=18;
import load_datasets;





class LogisticRegression1(object):           
     def __init__(self,input,output,n_dimensions,n_classes):
        print '******************************************************'
        print 'initialialising the class LogisticRegression'
        print '******************************************************'
        self.n_in=n_dimensions;
        self.n_classes=n_classes;
        self.classes=np.array(xrange(0,self.n_classes));
        self.x=input;        
        self.y=output;
        #self.n_out=self.n_classes;
        
        self.W = theano.shared(value=numpy.zeros((self.n_in,self.n_classes),
                            dtype=theano.config.floatX), name='W' ,borrow=True)
        # initialize the baises b as a vector of n_out 0s
        self.b = theano.shared(value=numpy.zeros((self.n_classes,),
                                            dtype=theano.config.floatX), name='b' ,borrow=True)         

        # compute vector of class-membership probabilities in symbolic form
        self.logistic = T.nnet.softmax(T.dot(self.x, self.W) + self.b)
        
        
        
        # compute prediction as class whose probability is maximal in
        # symbolic form
        self.y_pred=T.argmax(self.logistic, axis=1)        
        
        #self.classify = theano.function(inputs=[self.x],
        #    outputs=self.logistic);
    
        self.params=[self.W,self.b];

        print 'Num of Input Dimensions  :' + `self.n_in`;
        print 'Num of Output Dimensions :' + `self.n_classes`;
        print '******************************************************'
        #self.batch_size=1;
        #self.cost_function(learning_rate=0.13,batch_size=1);
         
     """Function Returns a zero one loss over the input training samples"""    
     def errors(self, y):
        return T.mean(T.neq(self.y_pred, y))

     """ Function returns the mean of negative log-likelyhood of the cost function"""
     def negative_log_likelihood(self, y):
        val=-T.mean(T.log(self.logistic)[T.arange(y.shape[0]), y])
        

        return val 

     def print_params(self):
        print self.W.get_value(borrow=True).T
        print self.b.get_value(borrow=True).T

     """ The function initializes the expressions required for gradient descent
         as well as the training,validation and testing expressions"""
     def cost_function(self,learning_rate,batch_size):
         index = T.lscalar()
         index1 = T.lscalar()
         
         """ cost function"""
         cost=self.negative_log_likelihood(self.y)
         """ Gradient of cost function"""
         g_W = T.grad(cost=cost, wrt=self.W)
         g_b = T.grad(cost=cost, wrt=self.b)    

         """ Gradient update equations used by gradient descent algorithms"""
         updates = [(self.W, self.W - learning_rate * g_W),(self.b, self.b - learning_rate * g_b)]
    
    
         num_samples=classifier.train[0].get_value(borrow=True).shape[0]
         print '\n\n********************************'
         #tbatch_size=batch_size;#feature.get_value(borrow=True).shape[0];
         print 'num of training samples :' + `num_samples`
         print 'num of dimensions :' + `self.n_in`
         print 'num of classes :' + `self.n_classes`    
         print 'Training batch size :' + `batch_size`
         
         #print 'Test batch size :' + `tbatch_size`
         self.n_train_batches = self.train[0].get_value(borrow=True).shape[0] / batch_size    
         self.n_valid_batches= self.validate[0].get_value(borrow=True).shape[0] / batch_size    
         self.n_test_batches= self.test[0].get_value(borrow=True).shape[0] / batch_size    
         #print 'num of training batches :'+`self.n_train_batches`
         
         self.train[1]=T.cast(self.train[1],'int32');
         self.test[1]=T.cast(self.test[1],'int32');
         self.validate[1]=T.cast(self.validate[1],'int32');

         """ Defining functions for training,testing and validation """
         self.train_model = theano.function(inputs=[index],
            outputs=cost,
            updates=updates,
            givens={
                self.x: self.train[0][index*batch_size:(index + 1)*batch_size],
                self.y: self.train[1][index*batch_size:(index + 1)*batch_size]});
    
         self.test_model = theano.function(inputs=[index1],
            outputs=[self.errors(self.y),self.y_pred],
             givens={
                 self.x: self.test[0][index1*batch_size:(index1 + 1)*batch_size],
                 self.y: self.test[1][index1*batch_size:(index1 + 1)*batch_size]});

    
         self.validate_model = theano.function(inputs=[index],
            outputs=self.errors(self.y),
            givens={
                self.x: self.validate[0][index * batch_size:(index + 1) * batch_size],
                self.y: self.validate[1][index * batch_size:(index + 1) * batch_size]})

    
     """ Main methods which performs training"""
     def train_classifier(self,learning_rate=0.013,batch_size=10,num_iterations=1000):
         self.learning_rate=learning_rate;
         self.batch_size=batch_size;
         self.best_validation_loss = numpy.inf
         
         """ initialize the training parameters """
         self.cost_function(learning_rate,batch_size);
         
         validation_frequency=10;
         start_time = time.clock()            
         done_looping = False
         iterations = 0    
         
         """ iteration loop for gradient descent algorithms"""
         #  theano.printing.Print('this is a very important value')(x)
         while (iterations < num_iterations) and (not done_looping):             
             iterations=iterations+1;
             for minibatch_index in xrange(self.n_train_batches):
                 #print 'batch : '+`minibatch_index`+' iteration number :'+`iterations`
                 minibatch_avg_cost = self.train_model(minibatch_index)                                  
                 iter = (iterations - 1) * self.n_train_batches + minibatch_index
                 if(iter+1)%validation_frequency==0:
                     self.validation();
                 print minibatch_avg_cost
                 #print self.W.get_value(borrow=True)[1:10,1];
                 
                 
                 #print self.W.get_value(borrow=True)[:,1:10];
         end_time = time.clock()             
         
         print 'Time Taken to train model'+`(end_time - start_time)`;     
         #classifier.print1()
         print '******************* \n\n\n\n'
         

     """ Function that computes accuracy over training,testing and validation
     datasets """
     def testing(self):
            print '************************************'
            self.dataset_accuracy(self.train[0],self.train[1],'Training Accuracy ');
            self.dataset_accuracy(self.test[0],self.test[1],'Testing Accuracy ');
            self.dataset_accuracy(self.validate[0],self.validate[1],'Validation Accuracy ');
            print '************************************'
            
     """ function that compute validation dataset accuracy """            
     def validation(self):
        this_validation_loss=self.dataset_accuracy(self.validate[0],self.validate[1],'validation Accuracy ');
        if this_validation_loss < self.best_validation_loss:
            self.best_validation_loss = this_validation_loss
            self.testing()
            
     """ function computes accurcy over datasets """
     def dataset_accuracy(self,feature,lable,text):
            result=self.predict(feature.get_value(borrow=True));
            y_test=lable
            y_test=np.array(y_test.eval());            
            accuracy=met.accuracy_score(y_test,result[1])    
            accuracy=1.0-accuracy;
            print text + `accuracy`
            return accuracy;


     """ function predict the output class,given input vector x"""
     def predict(self,x):
         aa=x
         data1 = aa;
         if np.shape(np.shape(aa))[0]==1:
             data1=np.reshape(aa,(-1,np.shape(aa)[0]));
         
         self.classify = theano.function(inputs=[self.x],            
            outputs=[self.logistic,self.y_pred]);         
         
         xx,yy=self.classify(data1);                           
         return xx,yy
     
     """ function accuracy given the original and predicted lables"""
     def accuracy(self,y_test,result):
         accuracy=met.accuracy_score(y_test,result)    
         return accuracy;
         
     """ function to set the training,testing and validation datasets """
     def set_datasets(self,train,test,validate):
        self.train=train;
        self.test=test;
        self.validate=validate;
    
     """ function to save the dataset"""
     def save(self,model_name):        
        load_datasets.print_debug();
        print 'Saving the model file  ';
        output = open('LogistRegression'+model_name, 'wb')
        pickle.dump(self.W.get_value(borrow=True).T, output,-1)
        pickle.dump(self.b.get_value(borrow=True).T, output,-1)
        pickle.dump(self.n_in,output,-1);
        #pickle.dump(self.n_out,output,-1);
        pickle.dump(self.n_classes,output,-1);            
        output.close()    

     """ function to load the dataset """
     def load(self,model_name):
        load_datasets.print_debug();
        print 'Loading the model file  ';         
        pkl_file = open('LogistRegression'+model_name,'rb')
        aa=pickle.load(pkl_file)
        self.W.set_value(aa.T,borrow=True);    
        bb=pickle.load(pkl_file);
        self.b.set_value(bb.T,borrow=True);
        self.n_out=pickle.load(pkl_file);
        self.n_classes=pickle.load(pkl_file);
        self.n_out=self.n_classes;        
        self.n_in=np.shape(aa)[1]

""" Main function """   
if __name__ == '__main__':    
    
    """ symbolic expressions defining input and output vectors"""
    x=T.matrix('x');
    y=T.ivector('y');
    """ The mnist dataset in pickel format"""
    model_name1="/home/pi19404/Documents/mnist.pkl.gz"
    
    """ creating object of class Logistic regression"""
    """ input is 28*28 dimension feature vector ,and
    output lables are digits from 0-9 """
    classifier = LogisticRegression1(x,y,28*28,10);
    
    """ loading the datasets"""
    [train,test,validate]=load_datasets.load_pickle_data(model_name1);
    
    """ setting the dataset"""
    classifier.set_datasets(train,test,validate);
    #
    #classifier.init_classifier(model_name1);n_out
    """ Training the classifiers"""
    classifier.train_classifier(0.13,50000,10);
    
    """ Saving the model """
    
   
    classifier.save('1');
    #x=classifier.train[0].get_value(borrow=True)[0];
    #classifier.predict(x);
    
    """ Loading the model"""
    #classifier.load('1')
    #x=train[0].get_value(borrow=True);
    #y=train[1].eval();
    #print 'True class:'+`y`
    #xx,yy=classifier.predict(x);
    #print 'Predicted class:' + `yy`
    #classifier.testing();
    
    #print classifier.predict(classifier.train[0].get_value(borrow=True)[0])
    
    












