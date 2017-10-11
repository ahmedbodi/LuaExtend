#ifndef ANDROIDUTILS_H_KLAKSDFJKL_HJHJASKLDFJLLAS_LJLSDJL
#define ANDROIDUTILS_H_KLAKSDFJKL_HJHJASKLDFJLLAS_LJLSDJL
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_initJNI(JNIEnv*, jclass cls);
JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_releaseJNI(JNIEnv*, jclass cls);
JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_setPath(JNIEnv* env,jclass cls,jstring japppath,jstring jdocpath,jstring jobbpath);
char* getJniRetBuff(int size);
char * AndroidUtils_CallStaticAndroidJavaFunction(const char* jsonstr);
#ifdef __cplusplus
}
#endif
#endif