LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#name of the library to be build
LOCAL_MODULE    := OpenVision
#list of source files to be build as part of the library
LOCAL_SRC_FILES := ImgApp/GesturePoint.cpp ImgApp/UniStrokeGesture.cpp ImgApp/UniStrokeGestureRecognizer.cpp GestureLibraryInterface.cpp ImgUtils/ImgUtils.cpp

#list of dependent 3rd party or external libraries are included in the LOCAL_SHARED_LIBRARY variable
LOCAL_SHARED_LIBRARIES := $(foreach module,$(OPENCV_LIBS3),opencv_$(module)) 
OPENCV_MODULES3:=core imgproc flann contrib features2d video  highgui legacy ml objdetect  
OPENCV_LIBS3:=$(OPENCV_MODULES3)

#list of dependent system libraries
LOCAL_LDLIBS +=  -fPIC -llog -ldl -lm  -lz -lm -lc  -Wl,-rpath,'libs/armeabi-v7a' 
LOCAL_LDLIBS += -L$(LOCAL_PATH)/../libs/armeabi -llog -Llibs/armeabi-v7a/ -Llibs/armeabi2 $(foreach module,$(OPENCV_LIBS3),-lopencv_$(module))

#include path for header files for C and C++ applications
LOCAL_C_INCLUDES +=/usr/local/include /usr/local/include/opencv /usr/local/include/opencv2 /home/pi19404/repository/OpenVision/OpenVision
LOCAL_CPP_INCLUDES +=/usr/local/include /usr/local/include/opencv /usr/local/include/opencv2 /home/pi19404/repository/OpenVision/OpenVision
#The compilation flags for C/C++ applications
LOCAL_CPPFLAGS += -DHAVE_NEON -fPIC -DANDROID -I/usr/local/include/opencv  -I/usr/local/include   -I/OpenVision -I/home/pi19404/repository/OpenVision/OpenVision -fPIC
LOCAL_CFLAGS += -DHAVE_NEON -fPIC -DANDROID  -I/usr/local/include/opencv -I/usr/local/include -I/OpenVision -I/home/pi19404/repository/OpenVision/OpenVision -fPIC
LOCAL_CPP_FEATURES += exceptions

#statement specifies build of a shared library
include $(BUILD_SHARED_LIBRARY)

#files in the libs/armeabi are deleted during each build
#we need to have 3rd party opencv libraries in this directory
#the files are placed in the armeabi2 directory
#when ever a native build is trigged the opencv library files specified in the OPENCV_MODULES2
#variable are copied from the armeabi2 directory to the armeabi or armeabi-v7a directory
#as per the specification of APP_ABI in the Application.mk file

include $(CLEAR_VARS)
OPENCV_MODULES2:= calib3d contrib  core features2d flann highgui imgproc  legacy ml nonfree objdetect photo stitching  video videostab
OPENCV_LIBS2:=$(OPENCV_MODULES2)  
OPENCV_LIB_SUFFIX:=so
OPENCV_LIB_TYPE:=SHARED


define add_opencv_module1	
include $(CLEAR_VARS)
	LOCAL_PATH := libs/armeabi2
	LOCAL_MODULE:=aaaopencv_$1
	LOCAL_SRC_FILES:=libopencv_$1.$(OPENCV_LIB_SUFFIX)
	include $(PREBUILT_$(OPENCV_LIB_TYPE)_LIBRARY)	
endef

$(foreach module,$(OPENCV_LIBS2),$(eval $(call add_opencv_module1,$(module))))