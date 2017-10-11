
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <sys/types.h>
#include "AssetsFile.h"
#include "ENG_DBG.h"
using namespace ENG_DBG;
AAssetManager* g_AndroidResMgr = NULL;
extern "C"
{
	char assertpathkey[512];
    JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_setAssetManager(JNIEnv * env,jclass cls,jobject assetManager)
    {        
    	DBG_L("com.harry.engine.androidutils.setAssetManager");
        g_AndroidResMgr = AAssetManager_fromJava(env, assetManager);
    }
	JNIEXPORT void JNICALL Java_com_harry_engine_AndroidUtils_setfolderkeywords(JNIEnv * env,jclass cls,jstring jPath)
    {
    	DBG_L("com.harry.engine.androidutils.setfolderkeywords");
        const char* url = env->GetStringUTFChars(jPath, NULL);
		int len = strlen(url);
		if(len > 2 && len < sizeof(assertpathkey))
		{			
			strcpy(assertpathkey,url + 1);
		}else
		{
			memset(assertpathkey,0,sizeof(assertpathkey));
		}
		env->ReleaseStringUTFChars(jPath, url);
    }
	const char* getFolderKeyWords()
	{
		if(strlen(assertpathkey) > 2)
		{
			return assertpathkey;
		}else
		{
			return "company/gamename/docfolder";
		}
	}
};
 

AndroidFile::AndroidFile():m_file(NULL)
{
}

AndroidFile::~AndroidFile()
{
	if(m_file != NULL)
	{
    	AAsset_close(m_file);
	}
	m_file = NULL;
}

bool AndroidFile::open(std::string p)
{
	if (!g_AndroidResMgr)
    {
		DBG_E("mgr is null");
        return false;
    }else 
    {
		m_file = AAssetManager_open(g_AndroidResMgr, p.c_str(), 0);
		if(m_file == NULL)
		{
			DBG_E("open file is null");
		}
    	return true;
	}
}

int AndroidFile::length()
{
    int recordthefileLength = 0;
    recordthefileLength = AAsset_getLength(m_file);
    #ifdef DEBUG_ANDROID_FILE_FOR_TEST
    DBG_E("open file is null");
    #endif
    return recordthefileLength;
}

int AndroidFile::read(void* buffer, int size)
{
	int rS = 0;
	if(m_file == NULL)
	{
		DBG_E("m_file is NULL");
		return 0;
	}	
	if(buffer == NULL)
	{
		DBG_E("buffer is NULL");
		return 0;
	}
    
    rS = AAsset_read(m_file, buffer, size);
	if (size != rS)
	{
		DBG_E("asset read warning");
		DBG_E("size not equals readsize");
	}
    return rS;
}

void AndroidFile::seek(int o, int p)
{
	if(m_file == NULL)
	{
		DBG_E("m_file is NULL");
		return;
	}
	if (AAsset_seek(m_file, o, p) == -1)
	{
		DBG_E("asset seek error");
	}
}

void AndroidFile::close()
{
    if (m_file != NULL)
    {
    	AAsset_close(m_file);
    }
    m_file = NULL;
}
