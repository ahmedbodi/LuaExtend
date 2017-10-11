#include "stdafx.h"
#include "GameApp.h"

#include "LuaInterface/LuaInterface.h"
#include "Common/TxtMgr.h"
#include "IO/CMemToFile.h"
#include "Common/md5.h"
#include <map>
#ifdef _WIN32
#define snprintf _snprintf
#endif
#include "lua.hpp"
extern "C" {
#include "lobject.h"
#include "lundump.h"
#include "lstate.h"
}
using namespace std;
namespace ENG_DBG
{
	extern int g_DevMode;
	extern char g_luaPath[1000];
	extern char g_DebugPath[1000];
	extern string GetLuaFullPath(std::string filename);
	extern std::string GetDebugFileFullPath(std::string filenameIn);
}
using namespace ENG_DBG;
#define toproto(L,i) (clvalue(L->top+(i))->l.p)

#ifdef WIN32
extern int TrackingAssert(const char * key, const char * file, int line)
{
	DBG_L("TrackingAssert key:%s, file: %s,line %d", key, file, line);
	return 0;
}
#else
extern int TrackingAssert_impl(const char*key,const char*param);
std::map<std::string, std::string> g_TrackAssert;

extern int TrackingAssert(const char * key, const char * file, int line)
{
	std::string tmpkey = key;
	std::map<std::string, std::string>::iterator iter = g_TrackAssert.find(tmpkey);
	if (iter == g_TrackAssert.end())
	{
		g_TrackAssert[tmpkey] = tmpkey;
		TrackingAssert_impl(key,file);
	}	
	return 0;
}
#endif
 
	static const char *getFCustom(lua_State *L, void *ud, size_t *size) {
		CMemToFile* fileStream = static_cast<CMemToFile*>(ud);
		const char* buffer = (const char*)fileStream->getBuffer();
		*size = fileStream->fileLength();
		return buffer;
	}
	
	extern "C" int lua_loadfile(lua_State *L, const char *filename)
	{
		//   CUS_LOG("load LUA file %s ", filename);		
		FileBaseStreamPtr file;
		if (g_DevMode == 1)
		{
			string fn(filename);
			size_t pos = fn.find('.');
			if (pos != string::npos)
			{
				string tmpKey = fn.substr(0, pos);				
				tmpKey = GetLuaFullPath(tmpKey);
				file = GET_FS()->OpenFile(tmpKey.c_str(), "r", true);
				if (file.get() && file->existFile())
				{
					extern int luaL_loadfileWin(lua_State *L, const char *filename);
					return luaL_loadfileWin(L, tmpKey.c_str());
				}
			}
		}

		int fnameindex = lua_gettop(L) + 1;	// index of filename on the stack
		lua_pushfstring(L, "@%s", filename);
		
		
		
		if (!file.get() || !file->existFile())
		{
			file = GET_FS()->OpenFile(filename);
		}

		if (!file.get() || !file->existFile())
		{
			DBG_E("Load Lua file %s failed", filename);
			lua_pushfstring(L, "cannot open %s", filename);
			lua_remove(L, fnameindex);
			return LUA_ERRFILE;
		}
		int status = lua_load(L, getFCustom, file.get(), lua_tostring(L, -1));
		lua_remove(L, fnameindex);
		return status;
	}

	extern "C" int luaB_print(lua_State *L)
	{
		int n = lua_gettop(L);  /* number of arguments */
		int i;
		lua_getglobal(L, "tostring");
		for (i = 1; i <= n; i++) {
			const char *s;
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */
			lua_call(L, 1, 1);
			s = lua_tostring(L, -1);  /* get result */
			if (s == NULL)
				return luaL_error(L, LUA_QL("tostring") " must return a string to "
				LUA_QL("print"));
			if (i>1)
			{
				DBG_L("\t");
			}
			DBG_L(s);
			lua_pop(L, 1);  /* pop result */
		}
		DBG_L("");
		fflush(stdout);
		return 0;
	}
 
 
 
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
extern "C" void c_extMD5(const char* src, std::string& outStr)
#else
extern "C" void extMD5(const char* src, std::string& outStr)
#endif
{
	outStr = md5(src);
}

extern "C" const char * extBase64(const char* src)
{
    ENG_ASSERT(0,"win extBase64");
    return NULL;
}

extern "C" const char * stringFromBase64(const char* src)
{
	ENG_ASSERT(0, "win stringFromBase64");
    return NULL;
}

extern bool GetFName(const char *filename, char *filename_out, int len)
{
	return GET_DLC()->GetFName(filename, filename_out, len);
}

extern "C" void NotifyGameRestartEnd()
{

}

extern "C" void NotifyGameRestart()
{

}



int EngLoadGameText(lua_State *L)
{
	const char* language = luaL_checklstring(L, 1, NULL);
	std::vector<std::string> sheets;
	lua_pushnil(L);
	while (lua_next(L, 2) != 0)
	{
		const char* str = lua_tostring(L, -1);
		sheets.push_back(str);
		lua_pop(L, 1);
	}

	int ret = GetStrMgr()->changeswchPk(sheets,language);

	lua_pushboolean(L, ret);

	return 1;
}

