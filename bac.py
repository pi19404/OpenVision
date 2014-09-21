# -*- coding: utf-8 -*-
"""
Created on Sun Sep 21 20:02:09 2014

@author: pi19404
"""

    def train_classifier(self,learning_rate=0.013,num_iterations=1000):
         self.learning_rate=learning_rate;
         self.best_validation_loss = numpy.inf
         
         """ initialize the training parameters """
         

         start_time = time.clock()            
         done_looping = False
         iterations = 0    
         
         """ iteration loop for gradient descent algorithms"""
         #  theano.printing.Print('this is a very important value')(x)
         while (iterations < num_iterations) and (not done_looping):             
             iterations=iterations+1;
             
               
             minibatch_avg_cost = self.negative_log_likelihood(self.params,self);
             print "AA",minibatch_avg_cost
             grads=self.gradients(self.params,self);

             self.params=self.params-learning_rate*grads;
             #print "BB",self.params[0:785]         
              #updates = [(self.W, self.W - learning_rate * g_W),(self.b, self.b - learning_rate * g_b)]
             minibatch_avg_cost = self.negative_log_likelihood(self.params,self);

             #print minibatch_avg_cost
             
             #print self.params[1:10];
                 
                 #print self.W.get_value(borrow=True)[:,1:10];
         end_time = time.clock()             
         
         print 'Time Taken to train model'+`(end_time - start_time)`;     
         #classifier.print1()
         print '******************* \n\n\n\n'
         return self.params;
         
         

__all__ = ['fmin', 'fmin_powell', 'fmin_bfgs', 'fmin_ncg', 'fmin_cg',
           'fminbound', 'brent', 'golden', 'bracket', 'rosen', 'rosen_der',
           'rosen_hess', 'rosen_hess_prod', 'brute', 'approx_fprime',
           'line_search', 'check_grad', 'OptimizeResult', 'show_options',
           'OptimizeWarning']

__docformat__ = "restructuredtext en"

import numpy
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
from numpy import (atleast_1d, eye, mgrid, argmin, zeros, shape, squeeze,
                   vectorize, asarray, sqrt, Inf, asfarray, isinf)

_epsilon = sqrt(numpy.finfo(float).eps)
# standard status messages of optimizers
_status_message = {'success': 'Optimization terminated successfully.',
                   'maxfev': 'Maximum number of function evaluations has '
                              'been exceeded.',
                   'maxiter': 'Maximum number of iterations has been '
                              'exceeded.',
                   'pr_loss': 'Desired error not necessarily achieved due '
                              'to precision loss.'}
                              

    
import LoadDataSets

def f(x, *args):
     u, v = x
     a, b, c, d, e, f = args
     return a*u**2 + b*u*v + c*v**2 + d*u + e*v + f


def gradf(x, *args):
     u, v = x
     a, b, c, d, e, f = args
     gu = 2*a*u + b*v + d     # u-component of the gradient
     gv = b*u + 2*c*v + e     # v-component of the gradient
     return np.asarray((gu, gv))         