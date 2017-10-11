#ifndef _engj_838iskldlifjieoooppwieijf_kdjfeilkdfj_h
#define _engj_838iskldlifjieoooppwieijf_kdjfeilkdfj_h
#include "lua.hpp"
#define SOCKET_NAMESPACE "eng.socket"


#include <set>
#include "../../LuaInterface/LuaInterface.h"

#include "SocketConnectionManager.h"
#define FUNCTION_INTERFACE_TO_LUA_GETSOCKET_NAME GetSocketOjbectName
#define FUNCTION_INTERFACE_TO_LUA_UPDATESOCKET_NET_MESSAGE UpdateSocketOjbectNetMsg
#define FUNCTION_INTERFACE_TO_LUA_CKECK_PENGDING_MESSAGE SocketOjbectHsPdingMsg
#define FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION_F ClsConOfSocketObject
#define FUNCTION_INTERFACE_TO_LUA_CONNECTION_TO_SERVER Conn_2Ser_SocketOBject
#define FUNCTION_INTERFACE_TO_LUA_KEEP_CONNECTION_ALIVED SetConisAlivedSocketInfo
#define FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER ReConn_2Ser_SocketObject
#define FUNCTION_INTERFACE_TO_LUA_INIT_ENCRYPT_SEED InitEncryptSocketSeed

class S_O_TCP
{
public:
	S_O_TCP(const char* instClassName)
	{	
		m_pointSocketConnectionMgr = NULL;
		s_staticForMultSocketobjList.insert(this);		
		m_socketObjectName = instClassName;
	}
	~S_O_TCP()
	{
		if (m_pointSocketConnectionMgr)
		{
			m_pointSocketConnectionMgr->CloseConnect();
			m_pointSocketConnectionMgr->ClearCachedMsg();
		}
		s_staticForMultSocketobjList.erase(this);
	}
private:
	CSocketConnectionManager * m_pointSocketConnectionMgr;
	std::string m_socketObjectName;
	int a;
	int b;
	std::string c;
	float d;
	double f;
public:
	static std::set<S_O_TCP*> s_staticForMultSocketobjList;
public:
	CSocketConnectionManager * GetConnectionSocketManager();
	void RestartTheSocketObject();
	std::string getSocketName(){ return m_socketObjectName; };
	
	int getA(){ return a; }
	int getB(){ return b; }
	const char* getC(){ return c.c_str(); }
	float getD(){ return d; }
	double getF(){ return f; }
	void setA(int A){ a = A; }
	void setB(int B){ b = B; }
	void setC(const char *C){ c = C; }
	void setD(float D){ d = D; }
	void setF(double F){ f = F; }

	int doaction_a_add_b(lua_State *L);
	int doaction_a_cut_b(lua_State *L);
	int doaction_a_mul_b(lua_State *L);
	int doaction_a_div_b(lua_State *L);
	int debug_out_c(lua_State *L);
	int debug_out_d(lua_State *L);
	int debug_out_f(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_GETSOCKET_NAME(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_UPDATESOCKET_NET_MESSAGE(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_CKECK_PENGDING_MESSAGE(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION_F(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_CONNECTION_TO_SERVER(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_KEEP_CONNECTION_ALIVED(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_INIT_ENCRYPT_SEED(lua_State *L);
	int FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER(lua_State *L);
	
	LUNPLUS_DECLARE_INTERFACE(S_O_TCP);
};
extern S_O_TCP*GetSocketObjectByDefaultName();
extern S_O_TCP*GetSocketObjectByName(const char * socketname);

#define STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER ext_socket_static_con2Ser
#define STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER_STR "connect"

#define STATIC_FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER ext_socket_static_recon2Ser
#define STATIC_FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER_STR "reconnect"

#define STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE ext_socket_static_hsPdingMsgSocket
#define STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE_STR "hasPendingMsg"

#define STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE updateNetMsg
#define STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE_STR "update"

#define STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED initPike
#define STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED_STR "initEncrypt"

#define STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT keepAlived
#define STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT_STR "keepAlived"

#define STATIC_FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION closeConnection
#define STATIC_FUNCTION_INTERFACE_TO_LUA_CLOSE_CONNECTION_STR "close"

#define STATIC_FUNCTION_INTERFACE_TO_LUA_ENERATE_ENCRYPT_SECRET generateSecret
#define STATIC_FUNCTION_INTERFACE_TO_LUA_ENERATE_ENCRYPT_SECRET_STR "generateSecret"

static int STATIC_FUNCTION_INTERFACE_TO_LUA_CONNEC_TO_SERVER(lua_State *L);
static int STATIC_FUNCTION_INTERFACE_TO_LUA_RECONNECT_TO_SERVER(lua_State *L);
static int STATIC_FUNCTION_INTERFACE_TO_LUA_HAS_PENDING_MESSAGE(lua_State *L);
static int STATIC_FUNCTION_INTERFACE_TO_LUA_UPDATE_NET_MESSAGE(lua_State *L);
static int STATIC_FUNCTION_INTERFACE_TO_LUA_SET_ENCRYPT_SEED(lua_State *L);
static int STATIC_FUNCTION_INTERFACE_TO_KEEP_SOCKET_ALIVED_INT(lua_State *L);
int eng_lua_socket_register(lua_State *L);
#endif