int EngGetGameText(lua_State *L)
{
	size_t len;
	const char* IdName = luaL_checklstring(L, 1, &len);
	const char* str = GetStrMgr()->gStr(IdName);
	lua_pushstring(L, str ? str : "");
	return 1;
}

int EngGetStringByLanguageAndSheet(lua_State *L)
{
	const char* retStr = NULL;

	size_t len;
	const char* IdName = luaL_checklstring(L, 1, &len);
	const char* language = luaL_checklstring(L, 2, NULL);
	const char* sheet = luaL_checklstring(L, 3, NULL);

	if (!IdName || !language || !sheet)
	{
		lua_pushstring(L, "");
		return 1;
	}

	
	std::map<std::string, char**> m_txtlst;
	// The count of strings
	std::map<std::string, short> m_list_count;
	std::map<std::string, std::map<std::string, size_t> > m_indexMap;
	std::map<std::string, size_t> map;

	m_indexMap.insert(std::pair<std::string, std::map<std::string, size_t> >(sheet, map));

	bool preloadPackSheetIndexRet = false;
	bool preloadPackSheetRet = false;

	char filename[128];
	sprintf(filename, "text/%s.idx", sheet);

	FileBaseStreamPtr fs = GET_FS()->OpenFile(filename);

	if (fs != NULL)
	{
		int index = 0;
		unsigned short numStrings;
		unsigned short strLength;
		numStrings = fs->readShort();
		char buf[512] = { 0 };
		for (size_t i = 0; i < numStrings; ++i)
		{
			strLength = fs->readShort();
			char* pChar = buf;
			fs->read(pChar, strLength);
			pChar[strLength] = '\0';
			m_indexMap[sheet].insert(std::pair<std::string, size_t>(pChar, index++));
		}
		DBG_L("Found %d text in sheet %s\n", numStrings, sheet);
		preloadPackSheetIndexRet = true;
	}

	if (preloadPackSheetIndexRet)
	{
		char			filename[100];
		sprintf(filename, "text/%s_%s.bin", sheet, language);

		FileBaseStreamPtr fs = GET_FS()->OpenFile(filename);
		if (fs->existFile())
		{
			char**	slist;
			unsigned short	numStrings;
			unsigned short	strLength;

			numStrings = fs->readShort();
			slist = MARC_NEW char*[numStrings + 1];

			for (unsigned short i = 0; i < numStrings; ++i)
			{
				strLength = fs->readShort();
				slist[i] = MARC_NEW char[strLength + 1];
				fs->read(slist[i], strLength);
				slist[i][strLength] = '\0';
			}

			slist[numStrings] = NULL;

			m_txtlst.insert(std::pair<std::string, char**>(sheet, slist));
			m_list_count.insert(std::pair<std::string, short>(sheet, numStrings));

			preloadPackSheetRet = true;
		}
		else
		{
			DBG_L("[TxtMgr] : ERROR opening %s\n", filename);
		}
	}
	else
	{
		DBG_L("[TxtMgr] : ERROR opening %s\n", filename);
	}

	// get string start.
	if (preloadPackSheetRet)
	{
		std::map<std::string, std::map<std::string, size_t> >::const_iterator itSheet = m_indexMap.find(sheet);

		if (itSheet == m_indexMap.end())
		{
			DBG_L("ERR: cannot find sheet %s\n", sheet);
			retStr = 0;
		}

		std::map<std::string, size_t>::const_iterator it = itSheet->second.find(IdName);

		if (it == itSheet->second.end())
		{
			DBG_L("ERR: cannot find symbol %s in sheet %s\n", IdName, sheet);
			retStr = 0;
		}

		size_t stringid = it->second;
		std::map<std::string, char**>::const_iterator itList = m_txtlst.find(sheet);

		if (itList == m_txtlst.end())
		{
			DBG_L("ERR: cannot find sheet %s 2\n", sheet);
			retStr = 0;
		}

		retStr = *(itList->second + stringid);

		lua_pushstring(L, retStr ? retStr : "");
	}
	else
	{
		lua_pushstring(L, "");
	}
	// get string end.

	if (preloadPackSheetIndexRet)
	{// unload
		std::map<std::string, char**>::iterator itList;
		std::map<std::string, short>::iterator itCount = m_list_count.begin();
		while (itCount != m_list_count.end())
		{
			itList = m_txtlst.find(itCount->first);
			unsigned short numStrings = itCount->second;

			if (itList != m_txtlst.end())
			{
				char** slist = itList->second;
				for (unsigned short i = 0; i < numStrings; ++i)
				{
					CHECK_DEL_ARRAY(slist[i]);
				}
				CHECK_DEL_ARRAY(slist);
			}

			itCount++;
		}

		m_txtlst.clear();
		m_list_count.clear();
		m_indexMap.clear();
	}

	return 1;
}