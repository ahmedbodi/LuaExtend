#ifndef __GLOBALCONFIG_H__
#define __GLOBALCONFIG_H__
#ifdef WIN32
#define ASSERT(x) assert(x)
#else
#if defined(_DEBUG)
#define ASSERT assert
#else
#define ASSERT(a) 
#endif
#endif
#include "assert.h"
extern int TrackingAssert(const char * key, const char * file, int line);
#define ENG_ASSERT(condition,key) do {bool cond = (bool)(condition); if(!(cond))TrackingAssert((key),__FILE__,__LINE__);ASSERT(cond);} while(0)
#define MAKE_GAME_CRASH {int *assert_crash = 0;*assert_crash=1;assert(0);}

enum
{
	ENG_HTTP_UNKNOWN,
	ENG_HTTP_TEXT,
	ENG_HTTP_BINARY,
	ENG_HTTP_JSON
};

enum
{
	ENG_HTTP_MODE_NORMAL,
	ENG_HTTP_MODE_DOWNLOAD,
	ENG_HTTP_MODE_NOT_ENCRYPT
};
const double ENG_HTTP_TIMEOUT = 20;
#define ENG_HTTP_CALLBACK "callback"
#define ENG_HTTP_PROGRESS_CALLBACK "progressCallback"


#endif //__GLOBALCONFIG_H__
