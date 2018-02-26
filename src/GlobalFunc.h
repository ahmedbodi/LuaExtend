#ifdef _WIN32
#define snprintf _snprintf
#endif
///global extern 
extern bool g_outputInfoOnDevice;
extern int isLoadingLua;
extern "C" int eng_lua_crc32_register(lua_State *L);
extern "C" void eng_lua_extensions(lua_State *L);
extern "C" void extMD5(const char* src, std::string& outStr);
extern "C" const char * extBase64(const char* src);
extern "C" const char * stringFromBase64(const char* src);
extern "C" void AddZip2FS(const char* pathname);
extern "C" void NotifyGameRestart();
extern "C" void NotifyGameRestartEnd();
extern int SALTATable(lua_State *L);
extern int LALTATable(lua_State *L);
extern int RmvDelFileL(lua_State *L);
extern int luaopen_lz4(lua_State *L);
extern int EngLoadGameText(lua_State *L);
extern int EngGetGameText(lua_State *L);
extern int EngGetStringByLanguageAndSheet(lua_State *L);
//different plateform 
extern "C" const char *AppVer();
extern "C" void PopUpGameExitUI();
extern "C" void ExitGame();
extern "C" const char *PlatformTimeZoneArea();
extern "C" const char *PlatformAndroidID();
extern "C" const char *PlatformAndroidDeviceID();
extern "C" const char *PlatformDeviceMacAddress();
extern "C" const char *GetOpenUdid();
extern "C" const char *DeviceLanguageSetting();
extern "C" const char *GPUDevVersion();
extern "C" const char *GetCPUModel();
extern "C" const char *DeviceNameCode();
extern "C" const char *DeviceOSVersionCode();
extern "C" void SetPastBoard(const char* param);
extern "C" const char* GetPastBoard();
extern "C" int IsWifiOK();
extern "C" bool DoCreateAndShowWebView(const char *url, int x, int y, int w, int h);
extern "C" void DoHideWebView(bool);
extern "C" int IsNetOK();

extern "C" const char* GetNetworkType();
extern "C" void showAlertView(const char* message, int tag);
extern "C" const char* GetPlatform();
extern "C" void addLocalNotification(const char * title,const char * content,int delaysecond,int isdailyloop);
extern "C" void clearLocalNotification();
extern "C" const char* GetPushDeviceToken();
extern "C" const char *GetIDFA();
extern "C" void OpenURL(const char* url);

extern "C" void CallSDKFunction(const char * jsoncontent);