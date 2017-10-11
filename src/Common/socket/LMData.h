 
#ifndef asiklskldfio902fjloskl_netmesage_h_kljafklejkiejfdksl
#define asiklskldfio902fjloskl_netmesage_h_kljafklejkiejfdksl
#include "../Common/socket/NTMSG.h"
#include "../LuaInterface/LuaInterface.h"
#define Class_LuaMessageObject CLMData
//#CLuaMessage_LoadMessage#LoadMsg
//#CLuaMessage_CNM#CreateNewMessage
//#CLuaMessage_GS#GetSize
//#CLuaMessage_RR#ReadRaw
//#CLuaMessage_WR#WriteRaw
//#CLuaMessage_BFW#BeginForWrite
//#CLuaMessage_EFW#EndForWrite
//#CLuaMessage_SM#SendMsg
class CLMData
{
public:
	CLMData(const char* instClassName);
	~CLMData();
public:
	void Class_LuaMessageObject_releaseMem(){
		if (m_pMsgNetMessage){
			delete m_pMsgNetMessage;
		}
		m_pMsgNetMessage = NULL;
	}
	const char * getLuaStateStringParam(lua_State *L,int index);
	int CLuaMessage_ZeroParam_LM(lua_State *L);
	int CLuaMessage_NZeroParam_LM(lua_State *L);
	int CLuaMessage_LoadMessage(lua_State *L);
	int CLuaMessage_CNM(lua_State *L);
	int CLuaMessage_GS(lua_State *L);
	int CLuaMessage_RR(lua_State *L);
	int CLuaMessage_WR(lua_State *L);
	int CLuaMessage_BFW(lua_State *L);
	int CLuaMessage_EFW(lua_State *L);
	int CLuaMessage_ZeroParam_SM(lua_State *L);
	int CLuaMessage_NZeroParam_SM(lua_State *L);
	int CLuaMessage_SM(lua_State *L);
	int CLuaMessage_GetLast_CALL_STP(lua_State *L);
	int SetCallStep(int id, const char * idname);
	LUNPLUS_DECLARE_INTERFACE(CLMData);
private:
	int m_callstep;
	const char * m_callstepName;
	NTMSG* m_pMsgNetMessage;

};
#endif
