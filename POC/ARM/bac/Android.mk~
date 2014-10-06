LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)



LOCAL_MODULE := helloneon
LOCAL_SRC_FILES := neon.cpp
LOCAL_STATIC_LIBRARIES := cpufeatures

OPENCV_MODULES3:=  core imgproc highgui flann legacy 
#calib3d contrib  core features2d flann highgui imgproc  legacy ml nonfree objdetect photo stitching  video videostab    
OPENCV_LIBS3:=$(OPENCV_MODULES3)
LOCAL_SHARED_LIBRARIES :=core calib3d contrib  core features2d flann highgui imgproc  legacy ml nonfree objdetect photo stitching  video videostab


LOCAL_LDLIBS +=  -llog -ldl -lm  -lz -lm -lc -lgcc
OPENCV_LIBS_DIR=libs/armeabi
LOCAL_LDLIBS += -L$(LOCAL_PATH)../libs/armeabi -llog -landroid -lEGL -lGLESv2 -Llibs/armebi -L../libs/armeabi $(foreach module,$(OPENCV_LIBS3),-lopencv_$(module))     
OPENVISON=/media/UBUNTU/repository/OpenVisionLibrary/OpenVision

LOCAL_C_INCLUDES +=/usr/local/include /media/UBUNTU/repository/OpenVisionLibrary/OpenVision/
LOCAL_CPPFLAGS += -fPIC -DANDROID    
LOCAL_CFLAGS += -fPIC -DANDROID 
LOCAL_CPP_FEATURES += exceptions

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS +=  -DHAVE_NEON=1 -ftree-vectorize  -mfpu=neon -O3 -mfloat-abi=softfp -ffast-math -I/OpenVision -I/media/UBUNTU/repository/OpenVisionLibrary/OpenVision -fPIC
    LOCAL_CPPFLAGS +=  -DHAVE_NEON=1 -ftree-vectorize  -mfpu=neon -O3 -mfloat-abi=softfp -ffast-math -I/OpenVision -I/media/UBUNTU/repository/OpenVisionLibrary/OpenVision -fPIC
    LOCAL_SRC_FILES += helloneon-intrinsics.c.neon
endif
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
OPENCV_MODULES2:= calib3d contrib  core features2d flann highgui imgproc  legacy ml nonfree objdetect photo stitching  video videostab
OPENCV_LIBS2:=$(OPENCV_MODULES2)  
OPENCV_LIB_SUFFIX:=so
OPENCV_LIB_TYPE:=SHARED


define add_opencv_module1	
include $(CLEAR_VARS)
	LOCAL_PATH := libs/armeabi2
	LOCAL_MODULE:=opencv_$1
	LOCAL_SRC_FILES:=libopencv_$1.$(OPENCV_LIB_SUFFIX)
	include $(PREBUILT_$(OPENCV_LIB_TYPE)_LIBRARY)	
endef



$(foreach module,$(OPENCV_LIBS2),$(eval $(call add_opencv_module1,$(module))))


$(call import-module,cpufeatures)
