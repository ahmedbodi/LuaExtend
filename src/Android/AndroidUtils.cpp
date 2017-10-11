#include <jni.h>
#include <android/log.h>
#include "AndroidCommon.h"
#include "AndroidUtils.h"

jclass c_AndroidUtils;
//jobject o_AndroidUtils; all function is static so need not use this object
jmethodID m_CallStaticAndroidJavaFunction = NULL;

struct jniret_buffs
{
	int size;
	char * buf;
};

#define JNIRET_BUFF_COUNT  (20)
struct jniret_buffs jniretbuffs[JNIRET_BUFF_COUNT];
int index_for_nextjniretbuff = 0;
static void Init_jniret_buffs()
{
	int i = 0;
	for(;i < JNIRET_BUFF_COUNT; i ++)
		memset((void*)&(jniretbuffs[i]),0,sizeof(jniretbuffs[i]));
}
char* getJniRetBuff(int size)
{
	int index = index_for_nextjniretbuff;
	index_for_nextjniretbuff ++;
	index_for_nextjniretbuff = index_for_nextjniretbuff%JNIRET_BUFF_COUNT;
	struct jniret_buffs * b = &(jniretbuffs[index]);
	if(b->size < size)
	{
		free((void*)b->buf);
		b->buf = NULL;
		b->size = 0;
	}
	if(b->size == 0)
	{
		b->buf = NULL;
		b->buf = (char*)malloc(size);
		b->size = size;
	}
	if(b->buf == NULL)
	{
		int * a = (int*)b->buf;
		*a = 0xbaddead;
	}
	memset((void*)b->buf,0,b->size);
	return 	b->buf;
}

JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_initJNI(JNIEnv* env,jclass cls)
{

	LOGI("JNI call function by java %s \n","initJNI");
	if(env == NULL)
	{
		return ;
	}
	Init_jniret_buffs();
	if(c_AndroidUtils == NULL)
	{
		c_AndroidUtils = (jclass) env->NewGlobalRef(cls);
		if(c_AndroidUtils == NULL)
		{
			LOGE("Can not found java class com.harry.engine.AndroidUtils");
			return ;
		}
	}
	//--------------
	if(m_CallStaticAndroidJavaFunction != NULL){ LOGE("Error : %s is NOT NULL while initJNI \n" , "m_CallStaticAndroidJavaFunction"); return;}
	m_CallStaticAndroidJavaFunction	=env->GetStaticMethodID(c_AndroidUtils,"CallStaticAndroidJavaFunction","(Ljava/lang/String;)Ljava/lang/String;");
	if(m_CallStaticAndroidJavaFunction == NULL){LOGE("Error: can not found function %s in java \n","CallStaticAndroidJavaFunction");return;}
}
JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_releaseJNI(JNIEnv* env,jclass cls)
{
	LOGI("JNI call function by java %s \n","releaseJNI");
	env->DeleteGlobalRef(c_AndroidUtils);
	c_AndroidUtils = NULL;
	delete m_CallStaticAndroidJavaFunction;
	m_CallStaticAndroidJavaFunction = NULL;
}
extern "C" void GameApp_setAppPath(const char*path);
extern "C" void GameApp_setCachePath(const char*path);
extern "C" void GameApp_setSavePath(const char * path);
extern "C" void GameApp_setObbBundlePath(const char*path);

JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_setPath(JNIEnv* env,jclass cls,jstring japppath,jstring jdocpath,jstring jobbpath)
{
	const char *resultapp = env->GetStringUTFChars(japppath, NULL);
	const char *resultdoc = env->GetStringUTFChars(jdocpath, NULL);
	const char *resultobb = env->GetStringUTFChars(jobbpath, NULL);
	if(resultapp != NULL) 
	{
		GameApp_setAppPath(resultapp);
	}
	if(resultdoc != NULL)
	{
		GameApp_setSavePath(resultdoc);
		GameApp_setCachePath(resultdoc);
	}
	if(resultobb != NULL && strlen(resultobb) > 2) 
	{
		GameApp_setObbBundlePath(resultapp);
	}
	env->ReleaseStringUTFChars(japppath, resultapp);
	env->ReleaseStringUTFChars(jdocpath, resultdoc);
	env->ReleaseStringUTFChars(jobbpath, resultobb);
}

#ifdef __cplusplus
extern "C" {
#endif

char * AndroidUtils_CallStaticAndroidJavaFunction(const char* jsonstr)
{
	//getJniRetBuff
	JNIEnv* env = AndroidGetJavaEnv();
	jstring jParam = env->NewStringUTF(jsonstr);
	jstring jResult = (jstring)env->CallStaticObjectMethod(c_AndroidUtils, m_CallStaticAndroidJavaFunction, jParam);
	char * buf = NULL;
	if(jResult)
	{
		const char *resultC = env->GetStringUTFChars(jResult, NULL);
		buf = getJniRetBuff(strlen(resultC) + 2);
		strcpy(buf,resultC);	
		env->ReleaseStringUTFChars(jResult, resultC);
		env->DeleteLocalRef(jResult);
	}
	env->DeleteLocalRef(jParam);
	return buf;
}

#ifdef __cplusplus
}
#endif