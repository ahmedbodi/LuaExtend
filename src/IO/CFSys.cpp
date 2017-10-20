#include "stdafx.h"
#include "CEFile.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "CFStream.h"
#include "CFSys.h"
#include "CMemToFile.h"
#include <stdio.h>
#ifndef _STAND_ALONE_PROJECT
#include "GameApp.h"
#endif
#ifdef _WIN32
#define snprintf _snprintf
#endif
#ifdef OS_ANDROID
#include <android/log.h>

extern "C" void AddObbFile(const char* p)
{
	GET_FS()->addZip(p);
}
#endif
namespace ENG_DBG
{
	extern int g_DevMode;
	extern char g_luaPath[1000];
	extern char g_DebugPath[1000];
	extern string GetLuaFullPath(std::string filename);
	extern std::string GetDebugFileFullPath(std::string filenameIn);
}
using namespace ENG_DBG;

CFSys::~CFSys(void)
{	
	release();
}
void CFSys::releaseZip()
{
	std::map<std::string, CZFRder*>::iterator iter = m_zrs.begin();
	while(iter != m_zrs.end())
	{
		CHECK_DEL(iter->second);
		++iter;
	}
	m_zrs.clear();
}
CFSys* CFSys::Inst()
{
	static CFSys i;
	return &i;
}
int CFSys::getMode(const char *m)
{
	int a = ESM::FAM_NONE;
	bool r = strstr(m, "r") != 0;
	bool w = strstr(m, "w") != 0;
	if (r && w)
	{
		a = ESM::FAM_READ | ESM::FAM_WRITE;
	}
	else if (r)
	{
		a = ESM::FAM_READ;
	}
	else if (w)
	{
		a = ESM::FAM_WRITE;
	}
	return a;
}
FileBaseStreamPtr CFSys::OpenZipFile(const char *path)
{
	for (std::map<std::string, CZFRder*>::iterator iterIdx = m_zrs.begin(); iterIdx != m_zrs.end(); ++iterIdx)
	{
		FileBaseStreamPtr file = iterIdx->second->openFile(path);
		if (file.get())
		{
			return file;
		}
	}
	return FileBaseStreamPtr();
}
FileBaseStreamPtr CFSys::OpenDirectlyFile(const char *path, int mode)
{
	CFStream *F = MARC_NEW CFStream(path, mode);
	if (F->existFile() == false)
	{
		return FileBaseStreamPtr();
	}
	else
	{
		int len = F->fileLength();
		char *buf = MARC_NEW char[len];
		F->read(buf, len);
		CMemToFile *mfs = MARC_NEW CMemToFile(buf, len, path);
		MARC_DELETE F;
		return FileBaseStreamPtr(mfs);
	}	
}
FileBaseStreamPtr CFSys::GetFileToMemFile(FileBaseStreamPtr file)
{
	if (file->existFile() == false)
	{
		return FileBaseStreamPtr();
	}
	else
	{
		int len = file->fileLength();
		char * buf = MARC_NEW char[len];
		file->read(buf,len);
		return FileBaseStreamPtr(MARC_NEW CMemToFile(buf,len,file->fname()));
	}
}
FileBaseStreamPtr CFSys::OpenFile(const char* path, const char *mode, bool absolutionpath)
{
	int accessMode = getMode(mode);
	if (absolutionpath) 
	{
		return OpenDirectlyFile(path, accessMode);
	}
    char newfn[500];
	GET_DLC()->GetFName(path, newfn, sizeof(newfn));
 
	FileBaseStreamPtr zf = OpenZipFile(path);
	if(zf.get())
		return zf;
#ifdef OS_ANDROID
	{
		FileBaseStreamPtr androidf = m_adrfR.open(path);
		if (androidf.get())
		{
			return androidf;
		}
	}
#endif
	if (strstr(newfn, ".ls"))
	{	
		FileBaseStreamPtr Fsfs(MARC_NEW CFStream(newfn, accessMode));
		return GetFileToMemFile(Fsfs);
	}
	else
	{	
		CFStream *pFileStream = MARC_NEW CFStream(newfn, accessMode);
		return FileBaseStreamPtr(pFileStream);
	}
}
int CFSys::zipFileLength(const char *path)
{
	map<string, CZFRder*>::iterator iterIdx = m_zrs.begin();
	for (; iterIdx != m_zrs.end(); ++iterIdx)
	{
		size_t size = iterIdx->second->fileLength(path);
		if (size)
		{
			return size;
		}
	}
	return 0;
}
int CFSys::fileLength(const char* path)
{
    char newfname[500];
	GET_DLC()->GetFName(path, newfname, sizeof(newfname));
	CEFile file(newfname);
	if (file.exist())
	{
		return file.length();
	}

	size_t zs = zipFileLength(path);
	if(zs)
		return zs;
	
#ifdef OS_ANDROID
	size_t tsad = m_adrfR.fileLength(path);
	if (tsad) return tsad;
#endif
	return 0;
}
bool CFSys::zipFileexist(const char *path)
{
	map<string, CZFRder*>::iterator iterIdx = m_zrs.begin();
	for (; iterIdx != m_zrs.end(); ++iterIdx)
	{
		if (iterIdx->second->exist(path)) 
			return true;
	}
	return false;
}
bool CFSys::exist(const char *path)
{
	char nfn[512];
	GET_DLC()->GetFName(path, nfn, sizeof(nfn));
	CEFile file(nfn);

	if (file.exist()) 
	{
		return true;
	}

	if(zipFileexist(path))
		return true;

#ifdef OS_ANDROID
	if (m_adrfR.exist(path)) 
		return true;
#endif
	return false;
 
}
 

