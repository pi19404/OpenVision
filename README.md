OpenVision
==========

OpenVisionLibrary : Version 0.0

A Open source platform for developement and learning of image processing and computer vision algorithms.
Focus will be to develope modular,object oriented approach and scalable architecture for computer vision application.

It is an attempt at a opensource C/C++ Library developed in C/C++ using OpenCV,Eigen etc  providing modular interface
for image processing,computer vision,machine learning applications.

In the initial version a high level interface to opencv libraries will be developed and with time the opencv
interface will be replaced with sutiable alternatives.

OpenCV has a large repository but most of code is difficult to interpret due to lack of comments and no implementation detail at all. This platform will focus on documentation of algorithms and implementation details so that 
it can also serve as a good learning platform.

The code consists of 16841 lines of code as of 1st March 2014


Following Models are currently present in OpenVisionLibrary

* Feature Detectors :
Fast,Harris Corner,Good Feature to Track
Fast3d,Harris3D

* SubPixel Corner Localization

* Feature Descriptors
  HOG - Histogram of oriented gradients ,LBP -Local Binary Pattern,Random Ferns

* Temporal Filters

* Integral Images


* Contrast Stretching,Tan and Triggs Illumination normalization,Gamma Correction,Color Constancy algorithms :
  gray world,shades of gray,gray edge,max edge,max RGB
* Multi Threshold Canny edge detection
* Linear Channel Filters
* Fast 2D symmetric/Asymmmetric convolution
* Line segment detector
* Histogram Computation and Backprojection
* Seeded Region growing - Stack Based Implementation
* Polynomial Image Basis Representation
* Tracking Based on Random Ferns
* Mean Shift Tracking
* Discrete Emission Time Hidden markov model for sequence Classification
* Continuous Hidden markov model for sequence Classification
* Logistic Regression Classifier
* Dense optical flow using polynomial basis


ARM Neon Optimization
* Deinterlacing and Interlacing

Topics in progress
* Visual odometry
* Sparse Optical Flow Tracking
* LibSVM API in C/C++
* ARM Neon : BGR2GRAY
* Circle Detection using RANSAC
* Rotationally Invariant object detection Framwork 
* Mean flow tracker
* Mean Shift Tracking using histogram similarity
* Maximum Entropy Markov Models for Sequence classification
* K Means Clustering Algorithm
* Gaussian Mixture Models and EM Algorithm
* HMM Training Using Vitrebi training
* HMM Training using Bahum Welch algorithm








some of the documentation can be found on blog : http://pi-virtualworld@blogspot.com and doc repostitory can be found at
can be found at http://scribd.com/pi19404
