#ifndef ANDROIDCOMMON_H_LKLASLDJKL_LSKLSLKLSL_KDKDJFJ
#define ANDROIDCOMMON_H_LKLASLDJKL_LSKLSLKLSL_KDKDJFJ
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#ifdef __cplusplus
extern "C" {
#endif

JNIEnv* AndroidGetJavaEnv(void);
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE , __FILE__, __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , __FILE__, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , __FILE__, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , __FILE__, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , __FILE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif


#endif