void CFSys::addZip(const char *fn)
{
	char nfn[512];
	GET_DLC()->GetFName(fn, nfn, sizeof(nfn));
	CFStream *p = MARC_NEW CFStream(nfn, ESM::FAM_READ);
	FileBaseStreamPtr f = FileBaseStreamPtr(p);
	if (f->rOrw())
	{
		delZip(fn);
		CZFRder * zipReader = MARC_NEW CZFRder(f);
		m_zrs[fn] = zipReader;
	}
	else
	{
		DBG_E("zip file %s  open error!!!!\n", fn);
		return;
	}
}


void CFSys::delZip(const char *filename)
{
	std::map<std::string, CZFRder*>::iterator iter = m_zrs.find(filename);
	if (iter != m_zrs.end())
	{
		CHECK_DEL(iter->second);
		m_zrs.erase(iter);
	}
} 

extern "C" void AddZip2FS(const char* pathname)
{
	GET_FS()->addZip(pathname);
}
void DLCFileInfoMgr::reset()
{
	m_fs.clear();
}
const char* DLCFileInfoMgr::GetRelateFName(const char *f, bool &isfull)
{
	const char *a = GameApp::getInstance()->getAppPath();
	const char * ret = NULL;
	const char *c = GameApp::getInstance()->getCachePath();
	bool isF = true;
	const char *s = GameApp::getInstance()->getSavePath();
	if (strncmp(f, a, strlen(a)) == 0)
	{
		ret = f + strlen(a);		
	}
	else if (strncmp(f, c, strlen(c)) == 0)
	{
		ret = f + strlen(c);		
	}
	else if (strncmp(f, s, strlen(s)) == 0)
	{
		ret = f + strlen(s);		
	}
	else
	{
		isF = false;
		ret = f;
	}
	isfull = isF;
	return ret;
}

bool DLCFileInfoMgr::CheckSaveCacheFile(const char *r, char *o, int len)
{
	const char *c = GameApp::getInstance()->getCachePath();
	const char *s = GameApp::getInstance()->getSavePath();
	if (strstr(r, ".lua") || strstr(r, ".txt"))
	{
		char fs[1000];
		snprintf(fs, 1024, "%s%s", s, r);
		FILE *f = fopen(fs, "rb");
		if (f)
		{
			fclose(f);
			strncpy(o, fs, len);			
			return true;
		}
	}
	char fc[1000];
	snprintf(fc, 1000, "%s%s", c, r);
	FILE *f = fopen(fc, "rb");
	if (f)
	{
		strncpy(o, fc, len);
		fclose(f);
		return true;
	}
	return false;
}

bool DLCFileInfoMgr::GetFName(const char *fn, char *out, int len)
{
	const char *rfn = NULL;
	bool isFP = false;
	const char *a = GameApp::getInstance()->getAppPath();
	
	rfn = GetRelateFName(fn, isFP);
	
	if (g_DevMode == 1)
	{
		string fn(rfn);
		fn = GetDebugFileFullPath(fn);
		FileBaseStreamPtr file;
		file = GET_FS()->OpenFile(fn.c_str(), "r", true);
		if (file.get() && file->existFile())
		{
			snprintf(out, len, "%s", fn.c_str());			
			return  false;
		}
	}

	std::map<std::string, DLCEntry>::iterator itr = m_fs.find(rfn);
	std::map<std::string, DLCEntry>::iterator lastItr = m_fs.end();

	if (itr != lastItr)
	{
		if (CheckSaveCacheFile(rfn, out, len))
		{
			return true;
		}
	}
	if (isFP)
	{
		strncpy(out, fn, len);
	}
	else
	{
		snprintf(out, len, "%s%s", a, rfn);
	}
	return false;
}

