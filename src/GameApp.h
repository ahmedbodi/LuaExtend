#pragma once
#include "stdafx.h"
#include "IO/CFSys.h"
 
class GameApp 
{ 
public: 
	bool Update();    
	GameApp();
	static GameApp* getInstance();
	void initGame(lua_State* l);
	const char* getDeviceName() const;
	int getWidth() const;
	int getHeight() const;
	void setWidth(int wd) ;
	void setHeight(int ht) ;
    
	void update(int dt);
	
	void restartGame(lua_State* l);
    
	int getGameTime() const; 
	void setSavePath(const char*);
	const char* getSavePath() const;
	void setCachePath(const char*);
	const char* getCachePath() const;
	void setAppPath(const char*);
	const char* getAppPath() const; 
	#if defined(OS_ANDROID)
    const char* getObbBundlePath()const;	
	void setObbBundlePath(const char*);
	#endif
	void SendMessageToLua(const char * jsoncontent);
private:
	bool m_luaRecreateFlag;
	static GameApp* m_instance;
	int m_height;
	int m_width;
	int m_gameTime;	
	
	std::string m_appPath;
	std::string m_savePath;
	std::string m_cachePath;	
	std::string m_debugPath;	
#if defined(OS_ANDROID)
	std::string m_bundlePath;
#endif
	long long m_lastTime;
	std::string m_deviceName;
public:	
	long long getSysTime();
	static GameApp*GetInstance();
};
