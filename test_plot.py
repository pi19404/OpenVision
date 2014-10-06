# -*- coding: utf-8 -*-
"""
Created on Thu Oct  2 01:30:00 2014

@author: pi19404
"""
import numpy as np
import matplotlib
matplotlib.rcParams['backend'] = "TkAgg"
import matplotlib.pyplot as plt

from sklearn import cross_validation
from sklearn.naive_bayes import GaussianNB
from sklearn.svm import SVC
from sklearn.datasets import load_digits
from sklearn.learning_curve import learning_curve
import matplotlib.pyplot as p 
p.ion()
p.plot(range(20), range(20))
p.draw();

print "12312312312312";
raw_input("Press enter when done...")