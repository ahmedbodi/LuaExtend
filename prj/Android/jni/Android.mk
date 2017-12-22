MY_DIR := $(call my-dir)
#LOCAL_PATH := $(MY_DIR)/../../../src
LOCAL_PATH := $(MY_DIR)/..
include $(CLEAR_VARS)

ROOT_DIR := $(MY_DIR)/../../../trunk
SRC_DIR := $(MY_DIR)/../../../src
LIB_PATH := $(NDK_MODULE_PATH)/../trunk/prj/Android/obj/local/$(TARGET_ARCH_ABI)
#$(warning $(LIB_PATH))
LOCAL_MODULE    := EngineDll

FILTER = %GlobalFuncWin32.cpp \
%GlobalFuncMacOsx.cpp \
%GlobalFuncLinux.cpp \
%TextInput_Win32.cpp \
%wsocket.c \
jni/../../../src/zlib/% \
jni/../../../src/lua/doc/% \
jni/../../../src/lua/etc/% \
jni/../../../src/lua/liblua/% \
jni/../../../src/lua/Lua/% \
jni/../../../src/lua/test/% \
%lua.c \
%luac.c

#%Lusdffdsns/key/key.cpp \


define walk
$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef

ALLFILES = $(call walk, $(SRC_DIR))
FILE_LIST += $(filter %.cpp %.c, $(ALLFILES))
FILE_LIST += $(SRC_DIR)/../tools/bsdiff/bsdiff.c \
			 $(SRC_DIR)/../tools/bsdiff/bsmemoryfile.c \
			 $(SRC_DIR)/../tools/bsdiff/bscommon.c \
			 $(SRC_DIR)/../tools/bsdiff/sais.c \
			 $(SRC_DIR)/../tools/bsdiff/bspatch.c 

LOCAL_SRC_FILES := $(filter-out $(FILTER), $(FILE_LIST))

#$(warning $(LOCAL_SRC_FILES))


#LOCAL_SRC_FILES += ../../a/b/c/d.cpp
#LOCAL_SRC_FILES += ../../e/f/g/d/s/f.cpp
 

#LUA_USE_PREBUILD = true


LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/../../src/Common \
				    $(LOCAL_PATH)/../../src/Common/BitOp	\
				    $(LOCAL_PATH)/../../src/Common/crc	\
				    $(LOCAL_PATH)/../../src/Common/json	\
				    $(LOCAL_PATH)/../../src/Common/yajl	\
				    $(LOCAL_PATH)/../../src/Common/yajl/api	\
				    $(LOCAL_PATH)/../../src/Common/lz4	\
				    $(LOCAL_PATH)/../../src/Common/protobuf	\
				    $(LOCAL_PATH)/../../src/Common/socket	\
				    $(LOCAL_PATH)/../../src/Common/TableSaveLoad	\
				    $(LOCAL_PATH)/../../src/Configs	\
				    $(LOCAL_PATH)/../../src/IO	\
				    $(LOCAL_PATH)/../../src/lua	\
				    $(LOCAL_PATH)/../../src/lua/src	\
				    $(LOCAL_PATH)/../../src/lua/etc	\
				    $(LOCAL_PATH)/../../src/luasocket	\
				    $(LOCAL_PATH)/../../src/LuaInterface	\
				    $(LOCAL_PATH)/../../src/sharePtr	\
				    $(LOCAL_PATH)/../../src/zlib	\
				    $(LOCAL_PATH)/../../src/zlib/include	\
				    $(LOCAL_PATH)/../../src/zlib_config	\
				    $(LOCAL_PATH)/../../src	\
				    $(LOCAL_PATH)/../../src/Android	\
				    $(LOCAL_PATH)/../../tools/bsdiff	\
				    $(LOCAL_PATH)/

LOCAL_CFLAGS := -DOS_ANDROID -D__ANDROID__ -DANDROID -D__STDC_INT64__ -DAndroid_CHECK_MEM -DANDROID_BACK_EVENT -DLUA_CUSTOM_FILE_SYSTEM
ifdef LOCAL_DEBUG
LOCAL_CFLAGS += -D_DEBUG
endif


#LOCAL_LDLIBS += -L$(LIB_PATH) -llua
LOCAL_LDLIBS += -L$(LIB_PATH) -lzlib
LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -landroid



LOCAL_STATIC_LIBRARIES := \
						  zlib \

#						  lua

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,src/lua/Android)
$(call import-module,src/zlib/Android)

