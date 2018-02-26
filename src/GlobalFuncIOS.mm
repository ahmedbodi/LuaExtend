#include "stdafx.h"
#include "GameApp.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/utsname.h>
#import <AdSupport/ASIdentifierManager.h>
#import "IOS/Reachability.h"
#import "IOS/UICKeyChainStore.h"
#import <AdSupport/ASIdentifierManager.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#define kKeychainglgshareIdentifier          @"kKeychainglgsharechIdentifier"
#define kKeychainglgshareService             @"com.glgshare.service"
static NSString* getUUIDFromKeychain()
{
    NSString *uuid = [UICKeyChainStore stringForKey:kKeychainglgshareIdentifier service:kKeychainglgshareService];
    if(!uuid){
        CFUUIDRef uuidRef = CFUUIDCreate(kCFAllocatorDefault);
        CFStringRef cfstring = CFUUIDCreateString(kCFAllocatorDefault, uuidRef);
        const char *cStr = CFStringGetCStringPtr(cfstring,CFStringGetFastestEncoding(cfstring));
        uuid = [NSString stringWithUTF8String:cStr];
        CFRelease(uuidRef);
        CFRelease(cfstring);

        UICKeyChainStore *store = [UICKeyChainStore keyChainStoreWithService:kKeychainglgshareService];
        [store setString:uuid forKey:kKeychainglgshareIdentifier];
        [store synchronize];
    }
    /// for test only, can be removed
    NSLog(@"getUUIDFromKeychain open_udid=%@",uuid);
    return uuid;
}
static bool setUUIDToKeychain(const char * Writeuudid)
{
    NSString *uuid = [UICKeyChainStore stringForKey:kKeychainglgshareIdentifier service:kKeychainglgshareService];
    if(uuid){
        NSString * newUudid = [NSString stringWithUTF8String:Writeuudid];
        if([uuid compare:newUudid] != NSOrderedSame)
        {
            UICKeyChainStore *store = [UICKeyChainStore keyChainStoreWithService:kKeychainglgshareService];
            [store setString:newUudid forKey:kKeychainglgshareIdentifier];
            [store synchronize];
            return true;
        }
    }
    return false;
}
extern "C" const char *GetBundleIdentifier()
{
    NSString *identifier = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString *)kCFBundleIdentifierKey];
    return [identifier UTF8String];
}

extern "C" const char *AppVer()
{
    NSString *version = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    return [version UTF8String];
}




extern "C" const char *GetGameName()
{
    NSString *name = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString*)kCFBundleNameKey];
    return [name UTF8String];
}

extern "C" const char* GetIDFA()
{
    NSUUID* idfa = [[ASIdentifierManager sharedManager] advertisingIdentifier];
    return [[idfa UUIDString] UTF8String];
}

extern "C" int GetPixyMovedToDrag()
{
    return 2;
}

#define PUSH_ALERT_TAG 998
#define MSG_ALERT_BASE_TAG 1000

 

char s_deviceToken[128] = {0,};

extern "C" const char* GetDeviceToken()
{
   
    return s_deviceToken;
}

extern "C" const char *GetIOSMacAddress()
{
    return "";
}

void MACOSOUT(const char* str)
{
    NSLog([NSString stringWithCString:str encoding:NSASCIIStringEncoding]);
}
extern "C" const char* GetPlatform(){ return "IOS"; }

extern "C" const char *DeviceNameCode()
{
    static char machine[64];
    struct utsname systemInfo;
    uname(&systemInfo);
    strncpy(machine, systemInfo.machine, 64);
    return machine;
}


extern "C" const char *DeviceOSVersionCode()
{
    
    return [[[UIDevice currentDevice]systemVersion] UTF8String];
}

extern "C" const float getOSVersinInFloatValue()
{
    float value = [[[UIDevice currentDevice]systemVersion] floatValue];
    assert(value != 0.0);
    return value;
}

extern "C" const char *GetIOSOpenUdid()
{
    NSString *uuid = getUUIDFromKeychain();
    return [uuid UTF8String];
}

extern "C" const char *DeviceLanguageSetting()
{
	NSArray* preferredLang = [NSLocale preferredLanguages];

	if ( preferredLang && [preferredLang count] > 0 )
	{
		NSString* localeIdentifier = [preferredLang objectAtIndex:0];

		if ( localeIdentifier )
		{
            NSString* newStr = [localeIdentifier stringByReplacingOccurrencesOfString :@"-" withString:@"_"];
			return [newStr UTF8String];
		}
	}

    return "en";
}

