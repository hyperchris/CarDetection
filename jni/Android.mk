    LOCAL_PATH := $(call my-dir)    
    include $(CLEAR_VARS)    
    OPENCV_LIB_TYPE:=STATIC  
    ifeq ("$(wildcard $(OPENCV_MK_PATH))","")    
    #try to load OpenCV.mk from default install location    
    include /home/chris/Dev/openCV/opencvAndroid/sdk/native/jni/OpenCV.mk   
    else    
    include $(OPENCV_MK_PATH)    
    endif    
    LOCAL_MODULE    := ImgFun    
    LOCAL_SRC_FILES := ImgFun.cpp    
    include $(BUILD_SHARED_LIBRARY)   