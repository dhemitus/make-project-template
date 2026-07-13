
LOCAL_PATH := $(call my-dir)

# 1. Compile Your Modular Engine Library
include $(CLEAR_VARS)
LOCAL_MODULE := engine
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../include \
                    $(LOCAL_PATH)/../../../../libs/common/include \
                    $(LOCAL_PATH)/../../../../libs/core/include \
                    $(LOCAL_PATH)/../../../../external/SDL3/include
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/../../../../libs/common/src/*.c) \
                   $(wildcard $(LOCAL_PATH)/../../../../libs/core/src/*.c)
LOCAL_LDLIBS := -llog -landroid -lm
include $(BUILD_SHARED_LIBRARY)

# 2. Compile Your Main Application Entry Point
include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../include \
                    $(LOCAL_PATH)/../../../../libs/common/include \
                    $(LOCAL_PATH)/../../../../libs/core/include \
                    $(LOCAL_PATH)/../../../../external/SDL3/include
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../app/src/main.c
LOCAL_SHARED_LIBRARIES := SDL3 engine
LOCAL_LDLIBS := -llog -landroid -lm
include $(BUILD_SHARED_LIBRARY)