extern "C" void disableIdleTimer(bool enable)
{
    [UIApplication sharedApplication].idleTimerDisabled = enable;
}



extern "C" int IsRootedDevice()
{
    int i, isRooted = 0;

    const char* jailbreak_tool_pathes[] = {
        "/Applications/Cydia.app",
        "/Library/MobileSubstrate/MobileSubstrate.dylib",
        "/bin/bash",
        "/usr/sbin/sshd",
        "/etc/apt"
    };

    for(i = 0; i < 5; ++i) {
        if ([[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:jailbreak_tool_pathes[i]]]) {
            isRooted += 1;
        }
    }

    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@"cydia://"]]) {
        isRooted += 1;
    }

    if ([[NSFileManager defaultManager] fileExistsAtPath:@"/User/Applications/"]) {
     
        isRooted += 1;
    }

    if(getenv("DYLD_INSERT_LIBRARIES")) {
        isRooted += 1;
    }

    return isRooted;
}

extern "C" int IsQEmuEnvDetected()
{
    return 0;
}

extern "C" const char* GetInstallerName()
{
    return NULL;
}

extern "C" const char* GetNetworkType()
{
    int netType = -1;
    NetworkStatus status = [[Reachability reachabilityForInternetConnection] currentReachabilityStatus];
    switch(status)
    {
        case NotReachable:
            netType = -1;
            return "none";
            break;
        case ReachableViaWiFi:
            netType = 1;
            return "wifi";
            break;
        case ReachableViaWWAN:
            netType = 0;
            return "cell";
            break;
        default:
            netType = 0;
            break;
    }
    return "none";
    //return netType;
}

static bool setUUIDToKeychain(const char * Writeuudid);

extern "C" int RewriteToKeychain(const char * Writeuudid)
{
    return setUUIDToKeychain(Writeuudid)?1:0;
}
extern "C" void OpenURL(const char* url)
{
   // [[UIApplication sharedApplication] openURL:<#(nonnull NSURL *)#> //options:<#(nonnull NSDictionary<NSString *,id> *)#> completionHandler:<#^(BOOL success)completion#>
     [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSUTF8StringEncoding]] options:@{} completionHandler:nil];
}
extern "C" const char* CallNativeFuntionByJson(const char * jsonstring)
{
    return "error on ios";
}


extern "C" void CopyTextToPasteboard(const char * text);
extern "C" const char* GetPasteboardText();

extern "C" void showAlertView(const char* message, int tag){ printf("FakeWin32ShowAlertView %s, %d \n", message, tag); }
extern "C" void addLocalNotification (const char * title,const char * content,int delaysecond,int isdailyloop){}




extern "C" void PopUpGameExitUI(){}
extern "C" void ExitGame(){exit(0);}
extern "C" const char *PlatformTimeZoneArea(){return [[[NSTimeZone localTimeZone] name] UTF8String];}
extern "C" const char *PlatformAndroidID(){return "";}
extern "C" const char *PlatformAndroidDeviceID(){return "this a ios device ";}
extern "C" const char *PlatformDeviceMacAddress(){return "";}
extern "C" const char *GetOpenUdid(){ return GetIOSOpenUdid();}
extern "C" const char *GPUDevVersion(){return "IOS_GPU";}
extern "C" const char *GetCPUModel(){return "cpu";}
extern "C" const char *DeviceNameCode();
extern "C" const char *DeviceOSVersionCode();
extern "C" void SetPastBoard(const char* param){CopyTextToPasteboard(param);}
extern "C" const char* GetPastBoard(){return GetPasteboardText();}
extern "C" int IsWifiOK(){return ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] == ReachableViaWiFi);}
extern "C" bool DoCreateAndShowWebView(const char *url, int x, int y, int w, int h);
extern "C" void DoHideWebView(bool);
extern "C" int IsNetOK(){return ([[Reachability reachabilityForInternetConnection] currentReachabilityStatus] != NotReachable );}

extern int TrackingAssert_impl(const char*key,const char*param){return 1;}
extern "C" void Flurry_EventLog(const char * key,const char * p1,const char* p2,const char*p3);
extern "C" void EventLog(const char * key,const char * p1,const char * p2,const char * p3)
{
    Flurry_EventLog(key,p1,p2,p3);
}

extern "C" void CallSDKFunction(const char * jsoncontent)
{
    
}