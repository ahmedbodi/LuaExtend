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
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h> 


extern "C" const char *AppVer(){ return "0.0.1"; }
extern "C" void PopUpGameExitUI(){ return; }
extern "C" void ExitGame(){ return; }
extern "C" const char *PlatformTimeZoneArea(){ return "en"; }
extern "C" const char *PlatformAndroidID(){ return "LinuxAndroidID"; }
extern "C" const char *PlatformAndroidDeviceID(){ return "LinuxDeviceID"; }
extern "C" const char *PlatformDeviceMacAddress(){ return "LinuxMacAddr"; }
extern "C" const char *DeviceLanguageSetting(){ return "en"; }
extern "C" const char *AppleID(){ return "FakeLinuxAppleID"; }
extern "C" const char *GPUDevVersion(){ return "LinuxGPUVersion"; }
extern "C" const char *DeviceNameCode(){ return "FakeLinuxDeviceVersion"; }
extern "C" const char *DeviceOSVersionCode(){ return "Linux"; }
extern "C" void SetPastBoard(const char* param){}
extern "C" const char* GetPastBoard(){ return "linux todo "; }
extern "C" int IsWifiOK(){ return 0; }
extern "C" bool DoCreateAndShowWebView(const char *url, int x, int y, int w, int h){ printf("Open url %s", url); return true; }
extern "C" void DoHideWebView(bool){ printf("hide web view"); }
extern "C" int IsNetOK(){ return 1; }

extern "C" const char* GetNetworkType(){ return "wifi"; }
extern "C" void showAlertView(const char* message, int tag){ printf("FakeLinuxShowAlertView %s, %d \n", message, tag); }
extern "C" const char* GetPlatform(){ return "Linux"; }
extern "C" void addLocalNotification(const char * nt){ printf("to do add Linux local notification;"); }
extern "C" const char *GetIDFA(){ return "LINUXIDFA"; }

extern "C" void OpenURL(const char* url){}

extern "C" const char *GetOpenUdid()
{
	static char strid[512];
    int sock_mac;
    struct ifreq tmp;
	sprintf(strid, "%d%d%d%d", 1, 2, 3, 666);

    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
    if( sock_mac == -1){
            printf("create socket fail ");
            return strid;
    } 

    memset(&tmp,0,sizeof(tmp));
    strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name)-1 );
    if( (ioctl( sock_mac, SIOCGIFHWADDR, &tmp)) < 0 ){
            printf("mac ioctl error ");
            return strid;
        }
    unsigned char i1=(unsigned char)tmp.ifr_hwaddr.sa_data[0];
    unsigned char i2=(unsigned char)tmp.ifr_hwaddr.sa_data[1];
    unsigned char i3=(unsigned char)tmp.ifr_hwaddr.sa_data[4];
    unsigned char i4=(unsigned char)tmp.ifr_hwaddr.sa_data[5];
	sprintf(strid, "%d%d%d%d", i1, i2, i3, i4);
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
			//被当作目录，但是执行opendir后发现又不是目录，比如软链接就会发生这样的情况
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


