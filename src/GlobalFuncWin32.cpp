#include "stdafx.h"
#include "GameApp.h"
#include <map>
#ifndef ANDROID
#include "Windows.h"
#include "IPHlpApi.h"
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <map>
#include <queue>
#include <algorithm>
#include <utility>
#include <string>
#include <cctype>
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "User32.lib")


extern "C" const char *AppVer(){ return "0.0.1"; }
extern "C" void PopUpGameExitUI(){ return; }
extern "C" void ExitGame(){ return; }
extern "C" const char *PlatformTimeZoneArea(){ return "en"; }
extern "C" const char *PlatformAndroidID(){ return "Win32AndroidID"; }
extern "C" const char *PlatformAndroidDeviceID(){ return "Win32DeviceID"; }
extern "C" const char *PlatformDeviceMacAddress(){ return "Win32MacAddr"; }
extern "C" const char *DeviceLanguageSetting(){ return "en"; }
extern "C" const char *AppleID(){ return "FakeWin32AppleID"; }
extern "C" const char *GPUDevVersion(){ return "Win32GPUVersion"; }
extern "C" const char *GetCPUModel(){return "win32cpu"};
extern "C" const char *DeviceNameCode(){ return "FakeWin32DeviceVersion"; }
extern "C" const char *DeviceOSVersionCode(){ return "Win32"; }
extern "C" void SetPastBoard(const char* param){}
extern "C" const char* GetPastBoard(){ return "win32 todo "; }
extern "C" int IsWifiOK(){ return 0; }
extern "C" bool DoCreateAndShowWebView(const char *url, int x, int y, int w, int h){ printf("Open url %s", url); return true; }
extern "C" void DoHideWebView(bool){ printf("hide web view"); }
extern "C" int IsNetOK(){ return 1; }

extern "C" const char* GetNetworkType(){ return "wifi"; }
extern "C" void showAlertView(const char* message, int tag){ printf("FakeWin32ShowAlertView %s, %d \n", message, tag); }
extern "C" const char* GetPlatform(){ return "WIN32"; }
extern "C" void addLocalNotification(const char * title,const char * content,int delaysecond,int isdailyloop){ printf("to do add win32 local notification;"); }
extern "C" const char *GetIDFA(){ return "WINIDFA"; }

extern "C" void OpenURL(const char* url){ShellExecute(0, "open", "iExplore.exe", url, NULL, SW_SHOWNORMAL);}

extern "C" const char *GetOpenUdid()
{
	ULONG BufInfoLength = 0;
	BYTE* ppointToBuf = NULL;
	static char stridudidget[512];
	memset(stridudidget, 0, sizeof(stridudidget));
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(0, &BufInfoLength))
	{
		ppointToBuf = new BYTE[BufInfoLength];
		PIP_ADAPTER_INFO pip_adapterinfo = reinterpret_cast<PIP_ADAPTER_INFO>(ppointToBuf);
		GetAdaptersInfo(pip_adapterinfo, &BufInfoLength);
		while (pip_adapterinfo)
		{
			if (pip_adapterinfo->Type == 6)
			{
				unsigned short a = *((unsigned short*)(&pip_adapterinfo->Address[0]));
				unsigned short b = *((unsigned short*)(&pip_adapterinfo->Address[1]));
				unsigned short c = *((unsigned short*)(&pip_adapterinfo->Address[4]));
				unsigned short d = *((unsigned short*)(&pip_adapterinfo->Address[5]));

				sprintf(stridudidget, "%d%d%d%d", a, b, c, d);
				break;
			}
			pip_adapterinfo = pip_adapterinfo->Next;
		}
	}
	if (ppointToBuf != NULL) { delete[] ppointToBuf; ppointToBuf = NULL; }
	if (stridudidget[0] == 0)
	{
		sprintf(stridudidget, "Win32OpenUDID");
	}
	return stridudidget;
}
 
