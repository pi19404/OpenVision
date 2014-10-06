# -*- coding: utf-8 -*-
"""
Created on Sat Sep 20 20:36:58 2014

@author: pi19404
"""

import cPickle, gzip, numpy
import pickle
import pickle
import numpy
import numpy as np
import theano
import theano.tensor as T
import glob
import re
import cPickle, gzip, numpy
import pickle
import pickle

import sys  
import csv
import cPickle
import gzip
import os
import sys
import time

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
import numpy

import theano
import theano.tensor as T
import glob
import re
import pickle
from sklearn.datasets import load_digits
from sklearn.datasets import fetch_20newsgroups
import sys  
import csv
rng = numpy.random


def size_mb(docs):
    return sum(len(s.encode('utf-8')) for s in docs) / 1e6
    
    
class LoadDataSets:
    def load(self,model_name):
        self.print_debug();
        print 'Loading the model file  ';         
        pkl_file = open('LogistRegression'+model_name,'rb')
        aa=pickle.load(pkl_file)
        self.W.set_value(aa.T,borrow=True);    
        bb=pickle.load(pkl_file);
        self.b.set_value(bb.T,borrow=True);
        self.n_out=pickle.load(pkl_file);
        self.n_classes=pickle.load(pkl_file);
        self.n_out=self.n_classes;   
    

    
    
    def load_sklearn_data(self,name):
        if name == "digits":
            training = fetch_20newsgroups(subset='train',shuffle=True,random_state=42);
            testing = fetch_20newsgroups(subset='test',shuffle=True,random_state=100);
            validation = fetch_20newsgroups(subset='test',shuffle=True,random_state=200);
            categories = training.target_names
            data_train_size_mb = size_mb(training.data)
            data_test_size_mb = size_mb(testing.data)
            data_test_size_mb = size_mb(validation.data)
            
            print("%d documents - %0.3fMB (training set)" % (
                len(training.data), data_train_size_mb))
            print("%d documents - %0.3fMB (test set)" % (
                len(testing.data), data_test_size_mb))

            print("%d documents - %0.3fMB (test set)" % (
                len(validation.data), data_test_size_mb))
                
            print("%d categories" % len(categories))
            print()        
            
            training=[training.data,training.target_names]
            testing=[testing.data,testing.target_names]
            validation=[validation.data,validation.target_names]
            
            return [training,testing,validation];
        
    def load_pickle_data(self,dataset):
             
             data_dir, data_file = os.path.split(dataset)
             if data_dir == "" and not os.path.isfile(dataset):
                  print 'data set not found: '+dataset
                  return -1;
                  
             print '... loading data  '+dataset
             # Load the dataset
             f = gzip.open(dataset, 'r')
             train_set, valid_set, test_set = cPickle.load(f)
             f.close()   
             
             train_set=self.shared_dataset(train_set);
             valid_set=self.shared_dataset(valid_set);
             test_set=self.shared_dataset(test_set);
             
             return [train_set,valid_set,test_set]; 
             
    def print_debug(self):
        print '******************************************************'      
    
    def shared_dataset(self,data, borrow=True):
            """ Function that loads the dataset into shared variables
    
            The reason we store our dataset in shared variables is to allow
            Theano to copy it into the GPU memory (when code is run on GPU).
            Since copying data into the GPU is slow, copying a minibatch everytime
            is needed (the default behaviour if the data is not in a shared
            variable) would lead to a large decrease in performance.
            """
            
            datax,datay=data;
            
            fdata=datax;
            flable=datay;
            borrow=True;
            fdata=np.array(fdata,dtype='float');
            flable=np.array(flable,dtype='float');
            fdata1=[];
            flable1=[];
            rs = cross_validation.ShuffleSplit(np.shape(fdata)[0], n_iter=10,test_size=0, random_state=0)
            for r in rs:
                #print r
                fdata1=fdata[r[0]];
                #print 
                flable1=flable[r[0]];
            
            
            shared_feature = theano.shared(numpy.asarray(fdata1,dtype=theano.config.floatX),borrow=borrow);
            shared_lable = theano.shared(numpy.asarray(flable1,dtype=theano.config.floatX),borrow=borrow);
            
            #rr=self.shared_dataset(fdata,flable);
            data=shared_feature;
            lable=shared_lable;#T.cast(shared_lable, 'int32')
            rval=[data,lable];
            return rval;        