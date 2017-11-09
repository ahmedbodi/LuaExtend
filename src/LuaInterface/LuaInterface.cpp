#include "GameApp.h"
#include "Common/json/eng_json.h"
#include "Common/crc/crc32.h"
#include "Common/TableSL/SLTable.h"
#include "lua_extensions.h"
#include "Common/socket/S_O_TCP.h"
#include "Common/socket/LMData.h"
#include "Common/ENG_DBG.h"
#include "GlobalFunc.h"
#include "LuaInterface.h"

extern "C" int bspatch_file(const char * oldfile, const char* newfile, const char* patchfile);

extern int eng_lua_pb(lua_State *L);

int s_LuaCreateTimes = 1;
int g_CatchLuaError = 0;
int lua::CatchError(lua_State *L) {

#define LUA_ERROR_STRING_MAX    409600
	static char s_lastErrorStr[LUA_ERROR_STRING_MAX] = {0};
    
	char errorStr[LUA_ERROR_STRING_MAX];	
	int pos = 0;
	snprintf(errorStr + pos, LUA_ERROR_STRING_MAX - pos, "count:%d Err:%s\n", g_CatchLuaError, lua_tostring(L, -1));
	snprintf(s_lastErrorStr, LUA_ERROR_STRING_MAX, "count:%d Err:%s\n", g_CatchLuaError, lua_tostring(L, -1));

	pos += snprintf(errorStr + pos, LUA_ERROR_STRING_MAX - pos, "count:%d Err:%s\n", g_CatchLuaError, lua_tostring(L, -1));
	lua_Debug ldb;
	int i;
	for (i = 0; lua_getstack(_L, i, &ldb) == 1 && LUA_ERROR_STRING_MAX - pos > 0; i++) {
		lua_getinfo(_L, "Slnu", &ldb);
		const char * name = ldb.name;
		if (!name)
			name = "";
		const char * filename = ldb.source;
		if (filename[0] == '@')
			filename++;			    
		pos += snprintf(errorStr + pos, LUA_ERROR_STRING_MAX - pos, "L %d:(%s)%s@%s:%d\n",
			i, ldb.what, name, filename, ldb.currentline);
	}
	g_CatchLuaError ++;
	//enable log print while in catch error
	int tmp = ENG_DBG::g_OutLog;
	ENG_DBG::g_OutLog = 1;
	DBG_L("CatchLuaError:");
	DBG_L(errorStr);
	lua::CallLuaError(errorStr);
	////recover log print while in catch error
	ENG_DBG::g_OutLog = tmp;

#if !(defined(WIN32) || defined(MAC_OS_X) || defined(OS_LINUX))
	if (isLoadingLua == false) {
        MAKE_GAME_CRASH;
    }
#endif
    return 0;
}

void lua::RegisteAll() {
 	RegisteClassToScript();
 	RegisteGlobalFunctions();
 	RegisteConstants();
 	lua_atpanic(_L, lua::CatchError);
}

void lua::RegisteClassToScript() {
	eng_lua_json_register(_L);
	eng_lua_crc32_register(_L);
	eng_lua_socket_register(_L);
	eng_lua_extensions(_L);
	eng_lua_bit_register(_L);
	eng_lua_pb(_L);
	LuaPlus<CLMData>::Register(_L);
}

int UpdateDLCFile(lua_State *L)
{
	return GET_DLC()->UpdateFileInfo(L);
}
int SaveDCLFileInfo(lua_State *L)
{
	return GET_DLC()->SaveAllInfo(L);
}


int PopUpGameExitUIL(lua_State *L) {
	PopUpGameExitUI();
	return 0;
}
int AppVerL(lua_State *L) {
	lua_pushstring(L, AppVer());
	return 1;
}

int ExitGameL(lua_State *L) {
	ExitGame();
	return 0;
}
int PlatformTimeZoneAreaL(lua_State *L) {
	lua_pushstring(L, PlatformTimeZoneArea());
	return 1;
}
 
