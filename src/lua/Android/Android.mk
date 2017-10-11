
FILTER = %lua.c %luac.c

LOCAL_PATH := $(call my-dir)/..

include $(CLEAR_VARS)

LOCAL_CPP_FEATURES += exceptions
LOCAL_CPPFLAGS += -fexceptions

LOCAL_MODULE := lua
LOCAL_EXPORT_C_INCLUDES = $(LOCAL_PATH)/src

ifdef LUA_USE_PREBUILD
LOCAL_SRC_FILES := Android/$(TARGET_ARCH_ABI)/liblua.a

include $(PREBUILT_STATIC_LIBRARY)
else
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src

LOCAL_CFLAGS := -DLUA_CUSTOM_FILE_SYSTEM
LOCAL_CFLAGS += -DOS_ANDROID
LOCAL_CFLAGS += -D__STDC_INT64__

LOCAL_SRC_FILES := $(addprefix src/, $(filter-out $(FILTER), $(notdir $(wildcard $(LOCAL_PATH)/src/*.c) $(wildcard $(LOCAL_PATH)/src/*.cpp))))

include $(BUILD_STATIC_LIBRARY)
endif


