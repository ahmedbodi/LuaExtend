
#include "stdafx.h"

#include "GameApp.h"
#include "SLTable.h"
#include "LuaInterface/LuaInterface.h"

#ifdef _WIN32
#define snprintf _snprintf
#endif


typedef union {
	double ud;
	bool ub;
	char*us;
	int  ui;
	char uc;
	float uf;
	void*ut;
} UCSVLUAT;
class LUATVSC
{
public:
	int t;
	UCSVLUAT v;
	int Ld(FILE*f);
	int LdB(FILE*f);
	int LdT(FILE*f);
	int LdN(FILE*f);
	int LdS(FILE*f);
	void PV(lua_State *L);
	~LUATVSC();
	void RLTBL();
	void RLTSTR();
};

class CLTBL
{
public:
	int ti;
	std::map<LUATVSC*, LUATVSC*> tm;
	union {
		int  ui;
		char uc;
		float uf;
	}A;
	CLTBL(){ ti = 1; }
	int LLST(FILE*fptr);
	bool WVSDW()
	{
		return sizeof(A) == 4; 
	}
	void CALTBL(lua_State *L);
	void CMBKVIT(lua_State*L, LUATVSC *k, LUATVSC *v);
	~CLTBL();
	void RMMAPIA(LUATVSC *p)
	{
		delete p;
	}
};


CLTBL::~CLTBL()
{
	std::map<LUATVSC*, LUATVSC*>::iterator   i = tm.begin();
	for (; i != tm.end(); ++i)
	{
		RMMAPIA((LUATVSC *)i->first);
		RMMAPIA((LUATVSC *)i->second);		
	}
	tm.clear();
}

