#include <jni.h>
#include "AndroidCommon.h"
static JavaVM *g_vm = NULL;
static JNIEnv *g_env = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    g_vm=vm;
    JNIEnv* env;
    jint result = -1;
    if(g_vm){
        LOGD("g_vm init success \n");
    }else{
        LOGD("g_vm init failed \n");
    }
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_2) != JNI_OK){
    	LOGE("ERROR:GetEnv failed \n");
        return result;
    }
    g_vm->AttachCurrentThread(&env, NULL);
    return JNI_VERSION_1_2;
} 

//	JNIEnv* env;
//	sVM->GetEnv((void**)&env, JNI_VERSION_1_2);


JNIEnv* AndroidGetJavaEnv(void) {
	if (g_env != NULL) {
		return g_env;
	}
	JavaVM* jVM = g_vm;
	if (jVM == NULL) {
		LOGE("AndroidGetJavaEnv:jvm is null");
		return NULL;
	}

	jint ret = jVM->GetEnv((void**) &g_env, JNI_VERSION_1_2);
	switch (ret) {
	case JNI_OK:
		return g_env;

	case JNI_EDETACHED:
		if (jVM->AttachCurrentThread(&g_env, NULL) < 0) {
			LOGE("AndroidGetJavaEnv:AttachCurrentThread failed");
			return NULL;
		} else {
			return g_env;
		}

	case JNI_EVERSION:
			LOGE("AndroidGetJavaEnv:JNI_EVERSION");
		return NULL;
	default:
			LOGE("AndroidGetJavaEnv:default error");
		return NULL;
	}

	return g_env;
}