int PlatformAndroidIDL(lua_State *L) {
    lua_pushstring(L, PlatformAndroidID());
    return 1;
}
 
int PlatformAndroidDeviceIDL(lua_State *L){
	lua_pushstring(L, PlatformAndroidDeviceID());
	return 1;
}
 
int getSysTime(lua_State *L) {
	int time = GameApp::getInstance()->getSysTime();
    lua_pushnumber(L, time);
    return 1;
}

int getGameTime(lua_State *L) {
	int gameTime = GameApp::getInstance()->getGameTime();
    lua_pushnumber(L, gameTime);
    return 1;
}

int PlatformDeviceMacAddressL(lua_State *L) {
	lua_pushstring(L, PlatformDeviceMacAddress());
	return 1;
}


int GetOpenUdidL(lua_State *L) {
	lua_pushstring(L, GetOpenUdid());

	return 1;
}


int DeviceLanguageSettingL(lua_State *L) {
	lua_pushstring(L, DeviceLanguageSetting());
	return 1;
}

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
extern "C" void c_extMD5(const char* src, std::string& outStr);
#else
extern "C" void extMD5(const char* src, std::string& outStr);
#endif
int extMD5L(lua_State *L) {
	const char* src = luaL_checklstring(L, -1, 0);
	static std::string outStr;
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
    c_extMD5(src, outStr);
#else
    extMD5(src, outStr);
#endif
	lua_pushstring(L, outStr.c_str());
	return 1;
}


int extBase64L(lua_State *L)
{
    const char* src = luaL_checklstring(L, -1, 0);
    lua_pushstring(L, extBase64(src));
    return 1;
}

int stringFromBase64L(lua_State *L)
{
    const char* src = luaL_checklstring(L, -1, 0);
    lua_pushstring(L, "not support in win32");
    return 1;
}
 

int AddZipFileToFileSystemL(lua_State* L)
{
	const char* zipName = luaL_checklstring(L, 1, NULL);
	AddZip2FS(zipName);
	return 0;
}
 

int GetCachePathL(lua_State*L)
{
	const char *cachePath = GameApp::getInstance()->getCachePath();
	lua_pushstring(L, cachePath);
	return 1;
}
int GetAppPathL(lua_State*L)
{
	const char *appPath = GameApp::getInstance()->getAppPath();
	lua_pushstring(L, appPath);
	return 1;
}
int GetSavePathL(lua_State*L)
{
	const char *savePath = GameApp::getInstance()->getSavePath();
	lua_pushstring(L, savePath);
	return 1;
}
#ifdef OS_ANDROID
int GetObbBundlePathL(lua_State*L)
{
	const char *obbBundlePath = GameApp::getInstance()->getObbBundlePath();
	lua_pushstring(L, obbBundlePath);
	return 1;
}
#endif

bool bHasRestart = false;
int g_restartCount = 0;
int RestartGame(lua_State *L) {
	g_restartCount++;
	NotifyGameRestart();
	bHasRestart = true;
	return 0;
}
int GetRestartGameCountL(lua_State*L)
{
	lua_pushinteger(L, g_restartCount);
	return 1;
}
extern "C" const char *GPUDevVersion();

int GPUDevVersionL(lua_State *L)
{
	const char * gpuVersion = GPUDevVersion();
	lua_pushstring(L, gpuVersion);
	return 1;
}


int DeviceNameCodeL(lua_State *L) {
	lua_pushstring(L, DeviceNameCode());
	return 1;
}


int DeviceOSVersionCodeL(lua_State *L) {
	lua_pushstring(L, DeviceOSVersionCode());
	return 1;
}

int SetPastBoardL(lua_State *L){
	const char* param = luaL_checklstring(L, 1, NULL);
	SetPastBoard(param);
	return 0;
}

