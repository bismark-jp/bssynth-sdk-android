# Copyright (C) 2014 bismark 

LOCAL_PATH := $(call my-dir)

# for libbsse.a
include $(CLEAR_VARS)
LOCAL_MODULE := prebuild-libbsse
LOCAL_SRC_FILES := external/lib/$(TARGET_ARCH_ABI)/libbsse.a
include $(PREBUILT_STATIC_LIBRARY)

# for libbsmpd.a
include $(CLEAR_VARS)
LOCAL_MODULE := prebuild-libbsmpd
LOCAL_SRC_FILES := external/lib/$(TARGET_ARCH_ABI)/libbsmpd.a
include $(PREBUILT_STATIC_LIBRARY)

# for libbssynth-jni.so
include $(CLEAR_VARS)
LOCAL_MODULE := bssynth-jni
LOCAL_SRC_FILES := bssynth-jni.c
LOCAL_CFLAGS := 
# for native audio
LOCAL_LDLIBS    += -lOpenSLES
# for logging
LOCAL_LDLIBS    += -llog
# for native asset manager
LOCAL_LDLIBS    += -landroid
LOCAL_WHOLE_STATIC_LIBRARIES := prebuild-libbsmpd prebuild-libbsse
#LOCAL_STATIC_LIBRARIES := prebuild-libbsmpd prebuild-libbsse
LOCAL_C_INCLUDES := external/include
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY) 
