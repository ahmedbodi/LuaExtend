#ifndef owefjo23090jf092390239_SLTable_h
#define owefjo23090jf092390239_SLTable_h
#include <vector>
#include <string>
#include <map>
#define BIT_NAMESPACE "eng.bit"
int SALTATable(lua_State *L);
int LALTATable(lua_State *L);
int RmvDelFileL(lua_State *L);
#include "lua.hpp"
#ifdef __cplusplus
extern "C" {
#endif
	int eng_lua_bit_register(lua_State *L);
#if __cplusplus
}
#endif
#endif 