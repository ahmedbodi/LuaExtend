#ifndef _laskiejf_debug_hdjdj_lslldjfasdfewer3
#define _laskiejf_debug_hdjdj_lslldjfasdfewer3

#include "stdafx.h"
#include "TimeProfiler.h" 

namespace ENG_DBG
{

	void DOut(int l, const char* f, ...);
	extern int g_DevMode;
	extern int g_OutLog;
	extern char g_luaPath[1000];

	enum LLLE
	{	
		CPLUS_L,
		CPLUS_W,
		CPLUS_E,
		CPLUA_A_UUSED,
		CPLUA_B_UUSED,
		CPLUA_C_UUSED,
		CPLUA_D_UUSED,
		CPLUA_L_UUSED,
		CPLUA_W_UUSED,
		CPLUA_E_UUSED,
		LUA_P,
		LUA_Q,
		LUA_R,
	};
	extern char g_DebugPath[1000];
	extern void InitDBGInfo();
	
}

#define DBG_LUA_P(...) ENG_DBG::DOut(ENG_DBG::LUA_P, __VA_ARGS__)
#define DBG_L(...)   ENG_DBG::DOut(ENG_DBG::CPLUS_L, __VA_ARGS__)
#define DBG_W(...)  ENG_DBG::DOut(ENG_DBG::CPLUS_W, __VA_ARGS__)
#define DBG_E(...) ENG_DBG::DOut(ENG_DBG::CPLUS_E, __VA_ARGS__)

#define DBG_A_UUSED(...) ENG_DBG::DOut(ENG_DBG::CPLUA_A_UUSED, __VA_ARGS__)
#define DBG_B_UUSED(...) ENG_DBG::DOut(ENG_DBG::CPLUA_B_UUSED, __VA_ARGS__)
#define DBG_C_UUSED(...) ENG_DBG::DOut(ENG_DBG::CPLUA_C_UUSED, __VA_ARGS__)
#define DBG_D_UUSED(...) ENG_DBG::DOut(ENG_DBG::CPLUA_D_UUSED, __VA_ARGS__)
#define DBG_L_UUSED(...) ENG_DBG::DOut(ENG_DBG::CPLUA_L_UUSED, __VA_ARGS__)
#define DBG_W_UUSED(...) ENG_DBG::DOut(ENG_DBG::CPLUA_W_UUSED, __VA_ARGS__)
#define DBG_E_UUSED(...) ENG_DBG::DOut(ENG_DBG::CPLUA_E_UUSED, __VA_ARGS__)
#define DBG_P(...) ENG_DBG::DOut(ENG_DBG::CPLUA_P_UUSED, __VA_ARGS__)
#define DBG_Q(...) ENG_DBG::DOut(ENG_DBG::CPLUA_Q_UUSED, __VA_ARGS__)
#define DBG_R(...) ENG_DBG::DOut(ENG_DBG::CPLUA_R_UUSED, __VA_ARGS__)
#if (defined MAC_OS_X) || (defined OS_LINUX)
#define MAX_PATH 1024
#endif
#endif