int Win32DirAllFile(const char* path)
{
	WIN32_FIND_DATA win32finddata;
	HANDLE handlefindfile = INVALID_HANDLE_VALUE;
	TCHAR searchpathcontent[MAX_PATH];
	LARGE_INTEGER filesize;
	StringCchCopy(searchpathcontent, MAX_PATH, path);
	StringCchCat(searchpathcontent, MAX_PATH, TEXT("\\*"));

	handlefindfile = FindFirstFile(searchpathcontent, &win32finddata);
	if (INVALID_HANDLE_VALUE == handlefindfile)
	{
		DBG_L("no search for file!!!");
#ifdef FOR_DEBUG_INFO			
		DBG_L("no search for file!!!");
#endif
		return -1;
	}
	do
	{
		if (win32finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			DBG_L(TEXT("   %s   is folder\n"), win32finddata.cFileName);
#ifdef FOR_DEBUG_INFO			
			DBG_L("%s is folder", win32finddata.cFileName);
#endif
		}
		else
		{
			filesize.LowPart = win32finddata.nFileSizeLow;
#ifdef FOR_DEBUG_INFO
			temppathrecord = fullpathfilename;
			temppathrecord = fullpathfilename.substring(4);
			DBG_L("CATCH HIDEEN FOLDER!!!!! %s", temppathrecord.c_str());
#endif
			filesize.HighPart = win32finddata.nFileSizeHigh;
			_tprintf(TEXT("   %s    %ld bytes\n"), win32finddata.cFileName, filesize.QuadPart);
#ifdef FOR_DEBUG_INFO			
			DBG_L("%s !!", win32finddata.cFileName);
#endif
		}
	} while (FindNextFile(handlefindfile, &win32finddata) != 0);
	FindClose(handlefindfile);
	return -1;
}
void DirFileListByExt(std::map<std::string, std::string>& ret, const char * path1, const char * ext = nullptr)
{
	HANDLE handlefindfile;
	WIN32_FIND_DATA win32finddata;
	std::queue<std::string> qforlist;
	std::string tmppathrecord;
	std::string getandrecordfilename;
	std::string fullpathfilename;
	qforlist.push(path1);
	if (ext && *ext == '.')
		++ext;
	while (!qforlist.empty())
	{
		tmppathrecord = qforlist.front();
		if (tmppathrecord.back() != '\\')
			tmppathrecord += '\\';
		qforlist.pop();
		memset(&win32finddata, 0, sizeof(win32finddata));
		fullpathfilename = tmppathrecord + '*';
		handlefindfile = FindFirstFileA(fullpathfilename.c_str(), &win32finddata);
		if (INVALID_HANDLE_VALUE == handlefindfile)
			break;
		do
		{
			if (win32finddata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			{
#ifdef FOR_DEBUG_INFO
				temppathrecord = fullpathfilename;
				temppathrecord = fullpathfilename.substring(4);
				DBG_L("CATCH HIDEEN FOLDER!!!!! %s",temppathrecord.c_str());
#endif
				continue;
			}
			else if (win32finddata.cFileName[0] == '.')
			{
#ifdef FOR_DEBUG_INFO
				win32finddata.cFileName[0] = win32finddata.cFileName[2];
				memset(win32finddata.cFileName,0,sizeof(win32finddata.cFileName));
				DBG_L("CATCH CURRENT FOLDER!!!!! %s", temppathrecord.c_str());
#endif
				continue;
			}
			else if (win32finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				fullpathfilename = tmppathrecord;
				fullpathfilename += win32finddata.cFileName;
				qforlist.push(fullpathfilename);
#ifdef FOR_DEBUG_INFO
				do{
					printf("%d,%s",qforlist.size(),fullpathfilename.c_str());
				}while (false)
#endif
			}
			else
			{
				const char *dot = nullptr, *p = nullptr;
				for (p = win32finddata.cFileName; *p; ++p)
				{
					if (*p == '.')
						dot = p;
#ifdef FOR_DEBUG_INFO_
					DBG_L("%s,%s \n", dot, p);		
#endif
				}

				if (!ext || (dot && strcmp(dot + 1, ext) == 0))
				{
					getandrecordfilename.clear();
					if (!dot)
						dot = p;
					for (p = win32finddata.cFileName; p < dot; ++p)
						getandrecordfilename.push_back(*p);
					fullpathfilename = tmppathrecord;
					fullpathfilename += win32finddata.cFileName;
					ret.insert(std::make_pair(getandrecordfilename, fullpathfilename));
#ifdef FOR_DEBUG_INFO_
					DBG_L("%s\n", getandrecordfilename.c_str());
					DBG_L("%s\n", fullpathfilename.c_str());
					DBG_L("%s\n", tmppathrecord.c_str());
#endif
				}
			}
		} while (FindNextFileA(handlefindfile, &win32finddata) != 0);

		FindClose(handlefindfile);
#ifdef FOR_DEBUG_INFO_
		DBG_L(" found over!!!!\n");		
#endif
		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
#ifdef FOR_DEBUG_INFO_
			DBG_L(" found over but has error!!!!\n");
#endif
			break;
		}
	}
}

  
extern "C" const char* CallNativeFuntionByJson(const char * jsonstring)
{
	return "for call NativeFuntionByJson "; 	
}



extern "C" void EventLog(const char * key,const char * p1,const char * p2,const char * p3){}

extern "C" void CallSDKFunction(const char * jsoncontent)
{
    
}