int GetPastBoardL(lua_State *L) {
	lua_pushstring(L, GetPastBoard());
	return 1;
}
 

int IsWifiOKL(lua_State *L) {
	lua_pushboolean(L, IsWifiOK());

	return 1;
}


int CreateAndShowWebViewL(lua_State *L)
{
	const char* url = luaL_checklstring(L, 1, NULL);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int w = luaL_checkinteger(L, 4);
	int h = luaL_checkinteger(L, 5);
	DoCreateAndShowWebView(url, x, y, w, h);
	return 0;
}


int HideWebViewL(lua_State *L)
{
    bool b = luaL_checkinteger(L, 1) == 1 ? true : false;
    DoHideWebView(b);
    return 0;
}

int GetScreenWidthL(lua_State *L)
{
	lua_pushinteger(L, GameApp::getInstance()->getWidth());
	return 1;
}

int GetScreenHightL(lua_State *L)
{
	lua_pushinteger(L, GameApp::getInstance()->getHeight());
	return 1;
}


int IsNetOKL(lua_State *L) {
	lua_pushboolean(L, IsNetOK());

	return 1;
}


int GetNetworkTypeL(lua_State *L) {
	lua_pushstring(L, GetNetworkType());

	return 1;
}



int showAlertL(lua_State *L) {
    const char* str = luaL_checklstring(L, 1, NULL);
    int tag = lua_tointeger(L, 2);
    showAlertView(str, tag);

    return 0;
}
 
 
int GetDeviceNameL(lua_State *L) {
	lua_pushstring(L, GameApp::getInstance()->getDeviceName());
    return 1;
}

int GetDeviceFullNameL(lua_State *L) {
	lua_pushstring(L, GameApp::getInstance()->getDeviceName());
    return 1;
}

int GetPlatformL(lua_State *L) {
	lua_pushstring(L,GetPlatform());
	return 1;
}


int addLocalNotificationL(lua_State *L){
	const char* nt = luaL_checklstring(L, 1, NULL);
	addLocalNotification(nt);
	return 1;
}

int GetIDFAL(lua_State *L)
{
	lua_pushstring(L, GetIDFA());
	return 1;
}


int IsFileExistL(lua_State *L)
{
    const char* filename = luaL_checklstring(L, 1, NULL);
	lua_pushboolean(L, GET_FS()->exist(filename));
    return 1;
}

int WriteUUIDL(lua_State *L)
{
    lua_pushboolean(L, 0);
    return 1;
}

extern "C" void OpenURL(const char* url);
int OpenURLL(lua_State *L)
{
	const char* url = luaL_checklstring(L, 1, NULL);
	OpenURL(url);
    return 0;
}

 extern "C" const char* CallNativeFuntionByJson(const char * jsonstring);
int CallNativeFuntionByJsonL(lua_State *L)
{
	const char* json_str = luaL_checklstring(L, 1, NULL);
    lua_pushstring(L, CallNativeFuntionByJson(json_str));
    return 1;
}
extern "C" void DestroyKeyboard();
int DestroyKeyboardL(lua_State * L)
{
	DestroyKeyboard();
	return 0;
}
extern "C" void SetKeyboardContent(const char * txt);
int SetKeyboardCotentL(lua_State * L)
{
    const char * txt = luaL_checklstring(L, 1, NULL);
    SetKeyboardContent(txt);
    return 0;
}

void SetDebugConfigValue(const char * key, const char * value);
int SetDebugConfigValueL(lua_State * L)
{
	size_t len;
	const char *key = luaL_checklstring(L, 1, &len);
	const char *value = luaL_checklstring(L, 2, &len);
	SetDebugConfigValue(key,value);
	return 0;
}
extern "C" int bspatch_file(const char * oldfile, const char* newfile, const char* patchfile);
void BSPatch_File(const char* oldfile, const char*patchfile, const char*newfile);

