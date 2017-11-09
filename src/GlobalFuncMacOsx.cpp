#include "stdafx.h"
#include "GameApp.h"
#include <map>
#include <stdio.h>
#include <map>
#include <queue>
#include <algorithm>
#include <utility>
#include <string>
#include <cctype>
#include <dirent.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_dl.h>

extern "C" const char *AppVer(){ return "0.0.1"; }
extern "C" void PopUpGameExitUI(){ return; }
extern "C" void ExitGame(){ return; }
extern "C" const char *PlatformTimeZoneArea(){ return "en"; }
extern "C" const char *PlatformAndroidID(){ return "MacOSXAndroidID"; }
extern "C" const char *PlatformAndroidDeviceID(){ return "MacOSXDeviceID"; }
extern "C" const char *PlatformDeviceMacAddress(){ return "MacOSXMacAddr"; }
extern "C" const char *DeviceLanguageSetting(){ return "en"; }
extern "C" const char *AppleID(){ return "FakeMacOSXAppleID"; }
extern "C" const char *GPUDevVersion(){ return "MacOSXGPUVersion"; }
extern "C" const char *DeviceNameCode(){ return "FakeMacOSXDeviceVersion"; }
extern "C" const char *DeviceOSVersionCode(){ return "MacOSX"; }
extern "C" void SetPastBoard(const char* param){}
extern "C" const char* GetPastBoard(){ return "win32 todo "; }
extern "C" int IsWifiOK(){ return 0; }
extern "C" bool DoCreateAndShowWebView(const char *url, int x, int y, int w, int h){ printf("Open url %s", url); return true; }
extern "C" void DoHideWebView(bool){ printf("hide web view"); }
extern "C" int IsNetOK(){ return 1; }

extern "C" const char* GetNetworkType(){ return "wifi"; }
extern "C" void showAlertView(const char* message, int tag){ printf("FakeMacOSXShowAlertView %s, %d \n", message, tag); }
extern "C" const char* GetPlatform(){ return "MacOSX"; }
extern "C" void addLocalNotification(const char * nt){ printf("to do add MacOSX local notification;"); }
extern "C" const char *GetIDFA(){ return "MACOSXIDFA"; }

extern "C" void OpenURL(const char* url){}

extern "C" const char *GetOpenUdid()
{
	static char strid[512];
	struct ifaddrs *ifap, *ifaptr;
	unsigned char *ptr;

	if (getifaddrs(&ifap) == 0) {
	  for(ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
		if (((ifaptr)->ifa_addr)->sa_family == AF_LINK   ) {
		  ptr = (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
		  if(*ptr != 0) {
			sprintf(strid, "%02x:%02x:%02x:%02x:%02x:%02x\n",
					*ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
              return strid;
		  }
		}
	  }
	  freeifaddrs(ifap);
	  return "";
	} else {
	  return "";
	}
	return strid;
}

int Win32DirAllFile(const char* path)
{
	printf("not implement this function !! :)");
	return -1;
}

void DirFileListByExt(std::map<std::string, std::string>& ret, const char * path1, const char * ext = nullptr)
{
	std::queue<std::string> q;
	std::string tmppath;
	std::string filename;
	std::string fullpathfilename;
	q.push(path1);

	if (ext && *ext == '.')
		++ext;

	while (!q.empty()) {
		tmppath = q.front();
		q.pop();

		if (tmppath.back() != '/')
			tmppath += '/';

		//fullpathfilename = tmppath + '*';
		//std::cout<<"path:" <<tmppath<<std::endl;
	
		struct dirent* ent = NULL;
		DIR *pDir;
		pDir = opendir(tmppath.c_str());
		if (pDir == NULL) {
			return;
		}

		while (NULL != (ent = readdir(pDir))) {
			if (ent->d_type == 8) {
				//file

				const char *dot = NULL, *p = NULL;
				for (p = ent->d_name; *p; ++p) {
					if (*p == '.')
						dot = p;
				}

				if (!ext || (dot && strcmp(dot + 1, ext) == 0)) {
					filename.clear();
					if (!dot)
						dot = p;
					for (p = ent->d_name; p < dot; ++p)
						filename.push_back(*p);
					fullpathfilename = tmppath;
					fullpathfilename += ent->d_name;
					//std::cout<< filename<<" "<<fullpathfilename<<std::endl;
					ret.insert(std::make_pair(filename, fullpathfilename));
				}
			} else {
				if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
					continue;
				}
				//directory
				std::string _dirName(ent->d_name);
				std::string fullDirPath = tmppath  + _dirName;
				q.push(fullDirPath);
			}
		}


	}

}


extern "C" const char* CallNativeFuntionByJson(const char * jsonstring)
{
	return "for call NativeFuntionByJson ";
	//return AndroidUtils_CallStaticAndroidJavaFunction(input);
}


extern int TrackingAssert_impl(const char*key,const char*param)
{
	return 0;
}


extern "C" void DestroyKeyboard() {
}


extern "C" void SetKeyboardContent(const char * txt) {
}
extern "C" void EventLog(const char * key,const char * p1,const char * p2,const char * p3){}