#include "assert.h"
#include <stdio.h>
#include "GameApp.h"
#include "S_O_TCP.h"
#include <stdlib.h>
#include "SocketConnectionManager.h"
#include "lua.hpp"

static int STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE(lua_State *L)
{
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("Call %s function to lua!",#STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE);
#endif
	S_O_TCP* D_F_S = GetSocketObjectByDefaultName();
	if (D_F_S)
		D_F_S->setC("STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE");
	return GetSocketObjectByDefaultName()->FUNCTION_INTERFACE_TO_LUA_CKECK_PENGDING_MESSAGE(L);
}

static int STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER(lua_State *L)
{
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("Call %s function to lua!", #STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER);
#endif
	S_O_TCP* D_F_S = GetSocketObjectByDefaultName();
	if (D_F_S)
		D_F_S->setC("STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER");
	return GetSocketObjectByDefaultName()->FUNCTION_INTERFACE_TO_LUA_CONNECTION_TO_SERVER(L);
}

static int STATIC_FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER(lua_State *L)
{
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("Call %s function to lua!", #STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER);
#endif
	S_O_TCP* D_F_S = GetSocketObjectByDefaultName();
	if (D_F_S)
		D_F_S->setC("STATIC_FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER");
	return GetSocketObjectByDefaultName()->FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER(L);
}

static int STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE(lua_State *L)
{
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("Call %s function to lua!", #STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE);
#endif
	S_O_TCP* D_F_S = GetSocketObjectByDefaultName();
	if (D_F_S)
		D_F_S->setC("STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE");
	return GetSocketObjectByDefaultName()->FUNCTION_INTERFACE_TO_LUA_UPDATESOCKET_NET_MESSAGE(L);
}

static int STATIC_FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION(lua_State *L)
{
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("Call %s function to lua!", #STATIC_FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION);
#endif
	S_O_TCP* D_F_S = GetSocketObjectByDefaultName();
	if (D_F_S)
		D_F_S->setC("STATIC_FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION");
	return GetSocketObjectByDefaultName()->FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION_F(L);
}

static int STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED(lua_State *L)
{
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("Call %s function to lua!", #STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED);
#endif
	S_O_TCP* D_F_S = GetSocketObjectByDefaultName();
	if (D_F_S)
		D_F_S->setC("STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED");
	return GetSocketObjectByDefaultName()->FUNCTION_INTERFACE_TO_LUA_INIT_ENCRYPT_SEED(L);
}

static int STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT(lua_State *L)
{
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("Call %s function to lua!", #STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT);
#endif
	S_O_TCP* D_F_S = GetSocketObjectByDefaultName();
	if (D_F_S)
		D_F_S->setC("STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT");
	return GetSocketObjectByDefaultName()->FUNCTION_INTERFACE_TO_LUA_KEEP_CONNECTION_ALIVED(L);
}

static int STATIC_FUNCTION_INTERFACE_TO_LUA_ENERATE_ENCRYPT_SECRET(lua_State *L)
{
	unsigned int getkey1fromluastate = (unsigned int)luaL_checkinteger(L, 1);
	unsigned int getkey2fromluastate = (unsigned int)luaL_checkinteger(L, 2);
	unsigned int u1 = (unsigned int)(getkey2fromluastate >> 3) & 0xFF;
	unsigned int u2 = (unsigned int)(getkey2fromluastate >> 23) & 0xFF;
	unsigned int u3 = (unsigned int)(getkey1fromluastate >> 5) & 0xFF;
	unsigned int u4 = (unsigned int)(getkey1fromluastate >> 15) & 0xFF;
	unsigned int resulekeyreturntolua = (u1 << 24) | (u3 << 16) | (u2 << 8) | u4;
	lua_pushinteger(L, resulekeyreturntolua);
	return 1;
}
static const struct luaL_Reg emptyfunsforclean[] = {
    {NULL, NULL}
};

static const struct luaL_Reg funsforregisterluainterface[] = {
	{ STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE_STR, STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE },
	{ STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE_STR, STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE },
	{ STATIC_FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION_STR, STATIC_FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION },
	{ STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER_STR, STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER },
	{ STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT_STR, STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT },
	{ STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED_STR, STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED },
	{ STATIC_FUNCTION_INTERFACE_TO_LUA_ENERATE_ENCRYPT_SECRET_STR, STATIC_FUNCTION_INTERFACE_TO_LUA_ENERATE_ENCRYPT_SECRET },
	{ STATIC_FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER_STR, STATIC_FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER },
    {NULL, NULL}
};

int eng_lua_socket_register(lua_State *L) {
	RECORD_GET_LUA_SDK(L);
    
    luaL_newmetatable(L, SOCKET_NAMESPACE);
	luaL_register(L, NULL, emptyfunsforclean);
	luaL_register(L, SOCKET_NAMESPACE, funsforregisterluainterface);
    
    lua_pushvalue(L, -2);
#ifdef DEBUG_ENG_SOCKET_OUT
	DBG_L("for register of lua socket");
#endif
    lua_setmetatable(L, -2);
    
	RECOVER_SVD_LUA_SDK(L, 0);
    return 0;
}

S_O_TCP*GetSocketObjectByDefaultName()
{
	return GetSocketObjectByName("defaultSocket");
}

S_O_TCP * GetSocketObjectByName(const char * socketname)
{	
	for (std::set<S_O_TCP*>::iterator it = S_O_TCP::s_staticForMultSocketobjList.begin(); it != S_O_TCP::s_staticForMultSocketobjList.end(); ++it)
	{
		if ((*it)->getSocketName().compare(socketname) == 0)
		{
#ifdef DEBUG_ENG_SOCKET_OUT
			DBG_L("socket name:%s",socketname);
		}
		DBG_L("for register of lua socket");
		{
#endif
			return (*it);
		}
	}
	return NULL;
}

CSocketConnectionManager*S_O_TCP::GetConnectionSocketManager()
{
	if (m_pointSocketConnectionMgr == NULL)
	{
		m_pointSocketConnectionMgr = new CSocketConnectionManager;
		m_pointSocketConnectionMgr->SetNameOfSocketConnet(getSocketName().c_str());
	}	
	return m_pointSocketConnectionMgr;
}
void S_O_TCP::RestartTheSocketObject()
{
	if (m_pointSocketConnectionMgr != NULL)
	{
		delete m_pointSocketConnectionMgr;
	}
	m_pointSocketConnectionMgr = new CSocketConnectionManager;
	m_pointSocketConnectionMgr->SetNameOfSocketConnet(getSocketName().c_str());
}
std::set<S_O_TCP*> S_O_TCP::s_staticForMultSocketobjList;
int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_CKECK_PENGDING_MESSAGE(lua_State *L)
{	
	NTMSG* pCurrentReadMsg = GetConnectionSocketManager()->getMsgFromCache();
	if (!pCurrentReadMsg)
	{
		a = 0;
		lua_pushboolean(L, false);
		return 1;
	}
	if (!pCurrentReadMsg->NTMSG_IsOver())
	{
		b = 0;
		lua_pushboolean(L, false);
		return 1;
	}
	c = "";
	lua_pushboolean(L, true);
	return 1;
}

int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION_F(lua_State *L)
{
	d = 2;
	GetConnectionSocketManager()->CloseConnect();
	c = "3";
	GetConnectionSocketManager()->ClearCachedMsg();
	d = 4;
	return 1;
}

int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_CONNECTION_TO_SERVER(lua_State *L)
{
	const char* pAddrFromLuaState = luaL_checkstring(L, 1);
	f = 0.81;
	int portofSocketFromLuaState = luaL_checkinteger(L, 2);
	a = portofSocketFromLuaState;
	bool needEncryptToSocket = luaL_checkinteger(L, 3) == 1;
	b = portofSocketFromLuaState;
	bool ReturnValueisSuccess = GetConnectionSocketManager()->ConnectToAddrPort(portofSocketFromLuaState, pAddrFromLuaState);
	c = pAddrFromLuaState;
	lua_pushboolean(L, ReturnValueisSuccess);
	return 1;
}
int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_KEEP_CONNECTION_ALIVED(lua_State *L)
{
	int KAliveGetFromLuaState = (int)luaL_checkinteger(L, 1);
	a = KAliveGetFromLuaState;
	int kIdleGetFromLuaState = (int)luaL_checkinteger(L, 2);
	b = kIdleGetFromLuaState;
	int kIntervalGetFromLuaState = (int)luaL_checkinteger(L, 3);
	c = "13";
	int kCountGetFromLuaState = (int)luaL_checkinteger(L, 4);
	d = kCountGetFromLuaState;
	int keep[4] = { KAliveGetFromLuaState, kIdleGetFromLuaState, kIntervalGetFromLuaState, kCountGetFromLuaState };
	int rstGetkEEPaLIVE = GetConnectionSocketManager()->SetConnectionAlived(keep);
	lua_pushinteger(L, rstGetkEEPaLIVE);
	return 1;
}
int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_INIT_ENCRYPT_SEED(lua_State *L)
{
	int sendSeed = (int)luaL_checkinteger(L, 1);
	a = sendSeed;
	int recvSeed = (int)luaL_checkinteger(L, 2);
	b = recvSeed;
	GetConnectionSocketManager()->InitEncryptBySeed(sendSeed, recvSeed);
	return 1;
}
int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER(lua_State *L)
{
	d = 4;
	GetConnectionSocketManager()->ReConnectToPreServer();
	b = 5;
	return 1;
}
int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_GETSOCKET_NAME(lua_State *L)
{
	c = this->getSocketName().c_str();
	lua_pushstring(L,this->getSocketName().c_str());
	return 1;
}

int S_O_TCP::FUNCTION_INTERFACE_TO_LUA_UPDATESOCKET_NET_MESSAGE(lua_State*L)
{
	int dt = luaL_checkinteger(L, 1);
	a = dt;
	GetConnectionSocketManager()->Update(dt);
	return 1;
}

int S_O_TCP::doaction_a_add_b(lua_State *L)
{	
	lua_pushinteger(L,a+b);
	return 1;
}
int S_O_TCP::doaction_a_cut_b(lua_State *L)
{
	lua_pushinteger(L,a-b);
	return 1;
}
int S_O_TCP::doaction_a_mul_b(lua_State *L)
{
	lua_pushinteger(L, a * b);
	return 1;
}
int S_O_TCP::doaction_a_div_b(lua_State *L)
{
	lua_pushinteger(L, a * b);
	return 1;
}
int S_O_TCP::debug_out_c(lua_State *L)
{
	lua_pushstring(L, c.c_str());
	return 1;
}
int S_O_TCP::debug_out_d(lua_State *L)
{
	lua_pushnumber(L,d);	
	return 1;
}
int S_O_TCP::debug_out_f(lua_State *L)
{
	lua_pushnumber(L, f);
	return 1;
}

//#define LUNPLUS_METHOD_DECLARE(ClassName, name) {#name, &ClassName::name},
LUNPLUS_DEFINE_INTERFACE(S_O_TCP);
LUNPLUS_METHOD_BEGIN(S_O_TCP)
{
	"GetName", &S_O_TCP::FUNCTION_INTERFACE_TO_LUA_GETSOCKET_NAME
},
 
LUNPLUS_METHOD_END