int BSPatch_FileL(lua_State*L)
{
	size_t len = 0;
	const char *oldfile = luaL_checklstring(L, 1, &len);	
	const char *patchfile = luaL_checklstring(L, 2, &len);
	const char *newfile = luaL_checklstring(L, 3, &len);
	BSPatch_File(oldfile,patchfile,newfile);
	return 0;
}

extern "C" void EventLog(const char * key,const char * p1,const char * p2,const char * p3);
int EventLogL(lua_State*L)
{
	size_t len = 0;
	const char *key = luaL_checklstring(L, 1, &len);	
	const char *p1 =  luaL_checklstring(L, 2, &len);
	const char *p2 =  luaL_checklstring(L, 3, &len);
	const char *p3 =  luaL_checklstring(L, 4, &len);	
	EventLog(key,p1,p2,p3);
	return 0;
}

void lua::RegisteGlobalFunctions() {
	const luaL_reg global_functions[] = {
		{ "RegisteSocketClass", lua::LuaPlus<S_O_TCP>::RegisteSocketClassL },
		{ "UpdateDLCFile", UpdateDLCFile },
		{ "BSPatchFile", BSPatch_FileL },
		{ "EventLog", EventLogL },
		{ "SaveDCLFileInfo", SaveDCLFileInfo },		
		{ "rawLoadGameText", EngLoadGameText },
		{ "rawGetGameText", EngGetGameText },
		{ "rawGetStringByLanguageAndSheet", EngGetStringByLanguageAndSheet },
		{ "DeleteFile", RmvDelFileL },
		{ "SaveTable", SALTATable },
		{ "LoadTable", LALTATable },
		{ "GetAppVersion", AppVerL },
		{ "GetTimeZone", PlatformTimeZoneAreaL },
        { "getSysTime", getSysTime},
        { "getGameTime", getGameTime},
        { "GetAndroidID", PlatformAndroidIDL},
		{ "GetDeviceID", PlatformAndroidDeviceIDL},
		{ "GetMacAddress", PlatformDeviceMacAddressL },
		{ "GetOpenUdid", GetOpenUdidL },
		{ "GetDeviceLanguage", DeviceLanguageSettingL },		
		{ "ENC1", extMD5L },		
		{ "IsFileExist", IsFileExistL },
		{ "ENC4", extBase64L},
    	{ "DENC4", stringFromBase64L },		
		{ "RestartGame", RestartGame },    	
		{ "GetRestartGameCount", GetRestartGameCountL },
        { "GetDeviceFullName", GetDeviceFullNameL},
		{ "GetDeviceVersion", DeviceNameCodeL },
		{ "GetOSVersion", DeviceOSVersionCodeL },
		{ "GetGPUVersion", GPUDevVersionL },		
		{ "IsReachableWifi", IsWifiOKL },
		{ "CreateWebView", CreateAndShowWebViewL },
		{ "HideWebView", HideWebViewL },
		{ "GetScreenWidth", GetScreenWidthL },
		{ "GetScreenHight", GetScreenHightL },
		{ "IsNetConnected", IsNetOKL },		
		{ "GetNetworkType", GetNetworkTypeL },
		{ "showAlert", showAlertL },
		{ "GetDeviceName", GetDeviceNameL},
		{ "GetPlatform", GetPlatformL },
        { "GetIDFA", GetIDFAL},
		{ "ShowExitGameDialog", PopUpGameExitUIL},
		{ "ExitGame", ExitGameL},		
		{ "AddZipToFileSystem", AddZipFileToFileSystemL },
		{ "GetCachePath",GetCachePathL},
		{ "GetAppPath", GetAppPathL },
		{ "GetSavePath", GetSavePathL },
		#ifdef OS_ANDROID
		{ "GetObbBundlePath", GetObbBundlePathL },
		#endif			
		{ "CopyStringToPastBoard", SetPastBoardL},
		{ "getCopyStringToPastBoard", GetPastBoardL},
		{ "addLocalNotification",addLocalNotificationL},
		{ "WriteUUID", WriteUUIDL },
		{ "CallNativeFuntionByJson", CallNativeFuntionByJsonL },	
		{ "OpenURL", OpenURLL },	
		{ "DestroyKeyboard", DestroyKeyboardL },
        { "SetKeyboardCotent", SetKeyboardCotentL },
		{ "SetDebugConfigValue", SetDebugConfigValueL },
		{ NULL, NULL }
	};
	RECORD_GET_LUA_SDK(_L);
	luaL_register(_L, "eng", global_functions);
	luaopen_lz4(_L);
	lua_pop(_L, 1);
	RECOVER_SVD_LUA_SDK(_L, 0)
}

