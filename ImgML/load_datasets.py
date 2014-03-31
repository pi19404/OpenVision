# -*- coding: utf-8 -*-
"""
Created on Mon Mar 31 17:01:54 2014

@author: root
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


def shared_dataset(data, borrow=True):
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
        rs = cross_validation.ShuffleSplit(np.shape(fdata)[0], n_iter=1,test_size=0, random_state=0)
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
        
def load_pickle_data(dataset):
         
         data_dir, data_file = os.path.split(dataset)
         if data_dir == "" and not os.path.isfile(dataset):
              print 'data set not found: '+dataset
              return -1;
              
         print '... loading data  '+dataset
         # Load the dataset
         f = gzip.open(dataset, 'r')
         train_set, valid_set, test_set = cPickle.load(f)
         f.close()   
         
         train_set=shared_dataset(train_set);
         valid_set=shared_dataset(valid_set);
         test_set=shared_dataset(test_set);
         
         return [train_set,valid_set,test_set]; 
         
def print_debug():
    print '******************************************************'         