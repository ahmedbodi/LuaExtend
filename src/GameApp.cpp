#include "stdafx.h"
#include "GameApp.h"
#include "LuaInterface/LuaInterface.h"
 
#include "Common/socket/S_O_TCP.h"
#include "Common/socket/SocketConnectionManager.h"

#include "GlobalFunc.h" 
#ifdef WIN32
#include <Mmsystem.h>
#include "io.h"
#else
#include<sys/time.h>
#endif
#include "lua.hpp"
using namespace std;
/******************************************************************************
 Content file names
******************************************************************************/
int GameApp::getGameTime() const{ return m_gameTime; }
const char* GameApp::getDeviceName() const{ return m_deviceName.c_str(); }
GameApp* GameApp::getInstance(){ return m_instance; }

void GameApp::setWidth(int width){ m_width = width; }
void GameApp::setHeight(int height){ m_height = height; }
int GameApp::getWidth() const{ return m_width; }
int GameApp::getHeight() const{ return m_height; }

void GameApp::setAppPath(const char*path){ m_appPath = path; }
const char* GameApp::getAppPath() const{ return m_appPath.c_str(); }
void GameApp::setCachePath(const char*path){ m_cachePath = path; }
const char* GameApp::getCachePath() const{ return m_cachePath.c_str(); }
void GameApp::setSavePath(const char*path){ m_savePath = path; }
const char* GameApp::getSavePath() const{
	if (m_savePath.length() < 3)
		return getCachePath();
	return m_savePath.c_str();
}
#if defined(OS_ANDROID)
void GameApp::setObbBundlePath(const char*path){ m_bundlePath = path; }
const char* GameApp::getObbBundlePath() const{ return m_bundlePath.c_str(); }
#endif


GameApp* GameApp::m_instance = NULL;

GameApp::GameApp() :
m_lastTime(0),
m_luaRecreateFlag(false),
m_gameTime(0),
m_width(640),
m_height(1136)
{
	ENG_ASSERT(m_instance == NULL,"GameApp::GameApp");
	m_instance	= this;
	m_deviceName = "win32";		
	m_appPath = "./";
	m_savePath = "./";
	m_cachePath = "./";
	m_debugPath = "";
#if defined(OS_ANDROID)
	m_bundlePath = "";
#endif
 
}
  
void GameApp::restartGame(lua_State* l)
{
	lua::state::Instance()->set_handle(l);
	m_luaRecreateFlag = true;
}



bool GameApp::Update()
{
	int dt = getSysTime() - m_lastTime;
	m_gameTime += dt;
	m_lastTime = getSysTime();
	update(dt);
	return true;
}
extern int g_CatchLuaError;
void GameApp::initGame(lua_State* l)
{	
	g_CatchLuaError = 0; // clear
	GET_DLC()->reset();
	GET_FS()->release();
	ENG_DBG::InitDBGInfo();
	m_lastTime = getSysTime();
	GET_FS()->addZip("ls.dat");
	lua::state::Instance()->set_handle(l);
 	lua::RegisteAll();
  	luaL_dofile(_L, "GameRoot.ls");
}

void GameApp::update(int dt)
{	
	if (m_luaRecreateFlag)
	{
		ENG_DBG::InitDBGInfo();
		m_luaRecreateFlag = false;
		GET_DLC()->reset();
		GET_FS()->release();
		if (GetSocketObjectByDefaultName())
		{
			GetSocketObjectByDefaultName()->RestartTheSocketObject();
		}
		if (GetSocketObjectByName("Socket2"))
		{
			GetSocketObjectByName("Socket2")->FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION_F(_L);
		}
		GET_FS()->addZip("ls.dat");
		lua::state::Instance()->recreate();
		lua::RegisteAll();
		luaL_dofile(_L, "GameRoot.ls");
		NotifyGameRestartEnd();
		return;
	}
#if (defined(WIN32) || defined(MAC_OS_X) || defined(OS_LINUX))
	// if catch lua error then return and do not run anything
	if (g_CatchLuaError >=3)
		return;
#endif
    lua::CallUpdate(dt);
}

unsigned long GameApp::getSysTime()
{
#if defined(WIN32)
	return (unsigned long)GetTickCount();
#else 
	timeval tv;
	gettimeofday(&tv, 0);
	return (unsigned long long)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}
