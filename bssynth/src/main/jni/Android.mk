# Copyright (C) 2012-2020 bismark LLC

LOCAL_PATH := $(call my-dir)

# for libbsse.a
include $(CLEAR_VARS)
LOCAL_MODULE := prebuild-libbsse
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libbsse.a
include $(PREBUILT_STATIC_LIBRARY)

# for libbsmp.a
include $(CLEAR_VARS)
LOCAL_MODULE := prebuild-libbsmp
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libbsmpd.a
include $(PREBUILT_STATIC_LIBRARY)

# for bssynth.so
include $(CLEAR_VARS)
LOCAL_MODULE := bssynth
LOCAL_SRC_FILES := bssynth.cpp
LOCAL_CFLAGS :=
# for native audio
LOCAL_LDLIBS    += -lOpenSLES
# for logging
LOCAL_LDLIBS    += -llog
# for native asset manager
LOCAL_LDLIBS    += -landroid
LOCAL_WHOLE_STATIC_LIBRARIES := prebuild-libbsmp prebuild-libbsse
#LOCAL_STATIC_LIBRARIES := prebuild-libbsmp prebuild-libbsse
LOCAL_C_INCLUDES := ./
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY) 