int CLTBL::LLST(FILE*fptr)
{
	LUATVSC*k = new LUATVSC();
	int ec = k->Ld(fptr);
	if (ec != 1)
	{
		while (k->t != LUA_TNONE && ec != 1)
		{
			LUATVSC*sv = new LUATVSC();
			ec = sv->Ld(fptr);
			tm[k] = sv;
			k = new LUATVSC();
			if (ec == 1)
			{
				break;
			}
			else
			{
				ec = k->Ld(fptr);
			}
		}
	}
	delete k;
	if (ec == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void CLTBL::CMBKVIT(lua_State*L, LUATVSC *k, LUATVSC *v)
{
	LUATVSC * tk = k;
	tk->PV(L);
	LUATVSC * tv = v;
	tv->PV(L);
	lua_settable(L, -3);
}
void CLTBL::CALTBL(lua_State *L)
{
	lua_State *Local = L;
	lua_newtable(Local);
	std::map<LUATVSC*, LUATVSC*>::iterator   it = tm.begin();
	for (; it != tm.end(); ++it)
	{
		CMBKVIT(Local, (LUATVSC*)it->first, (LUATVSC*)it->second);
	}
}
//
void LUATVSC::RLTBL()
{
	if (t != LUA_TTABLE)
		return;
	CLTBL * tT = (CLTBL*)v.ut;
	delete tT;
	v.ut = NULL;
}
void LUATVSC::RLTSTR()
{
	if (t != LUA_TSTRING)
		return;
	char * tS = v.us;
	delete tS;
	v.us = NULL;
}
LUATVSC::~LUATVSC()
{
	if (t == LUA_TTABLE)
	{
		RLTBL();
	}
	else if (t == LUA_TSTRING)
	{
		RLTSTR();
	}
}
int LUATVSC::LdB(FILE*f)
{
	return fread(&v.uc, 1, 1, f);
}
int LUATVSC::LdT(FILE*f)
{
	v.ut = new CLTBL();
	return ((CLTBL*)v.ut)->LLST(f);
}
int LUATVSC::LdN(FILE*f)
{
	char *r = (char *)&(v.ud);
	for (int i = 0; i < 8; i++)
	{
		fread(&r[7-i], 1, 1, f);
	}	
	return 1;
}
int LUATVSC::LdS(FILE*f)
{
	int tts = 0;
	fread(&tts, 4, 1, f);
	if (tts < 32755)
	{
		v.us = new char[tts + 3];
		memset(v.us, 0, (tts + 3)*sizeof(char));
		fread(v.us, tts, 1, f);
		return 0;		
	}
	else
	{
		return 1;  //a error string
	}
}
int LUATVSC::Ld(FILE*f)
{
	char ttype_value;
	fread(&ttype_value, 1, 1, f);
	t = ttype_value;
	if (LUA_TBOOLEAN == t){	
		LdB(f);
	}
	else if(LUA_TTABLE == t)
	{
		return LdT(f);
	}
	else if (LUA_TNUMBER == t)
	{
		LdN(f);
	}
	else if (LUA_TSTRING == t)
	{
		if (LdS(f) == 1)
			return 1;
	}
	else if(LUA_TNONE == t || 255 == t)
	{
		t = LUA_TNONE;
	}
	else if(LUA_TNIL == t)
	{
		return 0;
	}else 
	{
		return 1;  //return error 	
	}	
	return 0;
}
void LUATVSC::PV(lua_State *L)
{
	if (LUA_TBOOLEAN == t)
	{
		lua_pushboolean(L, v.uc == 1);
	}
	else if (LUA_TNUMBER == t)
	{
		lua_pushnumber(L, v.ud);
	}
	else if (LUA_TTABLE == t)
	{
		((CLTBL*)v.ut)->CALTBL(L);
	}
	else if (LUA_TSTRING == t)
	{
		lua_pushstring(L, v.us);
	}
	else if (LUA_TNIL == t)
	{
		lua_pushnil(L);
	}
	else
	{
		ASSERT(0);
	}
}
static FILE*getFptrFromL(lua_State *L)
{
	size_t s;
	char tfn[1023];		
	snprintf(tfn, 1023, "%s%s", GameApp::getInstance()->getSavePath(), luaL_checklstring(L, 1, &s));
	return fopen(tfn, "rb");
}
int LALTATable(lua_State *L)
{	
	FILE*f = getFptrFromL(L);
	if (f != NULL)
	{
		int c = fgetc(f);
		CLTBL*tT = new CLTBL();
		int ec = tT->LLST(f);
		fclose(f);
		if (ec != 1)			
			tT->CALTBL(L);
		else
			lua_pushnil(L);
		delete tT;
		return 1;
	}
	else
	{
		return 0;
	}
} 
 
static void SLV(FILE*f, lua_State *L, int _sid);
static void SLVNBER(FILE*f, lua_State *L, int _sid)
{
	char Tt = LUA_TNUMBER;
	fwrite(&Tt, 1, 1, f);
	double ttt;
	lua_Number ti = lua_tonumber(L, -2);
	ttt = ti;
	char *wb = (char *)(&ttt);
	for (int i = 0; i < 8; i++)
	{
		fwrite(&wb[7 - i], 1, 1, f);
	}

}
static void SLVSTRI(FILE*f, lua_State *L, int _sid)
{
	const char *s = luaL_checkstring(L, -2);	
	char tSt = LUA_TSTRING;
	fwrite(&tSt, 1, 1, f);
	int sz = strlen(s);
	fwrite(&sz, 4, 1, f);
	fwrite(s, sz, 1, f);
}
static void SLT(FILE*f, lua_State *L, int sId) {
	ASSERT(sId != -1 && "sId index error  must not equels -1");
	ASSERT(lua_istable(L, sId) && "not a lua table");
	int len = (int)lua_objlen(L, sId);
	lua_pushnil(L);
	while (lua_next(L, sId) != 0) {
		if (lua_isnumber(L, -2)) {
			SLVNBER(f, L, sId);
		}
		else
		{
			SLVSTRI(f, L, sId);
		}		
		SLV(f, L, lua_gettop(L));
		lua_pop(L, 1);
	}
	{
		char tvend = -1;
		fwrite(&tvend, 1, 1, f);
	}
}
static void SLV_B(FILE*f, lua_State *L, int I)
{
	char bt = lua_toboolean(L, I) == 1 ? 1 : 0;
	char Btt = LUA_TBOOLEAN;
	fwrite(&Btt, 1, 1, f);
	fwrite(&bt, 1, 1, f);
}
static void SLV_NB(FILE*f, lua_State *L, int I)
{
	double t = lua_tonumber(L, I);
	char *wb = (char *)&t;
	char Ntt = LUA_TNUMBER;
	fwrite(&Ntt, 1, 1, f);
	for (int i = 0; i < 8; i++)
		fwrite(&wb[7 - i], 1, 1, f);
}
static void SLV_STR(FILE*f, lua_State *L, int I)
{
	const char * ts = lua_tostring(L, I);
	int tsz = strlen(ts);
	char _tSt = LUA_TSTRING;
	fwrite(&_tSt, 1, 1, f);
	fwrite(&tsz, 4, 1, f);
	fwrite(ts, tsz, 1, f);
}
static void SLV_TBL(FILE*f, lua_State *L, int I)
{
	char T = LUA_TTABLE;
	fwrite(&T, 1, 1, f);
	SLT(f, L, I);
}
static void SLV_NIL(FILE*f, lua_State *L, int I)
{
	char N = LUA_TNIL;
	fwrite(&N, 1, 1, f);
}
static void SLV(FILE*f, lua_State *L, int I) {
	int t = lua_type(L, I);
	if(LUA_TBOOLEAN == t)
	{
		SLV_B(f, L, I);
	}
	else if (LUA_TSTRING == t)
	{
		SLV_STR(f, L, I);
	}else if(LUA_TNUMBER == t)
	{
		SLV_NB(f, L, I);
	
	}
	else if (LUA_TNIL == t)
	{
		SLV_NIL(f, L, I);
	}
	else if(LUA_TTABLE == t)
	{
		SLV_TBL(f, L, I);
	}	
	else
	{
		ASSERT(0);
	}
}
static FILE*SALTAB_GFPTR(lua_State *L)
{	
	size_t tlsz;
	const char* tTN = luaL_checklstring(L, 1, &tlsz);
	char tfnS[1023];
	snprintf(tfnS, 1023, "%s%s", GameApp::getInstance()->getSavePath(), tTN);
	return fopen(tfnS, "wb");	
}
int SALTATable(lua_State *L)
{
	int n = lua_gettop(L);
	FILE*f = SALTAB_GFPTR(L);
	if (!f)
	{
		return 0;
	}
	else
	{
		int aL = n - 1;
		SLV(f, L, 0 + 2);
		fclose(f);
		return 0;
	}
}

int RmvDelFileL(lua_State *L)
{
    int n;
	char tfS[1024];
	size_t tl;
	n = lua_gettop(L);
	const char* tFN = luaL_checklstring(L, 1, &tl);
	snprintf(tfS, 1024, "%s%s", GameApp::getInstance()->getSavePath(), tFN);
	lua_pushinteger(L, remove(tfS));
	return 1;
}

static const struct luaL_Reg cleanrfuns[] =
{
	{ NULL, NULL }
};
static int AndOperateOfLuaBitL(lua_State * L)
{	
	lua_pushinteger(L, ((unsigned int)(luaL_checkinteger(L, 1))) & ((unsigned int)(luaL_checkinteger(L, 2))));
	return 1;
}
static int OrOperateOfLuaBitL(lua_State * L)
{	
	lua_pushinteger(L, ((unsigned int)(luaL_checkinteger(L, 1))) | ((unsigned int)(luaL_checkinteger(L, 2))));
	return 1;
}

static const struct luaL_Reg funsregister[] =
{	
	{ "Or", &OrOperateOfLuaBitL },
	{ "And", &AndOperateOfLuaBitL },
	{ NULL, NULL }
};
int eng_lua_bit_register(lua_State *L)
{
	RECORD_GET_LUA_SDK(L);
	luaL_newmetatable(L, BIT_NAMESPACE);
	luaL_register(L, NULL, cleanrfuns);
	printf("register lua bit operate of lua value");
	luaL_register(L, BIT_NAMESPACE, funsregister);
	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);
	RECOVER_SVD_LUA_SDK(L, 0);
	return 0;
}