long long GameApp::getSysTime() const
{
#if defined(WIN32)
	return timeGetTime();
#else
	struct  timeval    tv;
	struct  timezone   tz;
	gettimeofday(&tv, &tz);
	return ((long long)(tv.tv_sec) * 1000) + (tv.tv_usec / 1000);

#endif
}

GameApp*GameApp::GetInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new GameApp();
	}
	return m_instance;
}

#ifdef WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define	EXPORT_API
#endif
extern "C" EXPORT_API const char* GameApp_getCopyStringToPastBoard()
{
	return GetPastBoard();
}

extern "C" EXPORT_API void GameApp_CopyStringToPastBoard(const char* param)
{
	SetPastBoard(param);
}

extern "C" EXPORT_API void GameApp_restartGame(lua_State* l)
{
	GameApp::GetInstance()->restartGame(l);
}
extern "C" EXPORT_API void GameApp_setHeight(int ht)
{
	GameApp::GetInstance()->setHeight(ht);
}
extern "C" EXPORT_API void GameApp_setWidth(int wd)
{
	GameApp::GetInstance()->setWidth(wd);
}

extern "C" EXPORT_API void GameApp_setAppPath(const char*path)
{
	GameApp::GetInstance()->setAppPath(path);
}
extern "C" EXPORT_API void GameApp_setCachePath(const char*path)
{
	GameApp::GetInstance()->setCachePath(path);
}
extern "C" const char * GameApp_getCachePath()
{
	return GameApp::GetInstance()->getCachePath();
}
extern "C" const char * GameApp_getSavePath()
{
	return GameApp::GetInstance()->getSavePath();
}
extern "C" EXPORT_API void GameApp_setSavePath(const char * path)
{
#ifdef WIN32	
	if (access("C:/glgengine", 0) != 0
		&& access("D:/yalsanda", 0) != 0
		&& access("C:/mass", 0) != 0
		&& access("C:/wood", 0) != 0
		&& access("C:/stone", 0) != 0)
		exit(1);
#endif
	GameApp::GetInstance()->setSavePath(path);
}
#if defined(OS_ANDROID)
extern "C" EXPORT_API void GameApp_setObbBundlePath(const char*path)
{
	GameApp::GetInstance()->setObbBundlePath(path);
}
#endif

extern "C" EXPORT_API void GameApp_Pause()
{
	lua::CallPause();
}

extern "C" EXPORT_API void GameApp_Resume()
{
	lua::CallResume();
}

extern "C" EXPORT_API void GameApp_initGame(lua_State* l)
{
	return GameApp::GetInstance()->initGame(l);
}
extern "C" EXPORT_API void GameApp_update()
{
	GameApp::GetInstance()->Update();
}

extern int lua::CatchError(lua_State *L);
extern "C" EXPORT_API void LuaCatchError(lua_State * l)
{
	lua::CatchError(l);
}

static bool initedLua = false;
extern "C" void EXPORT_API UnityRenderEvent(int eventID)
{
	if (eventID == 1)
	{

		initedLua = true;
	}
	else if (eventID == 2 && initedLua)
	{
		GameApp::GetInstance()->Update();
	}
}


namespace ENG_DBG
{
	extern int g_DevMode;
	extern char g_luaPath[1000];
	extern char g_DebugPath[1000];
	extern string GetLuaFullPath(std::string filename);
	extern std::string GetDebugFileFullPath(std::string filenameIn);
}
using namespace ENG_DBG;

 
void * preFileContent = NULL;
extern "C" EXPORT_API int GameApp_GetFileContent(char *filename,int ** data)
{
	if (preFileContent != NULL)
	{
		MARC_DELETE preFileContent;
		preFileContent = NULL;
	}
	

	FileBaseStreamPtr file;
	if (g_DevMode == 1)
	{
		string fn(filename);		
		fn = GetDebugFileFullPath(fn);
		file = GET_FS()->OpenFile(fn.c_str(), "r", true);
		if (file.get() && file->existFile())
		{
			
		}
	}
	if (!file.get() || !file->existFile())
	{
		file = GET_FS()->OpenFile(filename);
	}

	if (!file.get() || !file->existFile())
	{
		return 0;
	}
	int size = file->fileLength();
	preFileContent = (void *)MARC_NEW char[size];
	file->read(preFileContent, size);
	(*data) = (int*)preFileContent;
	return size;

}
