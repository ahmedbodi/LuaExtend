 
#include "stdafx.h"
#include "LMData.h"
#include "../Common/socket/S_O_TCP.h"
#include <stdio.h>
#include <stdlib.h>
#include "../Common/socket/SocketConnectionManager.h"
#define RET_ONE 1
#define RET_ZERO 0
CLMData::CLMData(const char* n) :m_pMsgNetMessage(NULL), m_callstep(0), m_callstepName(NULL)
{
	memset(m_instClassName, 0, sizeof(m_instClassName));
    strncpy(m_instClassName, n, 128);	
}

CLMData::~CLMData()
{
	if (m_pMsgNetMessage != NULL)
    {
		Class_LuaMessageObject_releaseMem();
    }
	else
	{
		printf("has released of cluamessage!");
	}
}
int CLMData::SetCallStep(int id, const char * idname)
{
	m_callstep = id;
	m_callstepName = idname;
	return 1;
}
int CLMData::CLuaMessage_GetLast_CALL_STP(lua_State *L)
{
	lua_pushinteger(L,m_callstep);
	lua_pushstring(L,m_callstepName);
	return 2;
}
int CLMData::CLuaMessage_CNM(lua_State *L)
{	
	m_pMsgNetMessage = new NTMSG(512);
	SetCallStep(1,"CLuaMessage_CNM");
    return RET_ONE;
}

const char * CLMData::getLuaStateStringParam(lua_State *L, int index)
{
	size_t lenSizet;
	SetCallStep(2, "getLuaStateStringParam");
	return luaL_checklstring(L, index, &lenSizet);
}

int CLMData::CLuaMessage_ZeroParam_LM(lua_State *L)
{
	m_pMsgNetMessage = GetSocketObjectByDefaultName()->GetConnectionSocketManager()->getMsgFromCache();
	GetSocketObjectByDefaultName()->GetConnectionSocketManager()->RemoveMsgFromCache(m_pMsgNetMessage);
	m_pMsgNetMessage->NTMSG_ResetFReadPos();
	SetCallStep(3, "CLuaMessage_ZeroParam_LM");
	return 0;
}
int CLMData::CLuaMessage_NZeroParam_LM(lua_State *L)
{	
	const char *socketname = getLuaStateStringParam(L,1);
	S_O_TCP*tmp = GetSocketObjectByName(socketname);
	m_pMsgNetMessage = tmp->GetConnectionSocketManager()->getMsgFromCache();
	tmp->GetConnectionSocketManager()->RemoveMsgFromCache(m_pMsgNetMessage);
	m_pMsgNetMessage->NTMSG_ResetFReadPos();
	SetCallStep(4, "CLuaMessage_NZeroParam_LM");
	return 0;
}
int CLMData::CLuaMessage_LoadMessage(lua_State* L)
{
	int nParamNumber = lua_gettop(L);
	if (nParamNumber == RET_ZERO)
	{
		SetCallStep(5, "CLuaMessage_LoadMessage");
		CLuaMessage_ZeroParam_LM(L);
		return RET_ONE;
	}
	else
	{
		SetCallStep(6, "CLuaMessage_LoadMessage");
		CLuaMessage_NZeroParam_LM(L);
	}
    return RET_ONE;
}

int CLMData::CLuaMessage_GS(lua_State *L)
{
	lua_pushinteger(L, m_pMsgNetMessage->NTMSG_GetSizeAndType());
	SetCallStep(7, "CLuaMessage_GS");
	return RET_ONE;
}

int CLMData::CLuaMessage_RR(lua_State *L)
{
	int sizeParam1CatchFromLua = luaL_checkinteger(L, 1);
	const char* buf = m_pMsgNetMessage->NTMSG_rRawValue(sizeParam1CatchFromLua);
	lua_pushlstring(L, buf, sizeParam1CatchFromLua);
	SetCallStep(8, "CLuaMessage_RR");
	return RET_ONE;
}

int CLMData::CLuaMessage_WR(lua_State *L)
{
	size_t sizeOfStringLength = RET_ZERO;
	const char* buf = luaL_checklstring(L, 1, &sizeOfStringLength);
	m_pMsgNetMessage->NTMSG_wRawValue(buf, (int)sizeOfStringLength);
	SetCallStep(9, "CLuaMessage_WR");
	return RET_ONE;
}

int CLMData::CLuaMessage_ZeroParam_SM(lua_State *L)
{
	GetSocketObjectByDefaultName()->GetConnectionSocketManager()->SendMsgFromNetMsg(m_pMsgNetMessage);
	SetCallStep(10, "CLuaMessage_ZeroParam_SM");
	return 0;
}
int CLMData::CLuaMessage_NZeroParam_SM(lua_State *L)
{
	size_t lenOfParamStringLength;
	const char *socketname = luaL_checklstring(L, 1, &lenOfParamStringLength);
	S_O_TCP*tmp = GetSocketObjectByName(socketname);
	tmp->GetConnectionSocketManager()->SendMsgFromNetMsg(m_pMsgNetMessage);
	SetCallStep(11, "CLuaMessage_NZeroParam_SM");
	return 0;
}

int CLMData::CLuaMessage_SM(lua_State *L)
{
	int nOfParamSize = lua_gettop(L);
	if (nOfParamSize == RET_ZERO)
	{
		SetCallStep(12, "CLuaMessage_SM");
		CLuaMessage_ZeroParam_SM(L);
	}
	else
	{
		SetCallStep(13, "CLuaMessage_SM");
		CLuaMessage_NZeroParam_SM(L);
	}    
	m_pMsgNetMessage = NULL;
    return RET_ONE;
}

int CLMData::CLuaMessage_BFW(lua_State *L)
{
	m_pMsgNetMessage->NTMSG_beginWriteData();
	SetCallStep(14,"CLuaMessage_BFW");
    return RET_ONE;
}

int CLMData::CLuaMessage_EFW(lua_State *L)
{
	m_pMsgNetMessage->NTMSG_endWriteData();
	SetCallStep(15, "CLuaMessage_EFW");
    return RET_ONE;
}

LUNPLUS_DEFINE_INTERFACE(CLMData);
LUNPLUS_METHOD_BEGIN(CLMData)
{
	"NewC", &CLMData::CLuaMessage_CNM
},
{ "Load", &CLMData::CLuaMessage_LoadMessage },
{ "WriteBegin", &CLMData::CLuaMessage_BFW },
{ "WriteEnd", &CLMData::CLuaMessage_EFW },
{ "SendMsg", &CLMData::CLuaMessage_SM },
{ "GetSize", &CLMData::CLuaMessage_GS },
{ "ReadRaw", &CLMData::CLuaMessage_RR },
{ "WriteRaw", &CLMData::CLuaMessage_WR },

{ "ZeroParam_LM", &CLMData::CLuaMessage_ZeroParam_LM },
{ "NZeroParam_LM", &CLMData::CLuaMessage_NZeroParam_LM },
{ "ZeroParam_SM", &CLMData::CLuaMessage_ZeroParam_SM },
{ "NZeroParam_SM", &CLMData::CLuaMessage_NZeroParam_SM },
{ "GetLast_CALL_STP", &CLMData::CLuaMessage_GetLast_CALL_STP },
LUNPLUS_METHOD_END