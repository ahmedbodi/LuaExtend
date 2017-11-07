#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <sys/types.h>
#include "Common.h"
#include "AssetsFile.h"
#include "AndroidReader.h"
#include "CEFile.h"
#include "CFStream.h"
#include "BaseStream.h"
#include "MemStream.h"
#include "CEFile.h"
#include "FileBaseStream.h"
#include "CMemToFile.h"
#include "ENG_DBG.h"
using namespace ENG_DBG;
extern AAssetManager* g_AndroidResMgr;
string getAssertPath(const char * filename)
{
	string tmp = filename;
	const char * keywords = "/app/";
	if (tmp.find(keywords) != -1)
	{
		string t = tmp.substr(tmp.rfind(keywords) + strlen(keywords));
		DBG_L("assert name:%s",t.c_str());
		return t;
	}
	else
	{
		return tmp;
	}
}
AndroidZipAsset::AndroidZipAsset(string filename):m_fn(filename)
{
	m_zF.open(filename);
	m_fn= filename;
	zGetFileList(&m_zF, m_zfL);
}
int AndroidZipAsset::findFile(const char *filename, ZipFile &file)
{
	string strfn = stripfolder(filename);

	map<string, ZipFile>::iterator iterIdx = m_zfL.find(strfn);
	if (iterIdx != m_zfL.end())
	{
		file = iterIdx->second;
		return 1;
	}
	return -1;
}
string AndroidZipAsset::stripfolder(const char * filename){
	string tmp = filename;
	if (tmp.find('/') != -1)
		return tmp.substr(tmp.rfind('/') + 1);
	else
		return tmp;
}

AndroidZipAsset::~AndroidZipAsset()
{
	m_zF.close();
} 

AndroidReader::AndroidReader()
{
	if (!g_AndroidResMgr) {
		return;
	}

	AAssetDir* ad = AAssetManager_openDir(g_AndroidResMgr, "");
	if (!ad) {
		return;
	}

	while(true) {
		const char* lp = AAssetDir_getNextFileName(ad);
		if (lp == NULL) {
			break;
		}

		if (strstr(lp, ".dat") != NULL)
		{
			AndroidZipAsset* tmp = MARC_NEW AndroidZipAsset(lp);
			if (m_zipfiles[lp] != NULL)
			{
				CHECK_DEL(m_zipfiles[lp]);
			}
			m_zipfiles[lp] = tmp;
		}
    }
    
    AAssetDir_close(ad);
}
void AndroidReader::release()
{
	for (map<string, AndroidZipAsset*>::iterator it = m_zipfiles.begin(); it != m_zipfiles.end(); ++it)
	{
		CHECK_DEL(it->second);		
	}
	m_zipfiles.clear();
}
AndroidReader::~AndroidReader()
{
	release();
}
bool AndroidReader::AndroidFileExist(const char *n)
{
	if (g_AndroidResMgr != NULL)
	{
		AAsset* a = AAssetManager_open(g_AndroidResMgr, getAssertPath(n).c_str(), 0);
		if(a == NULL)
		{				
			return false;
		}
		AAsset_close(a);
		return true;			
	}
	return false;
}
bool AndroidReader::exist(const char *filename)
{
	ZipFile zf;
	if (findFile(filename, zf) != -1)
		return true;
	return AndroidFileExist(filename);
}
size_t AndroidReader::AndroidFileLength(const char *n)
{
	if (g_AndroidResMgr != NULL)
	{		
		AAsset* a = AAssetManager_open(g_AndroidResMgr, getAssertPath(n).c_str(), 0);
		if(a != NULL)
		{	
			int l = AAsset_getLength(a);			
			AAsset_close(a);
			return l;			
		}
	}
	return 0;
}
size_t AndroidReader::fileLength(const char *filename)
{
	ZipFile file;
	if (findFile(filename, file) != -1)
		return file.fileSize;
	return AndroidFileLength(filename);	
}
string AndroidReader::stripfolder(const char * f){
	string tmp = f;
	if (tmp.find('/') != -1)
		return tmp.substr(tmp.rfind('/') + 1);
	else
		return tmp;
}
int AndroidReader::findFile(const char *f, ZipFile &file)
{
	string strfn = stripfolder(f);

	for (map<std::string, AndroidZipAsset*>::iterator it = m_zipfiles.begin(); it != m_zipfiles.end(); ++it)
	{
		map<string, ZipFile>::iterator iterIdx = it->second->m_zfL.find(strfn);
		if (iterIdx != it->second->m_zfL.end())
		{
			file = iterIdx->second;
			return 1;
		}
	}
	return -1;
}
FileBaseStreamPtr AndroidReader::open(ZipFile &fileInfo)
{
    char *fileData = NULL;
    int memBufSize = 0;

	for (map<string, AndroidZipAsset*>::iterator it = m_zipfiles.begin(); it != m_zipfiles.end(); ++it)
	{
		ZipFile tmp;
		if (it->second->findFile(fileInfo.fileName.c_str(), tmp) != -1)
		{
			if (zGetFileContent(&it->second->m_zF, fileInfo, fileData, memBufSize))
				return FileBaseStreamPtr(MARC_NEW CMemToFile(fileData, memBufSize, fileInfo.fileName.c_str()));
		}
	}
	
	return FileBaseStreamPtr();	
}
FileBaseStreamPtr AndroidReader::openAndroidFile(const char *n)
{
	if (g_AndroidResMgr != NULL)
	{
		AAsset* a = AAssetManager_open(g_AndroidResMgr, getAssertPath(n).c_str(), 0);
		if(a != NULL)
		{	
			int l = AAsset_getLength(a);
			char * c = new char[l];
			int result = AAsset_read(a, c, l);
			AAsset_close(a);
			return FileBaseStreamPtr(MARC_NEW CMemToFile(c, l, getAssertPath(n).c_str()));
		}
	}
	return FileBaseStreamPtr();
}
FileBaseStreamPtr AndroidReader::open(const char *filename)
{
	ZipFile zfile;
	if (findFile(filename, zfile) != -1)
		return open(zfile);
	return openAndroidFile(filename);
}