void lua::RegisteConstants() {
	// ext._DEBUG =
	RECORD_GET_LUA_SDK(_L);
	lua_getglobal(_L, "eng");
	ENG_ASSERT(lua_istable(_L, -1) && "eng table must exist !", "POS_4017");
#if defined(_DEBUG)
	lua_pushboolean(_L, 1);
#else
	lua_pushboolean(_L, 0);
#endif
	lua_setfield(_L, -2, "_DEBUG");

	int w = GameApp::getInstance()->getWidth();
	int h = GameApp::getInstance()->getHeight();

	lua_pushinteger(_L, w);
	lua_setfield(_L, -2, "SCREEN_WIDTH");
	lua_pushinteger(_L, h);
	lua_setfield(_L, -2, "SCREEN_HEIGHT");

	lua_pushinteger(_L, s_LuaCreateTimes++);
	lua_setfield(_L, -2, "CreateTimes");

	lua_pop(_L, 1);
	RECOVER_SVD_LUA_SDK(_L, 0)
}

void lua::CallUpdate(int dt) {
	RECORD_GET_LUA_SDK(_L);
	lua_getglobal(_L, "Update");
	lua_pushinteger(_L, dt);
	LUA_CALL(_L, 1, 0);
	RECOVER_SVD_LUA_SDK(_L, 0);
}

void lua::CallPause() {
	RECORD_GET_LUA_SDK(_L);
    lua_getglobal(_L, "OnPause");
    LUA_CALL(_L, 0, 0);
	RECOVER_SVD_LUA_SDK(_L, 0)
}

void lua::CallResume() {
	RECORD_GET_LUA_SDK(_L);
    lua_getglobal(_L, "OnResume");
    LUA_CALL(_L, 0, 0);
	RECOVER_SVD_LUA_SDK(_L, 0)
}

void lua::CallLuaError(const char * errorstr){
	RECORD_GET_LUA_SDK(_L);
	lua_getglobal(_L, "OnLuaError");
	lua_pushstring(_L, errorstr);
	LUA_CALL(_L, 1, 0);
	RECOVER_SVD_LUA_SDK(_L, 0)
}

void lua::OnNetFailed(const char * connectName,int codeId) {
	RECORD_GET_LUA_SDK(_L);
    lua_getglobal(_L, "OnNetFailed");
	lua_pushstring(_L, connectName);
    lua_pushinteger(_L, codeId);
    LUA_CALL(_L, 2, 0);
	RECOVER_SVD_LUA_SDK(_L, 0)
}

void lua::OnTryingReconnect(const char * connectName) {
	RECORD_GET_LUA_SDK(_L);
    lua_getglobal(_L, "OnTryingReconnect");
	lua_pushstring(_L, connectName);
    LUA_CALL(_L, 1, 0);
	RECOVER_SVD_LUA_SDK(_L, 0)
}

void lua::OnConnectToServer(const char * connectName) {
	RECORD_GET_LUA_SDK(_L);
    lua_getglobal(_L, "OnConnectToServer");
	lua_pushstring(_L, connectName);
    LUA_CALL(_L, 1, 0);
	RECOVER_SVD_LUA_SDK(_L, 0)
}