void DLCFileInfoMgr::AddFile(const char* filename)
{
    DLCEntry entry;
    strncpy(entry.crc, "1", 2);
    entry.ver = 1;    
	bool isf = false;;
	const char * p = GetRelateFName(filename,isf);
	m_fs[p] = entry;
}

int DLCFileInfoMgr::UpdateFileInfo(lua_State *L)
{
	size_t lentmpforloadstring;
	const char *fn = luaL_checklstring(L, 1, &lentmpforloadstring);
	const char *crc = luaL_checklstring(L, 2, &lentmpforloadstring);
	int v = luaL_checkinteger(L, 3);
	if (v == -1)
	{
		m_fs.erase(fn);
	}
	else
	{
		DLCEntry de;
		bool isf = false;
		memset(&de, 0, sizeof(DLCEntry));
		strncpy(de.crc, crc, 16);
		const char * p = GetRelateFName(fn, isf);
		de.ver = v;
		m_fs[p] = de;
	}
	return 0;
}

void DLCFileInfoMgr::SaveItem(const char* fn, DLCEntry * de,FILE*fptr)
{
	//--SAVE KEY
	{
		const char * tstr = fn;
		int size = strlen(tstr);
		char Stype = LUA_TSTRING;
		fwrite(&Stype, 1, 1, fptr);
		fwrite(&size, 4, 1, fptr);
		fwrite(tstr, size, 1, fptr);
	}
	//--save table
	{
		char Ttype = LUA_TTABLE;
		fwrite(&Ttype, 1, 1, fptr);
		{
			const char * tstr = "version";
			int size = strlen(tstr);
			char Stype = LUA_TSTRING;
			fwrite(&Stype, 1, 1, fptr);
			fwrite(&size, 4, 1, fptr);
			fwrite(tstr, size, 1, fptr);

			double tmp = de->ver;
			char *buf = (char *)&tmp;
			char Ntype = LUA_TNUMBER;
			fwrite(&Ntype, 1, 1, fptr);
			fwrite(&buf[7], 1, 1, fptr);
			fwrite(&buf[6], 1, 1, fptr);
			fwrite(&buf[5], 1, 1, fptr);
			fwrite(&buf[4], 1, 1, fptr);
			fwrite(&buf[3], 1, 1, fptr);
			fwrite(&buf[2], 1, 1, fptr);
			fwrite(&buf[1], 1, 1, fptr);
			fwrite(&buf[0], 1, 1, fptr);
		}
		{
			{
				const char * tstr = "crc32";
				int size = strlen(tstr);
				char Stype = LUA_TSTRING;
				fwrite(&Stype, 1, 1, fptr);
				fwrite(&size, 4, 1, fptr);
				fwrite(tstr, size, 1, fptr);
			}
			{
				const char * tstr = de->crc;
				int size = strlen(tstr);
				char Stype = LUA_TSTRING;
				fwrite(&Stype, 1, 1, fptr);
				fwrite(&size, 4, 1, fptr);
				fwrite(tstr, size, 1, fptr);
			}
			char tmp = -1;
			fwrite(&tmp, 1, 1, fptr);
		}
	}
}
int DLCFileInfoMgr::SaveAllInfo(lua_State *L)
{
	size_t s;
	char tfn[1023];
	const char *fn = luaL_checklstring(L, 1, &s);
	snprintf(tfn, 1023, "%s%s", GameApp::getInstance()->getSavePath(), fn);
	FILE* fptr = fopen(tfn, "wb");
	if (fptr != NULL)
	{
		//// TODO  add DLC table
		char Ttype = LUA_TTABLE;
		fwrite(&Ttype, 1, 1, fptr);
		for (map<string, DLCEntry>::iterator iter = m_fs.begin(); iter != m_fs.end(); iter ++)
		{
			DLCEntry entry = iter->second;
			SaveItem(iter->first.c_str(),&entry,fptr);
		}		
		char tmp = -1;
		fwrite(&tmp, 1, 1, fptr);
		fclose(fptr);
	}
	else
		return 0;    
	return 0;
} 

DLCFileInfoMgr* DLCFileInfoMgr::GetInstance()
{
	static DLCFileInfoMgr fs;
	return &fs;
}
