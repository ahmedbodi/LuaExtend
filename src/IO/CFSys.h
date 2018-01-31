#ifndef _lslkljfjfj_eng_lskdk_f_ldkfj_h_jsdjlsfkl_
#define _lslkljfjfj_eng_lskdk_f_ldkfj_h_jsdjlsfkl_
#include "IO/FileBaseStream.h"

#include <string>
#include <map>
#ifdef OS_ANDROID
#include "AndroidReader.h"
#endif
#include "ZipReader.h"

#include "lua.hpp"
using namespace std;
struct DLCEntry
{
	char crc[32];
	int ver;	
};
class DLCFileInfoMgr
{
public :
	static DLCFileInfoMgr* GetInstance();
	void reset();
	bool GetFName(const char *fin, char *fout, int len);
	const char* GetRelateFName(const char *fin,bool &isfull);
	bool CheckSaveCacheFile(const char *fin, char *fout, int len);
	int UpdateFileInfo(lua_State *L);
	void AddFile(const char* fn);
	int SaveAllInfo(lua_State *L);
	void SaveItem(const char* fn,DLCEntry * de,FILE* fptr);
	map<string, DLCEntry> m_fs;
};
class CFSys
{
public:
	map<string, CZFRder*>     m_zrs;
	CFSys(){}
	~CFSys();
	static CFSys* Inst();
	FileBaseStreamPtr OpenFile(const char *f, const char *m = "r", bool notrelate = false);
	FileBaseStreamPtr OpenZipFile(const char *f);
	FileBaseStreamPtr OpenDirectlyFile(const char *f,int m);
	FileBaseStreamPtr GetFileToMemFile(FileBaseStreamPtr file);
	int getMode(const char *m);
	void release(){ releaseZip(); }
	void releaseZip();
	int zipFileLength(const char *path);
	int fileLength(const char *path);
	bool exist(const char *path);
	bool zipFileexist(const char *path);
	void addZip(const char *f);
	void addObbFile(const char *f);
	void delZip(const char *f);
#ifdef OS_ANDROID
	AndroidReader  m_adrfR;
#endif

};

#define GET_FS() CFSys::Inst()

#define GET_DLC() DLCFileInfoMgr::GetInstance()

#endif