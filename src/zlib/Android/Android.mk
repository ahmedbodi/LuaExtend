
LOCAL_PATH:= $(call my-dir)/..

include $(CLEAR_VARS)

LOCAL_MODULE:= zlib

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDES = $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES:= src/adler32.c \
src/compress.c \
src/crc32.c \
src/deflate.c \
src/gzio.c \
src/infback.c \
src/inffast.c \
src/inflate.c \
src/inftrees.c \
src/trees.c \
src/uncompr.c \
src/zutil.c


LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -Wno-empty-body
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += -DDARWIN_NO_CARBON
LOCAL_CFLAGS += -D_LIB
LOCAL_CFLAGS += -O3

include $(BUILD_STATIC_LIBRARY)