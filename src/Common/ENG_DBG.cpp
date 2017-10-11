#include "stdafx.h"

#include <fstream>
#include <stdarg.h>
#include <iostream>
#if defined(OS_ANDROID)
#include <android/log.h>
#elif defined(_WIN32)
#include <windows.h>
#endif
#include "GameApp.h"
#include "Common/ENG_DBG.h"

void MACOSOUT(const char* str);
#ifdef WIN32
int Win32DirAllFile(const char* path);
void DirFileListByExt(std::map<std::string, std::string>& ret, const char * path1, const char * ext = nullptr);
#endif



using namespace std;
namespace ENG_DBG
{
	void WriteDoutToFile(const char * p, const char * lbm, size_t ll)
	{
		FILE * f = NULL;
		f = fopen(p, "a+");
		if (f) {
			fwrite(lbm, ll, 1, f);
			fclose(f);
		}
	}
	void OutDoutToConsole(const char * lb)
	{
	#if defined(OS_ANDROID)
		__android_log_print(ANDROID_LOG_INFO, "GAMEDEBUG", "%s", lb);
	#elif defined(TARGET_OS_IPHONE)
		MACOSOUT(lb);
	#else
		std::cout << lb << std::endl;
	#endif
	}
	char * DOut_MALLOC_MEM_BLOCK(int z)
	{
		char * m = (char*)malloc(z);
		memset(m, 0, z);
		return m;
	}
	const char * Dout_GetLogTypeStr(int t)
	{
		switch (t)
		{
		case CPLUS_L:
			return "[CPP] ";
		case CPLUS_W:
			return "[CPP] ";
		case LUA_P:
			return "[LUA] ";
		case CPLUS_E:
			return "[CPP] ";
		default:
			return "[LOG] ";
		}
		return "[E]";
	}
	#define DOUT_NSIZE_OUT_BUFFER (102400)
	void DOut(int Type, const char* format, ...)
	{
		static char * mb = NULL;
		static char * lp = NULL;
		int ll = 0;
		const char * lb = NULL;
		if (g_OutLog != 1)
			return;	
		const int fl = strlen(format);		
		const char * lt = NULL;
		lt = Dout_GetLogTypeStr(Type);
#if defined (_WIN32)  || (defined MAC_OS_X) || (defined OS_LINUX)
		while (lp == NULL) {
			lp = (char*)malloc(MAX_PATH);
#if(defined MAC_OS_X) || (defined OS_LINUX)
			sprintf(lp, "%s%s", GameApp::GetInstance()->getCachePath(), "log.txt");
#else
			sprintf_s(lp, MAX_PATH, "%s%s", GameApp::GetInstance()->getCachePath(), "log.txt");
#endif
		}
#endif
		while (mb == NULL)
		{
			mb = DOut_MALLOC_MEM_BLOCK(DOUT_NSIZE_OUT_BUFFER);
		}
		if (fl > DOUT_NSIZE_OUT_BUFFER)
		{
#if defined (_WIN32)  || (defined MAC_OS_X) || (defined OS_LINUX)
			WriteDoutToFile(lp, format, fl);
#else
			OutDoutToConsole(format);
#endif
			return;
		}
		else
		{	
			va_list arg_list;
			va_start(arg_list, format);
			int ltl = strlen(lt);
			memcpy(mb, lt, ltl);
			ll = vsnprintf(mb + ltl, DOUT_NSIZE_OUT_BUFFER - ltl, format, arg_list);
			va_end(arg_list);
			if (ll <= 0 || ll >= (DOUT_NSIZE_OUT_BUFFER - ltl - 2))
			{
				return;
			}
			else
			{
				ll = ll + ltl;
				lb = mb;
				if (mb[ll - 1] != '\n')
				{
					mb[ll] = '\n';
					mb[++ll] = 0;
				}
			}
		}
#if defined (_WIN32)  || (defined MAC_OS_X) || (defined OS_LINUX)
		WriteDoutToFile(lp,lb,ll);
#else
		OutDoutToConsole(lb);
#endif
  }

int g_DevMode = -1;
int g_OutLog = 0;
char g_luaPath[1000];
char g_DebugPath[1000];
static std::string RawLuaFilePathKeyWords = "LuaPath";
static std::string DebugPathKeyWords = "DebugPath";
static std::map<std::string, std::string> g_luafpathMap;
static std::string getfpignoreext(std::string fIn)
{
	string fn(fIn);
	size_t dotp = fn.find('.');	
	if (dotp != string::npos)
	{
		return fn.substr(0, dotp);
	}
	else
	{
		return fn;
	}
}

std::string GetLuaFullPath(std::string fIn)
{
	string fullpath = "";
	string fn = getfpignoreext(fIn);	
	if (g_luafpathMap.find(fn) != g_luafpathMap.end())
	{
		return g_luafpathMap[fn];
	}
	else
	{
#if  (defined WIN32)  || (defined MAC_OS_X) ||(defined OS_LINUX)
		return fullpath + g_luaPath + fn + ".ls";
#else
		return fullpath + (GameApp::GetInstance()->getSavePath()) + g_luaPath + fn + ".ls";
#endif
	}
}


std::string GetDebugFileFullPath(std::string fIn)
{
	string empty = "";
	string fn(fIn);
#if  (defined WIN32)  || (defined MAC_OS_X) || (defined OS_LINUX)
	return empty + g_DebugPath + fn;
#else
	return empty + (GameApp::GetInstance()->getSavePath()) + g_DebugPath + fn;
#endif
}
static std::string Getfilepathofconfigfile()
{
	string fpn = "";
	fpn = fpn + (GameApp::GetInstance()->getSavePath()) + "config.txt";
	return fpn;
}
static void CheckDBGPath()
{
	if (g_DebugPath[0] == '\0' || strlen(g_DebugPath) == 0)
	{
		ENG_ASSERT(sizeof(g_DebugPath) == sizeof(g_luaPath), "ASSERT_G_LUA_PATH");
		memcpy(g_DebugPath, g_luaPath, sizeof(g_DebugPath));
	}
	ENG_DBG::DOut(ENG_DBG::CPLUS_L, "g_DevMode is :%d", g_DevMode);
}
static void setPathBufWhileInConfigfile(char * buf, int size,string value)
{
	memset(buf, 0, sizeof(size));
	value.erase(0, value.find_first_not_of(" \n\r\t"));
	value.erase(value.find_last_not_of(" \n\r\t") + 1);
	memcpy(buf, value.c_str(), value.length());
}
void InitDBGInfo()
{    
	memset(g_DebugPath, 0, sizeof(g_DebugPath));
	fstream cfgFstream;
	cfgFstream.open(Getfilepathofconfigfile().c_str());
	memset(g_luaPath, 0, sizeof(g_luaPath));
	if (!cfgFstream.is_open())
	{
		g_DevMode = 0;
		CheckDBGPath();
		return;
	}
	else
	{	
		char tmplinecontent[1000];
		while (!cfgFstream.eof())
		{
			cfgFstream.getline(tmplinecontent, 1000);
			string line(tmplinecontent);
			size_t pos = line.find('=');
			if (pos != string::npos)
			{
				string tmpKey = line.substr(0,pos);
				if (tmpKey == RawLuaFilePathKeyWords || tmpKey == (string("--") + RawLuaFilePathKeyWords))
				{
					string vaule = line.substr(pos + 1);
					setPathBufWhileInConfigfile(g_luaPath, sizeof(g_luaPath),vaule);
#if  (defined WIN32) 
					Win32DirAllFile(g_luaPath);
					DirFileListByExt(g_luafpathMap, g_luaPath, ".lua");
#endif
				}
				else if (tmpKey == DebugPathKeyWords || tmpKey == (string("--") + DebugPathKeyWords))
				{
					string vaule = line.substr(pos + 1);
					setPathBufWhileInConfigfile(g_DebugPath, sizeof(g_DebugPath), vaule);
				}
			}
		}
		g_DevMode = 1;
		CheckDBGPath();
	}
}

}

void SetDebugConfigValue(const char * key,const char * value)
{
	if (0 == strcmp(key, "enableLogPrint"))
	{
		if (value != NULL && 0 == strcmp(value, "true"))
		{
			ENG_DBG::g_OutLog = 1;
		}
		else
		{
			ENG_DBG::g_OutLog = 0;
		}
	}
}