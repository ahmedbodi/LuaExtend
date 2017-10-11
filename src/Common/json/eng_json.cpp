#include <stdlib.h>
#include <stdio.h>


#include <math.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#ifndef WIN32
#include <stdbool.h>
#endif
#include "lua.hpp"
#include "eng_json.h"
#include "yajl/yajl_bytestack.h"
#include "yajl/yajl_lex.h"
#include "yajl/api/yajl_parse.h"
#include "yajl/yajl_parser.h"
#include "yajl/api/yajl_gen.h"
static yajl_gen g_yajl_gen;
static yajl_handle g_yajl_hand;
static yajl_bytestack g_yajl_state_bytestack;
static int DecodeJsonLuaInterface(lua_State *L);
static int EncodeJsonLuaInterface(lua_State *L);
extern void gen_value_for_lua(yajl_gen gen_handle, lua_State *L);
static const struct luaL_Reg emptyfunsForClean[] = { { NULL, NULL } };
enum {
	YAJI_JS_STATE_BEGIN_STU,
	YAJI_JS_STATE_ARRAY_STU,
	YAJI_JS_STATE_HASH_STU
};
#define JSAPI_B_PUSH_MACRO yajl_bs_push
#define JSAPI_B_POP_MACRO yajl_bs_pop
#define JSAPI_ALLOC_MACRO yajl_alloc
#define JSAPI_B_INIT_MACRO yajl_bs_init
#define JSAPI_G_ALLOC_MACRO yajl_gen_alloc
#define JSAPI_PARSE_MACRO yajl_parse
#define JSAPI_COMPLETE_MACRO yajl_parse_complete
#define JSAPI_B_FREE_MACRO yajl_bs_free
#define JSAPI_FREE_MACRO yajl_free
#define JSAPI_GET_ERROR_MACRO yajl_get_error
#define JSAPI_Free_ERROR_MACRO yajl_free_error
#define JSAPI_G_STRING_MACRO yajl_gen_string
#define JSAPI_G_BOOL_MACRO yajl_gen_bool
#define JSAPI_G_NULL_MACRO yajl_gen_null
#define JSAPI_G_NUMBER_MACRO yajl_gen_number
#define JSAPI_G_CLEAR_MACRO yajl_gen_clear
#define JSAPI_G_FREE_MACRO yajl_gen_free
#define JSAPI_G_MAP_OPEN_MACRO yajl_gen_map_open
#define JSAPI_G_ARRAY_OPEN_MACRO yajl_gen_array_open
#define JSAPI_G_MAP_CLOSE_MACRO yajl_gen_map_close
#define JSAPI_G_ARRAY_CLOSE_MACRO yajl_gen_array_close
#define JSON_TO_LUA_INTERFACE_DECODE_JSON DecodeJsonLuaInterface
#define JSON_TO_LUA_INTERFACE_DECODE_JSON_STR "DecodeJson"

#define JSON_TO_LUA_INTERFACE_ENCODE_JSON EncodeJsonLuaInterface
#define JSON_TO_LUA_INTERFACE_ENCODE_JSON_STR "EncodeJson"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define ONE_RETRUN_VALUE 1
#define TWO_RETRUN_VALUE 2
#define THREE_RETRUN_VALUE 3
static const struct luaL_Reg funsfordecodeandencodejson[] = {
	{ JSON_TO_LUA_INTERFACE_DECODE_JSON_STR, JSON_TO_LUA_INTERFACE_DECODE_JSON },
	{ JSON_TO_LUA_INTERFACE_ENCODE_JSON_STR, JSON_TO_LUA_INTERFACE_ENCODE_JSON },
    {NULL, NULL}
};
int eng_lua_json_register(lua_State *L) {
	luaL_newmetatable(L, JSON_NAMESPACE);
	luaL_register(L, NULL, emptyfunsForClean);
	printf("register json_namespace!!!!");
	luaL_register(L, JSON_NAMESPACE, funsfordecodeandencodejson);
    lua_pop(L, 2);    
    return 0;
}
#define CATCH_YAJI_HASH lua_rawset(L, -3)
#define CATCH_YAJI_ARRAY lua_rawseti(L, -2, lua_objlen(L, -2) + 1)
void doByCurState(void * luas,int curstate)
{
	lua_State *L = (lua_State*)luas;
	switch (curstate)
	{
	case YAJI_JS_STATE_HASH_STU:
	{
		CATCH_YAJI_HASH;
	}
	break;
	case YAJI_JS_STATE_ARRAY_STU:
	{
		CATCH_YAJI_ARRAY;
	}
	break;
	}
}
void HashORArray(lua_State *L) {
	int curstate = yajl_bs_current(g_yajl_state_bytestack);
	if (curstate != YAJI_JS_STATE_HASH_STU && curstate != YAJI_JS_STATE_ARRAY_STU)
	{
		return;
	}
	else
	{
		doByCurState((void*)L, curstate);
	}
}

static int JsonDoNullToTable(void *luastate) {
	lua_pushnil((lua_State *)luastate);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch null value");
	}
#endif
    return TRUE;
}
static int JsonDoIntToTable(void *luastate, long num) {
	lua_pushinteger((lua_State *)luastate, num);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch int value");
	}
#endif
	return TRUE;
}
 
static int JsonDoBoolToTable(void *luastate, int b) {
	lua_pushboolean((lua_State *)luastate, b ? true : false);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch bool value");
	}
#endif
	return TRUE;
}

static int JsonDoDoubleToTable(void *luastate, double num) {
	lua_pushnumber((lua_State *)luastate, num);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch double value");
	}
#endif
	return TRUE;
}

static int JsonDoStringToTable(void *luastate, const unsigned char *string, unsigned int len) {
	lua_pushlstring((lua_State *)luastate, (const char *)string, len);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch string value");
	}
#endif
	return TRUE;
}
static int JsonDoNumberToTable(void *luastate, const char *numberBuf, unsigned int bufLen)
{   
    double dvalueget = strtod((char *)numberBuf, NULL);
	if (errno == ERANGE && (dvalueget == -HUGE_VAL || dvalueget == HUGE_VAL)) {
#ifdef DEBUG_OUT_JASON
		{
			printf("debug out json parse error");
		}
#endif
		dvalueget = -1;
#ifdef DEBUG_OUT_JASON
		{
			printf("json parase error but also return a value for programme go on!!!!");
		}
#endif
    }    
	lua_pushnumber((lua_State *)luastate, dvalueget);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch number value");
	}
#endif
	return TRUE;
}

static int JsonStartMapToTable(void *luastate) {
	JSAPI_B_PUSH_MACRO(g_yajl_state_bytestack, YAJI_JS_STATE_HASH_STU);
	lua_newtable((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch start map value");
	}
#endif
	return TRUE;
}

static int JsonStartArrayToTable(void *luastate) {
	JSAPI_B_PUSH_MACRO(g_yajl_state_bytestack, YAJI_JS_STATE_ARRAY_STU);
	lua_newtable((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch start array value");
	}
#endif
	return TRUE;
}

static int JsonEndArrayToTable(void *luastate) {
	JSAPI_B_POP_MACRO(g_yajl_state_bytestack);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch end array value");
	}
#endif
	return TRUE;
}
static int JsonMapKeyToTable(void *luastate, const unsigned char *string, unsigned int len) {
	lua_pushlstring((lua_State *)luastate, (const char *)string, len);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch mapkey value");
	}
#endif
	return TRUE;
}

static int JsonEndMapToTable(void *luastate) {
	JSAPI_B_POP_MACRO(g_yajl_state_bytestack);
	HashORArray((lua_State *)luastate);
#ifdef DEBUG_OUT_JASON
	{
		DBG_L("for catch end map value");
	}
#endif
	return TRUE;
}


static void dojsoninit()
{
	JSAPI_B_INIT_MACRO(g_yajl_state_bytestack, &(g_yajl_hand->alloc));
	yajl_bs_push(g_yajl_state_bytestack, YAJI_JS_STATE_BEGIN_STU);

#ifdef DEBUG_OUT_JASON
	g_yajl_gen = JSAPI_G_ALLOC_MACRO(&config, NULL);
	gen_value_for_lua(g_yajl_gen, L);
#endif
}
static void dojsonstatusOK()
{
	JSAPI_B_FREE_MACRO(g_yajl_state_bytestack);
	JSAPI_FREE_MACRO(g_yajl_hand);
}
static void freejsobject()
{
	JSAPI_B_FREE_MACRO(g_yajl_state_bytestack);
#ifdef DEBUG_OUT_JASON
	strgenerate = lua_tolstring(L, -1, &length);
	JSAPI_G_STRING_MACRO(gen_handle, (const unsigned char*)strgenerate, (unsigned int)length);
#endif
	JSAPI_FREE_MACRO(g_yajl_hand);
}

yajl_status getjs_status_parse(const unsigned char * input,unsigned int len)
{
	yajl_status t;
	t = JSAPI_PARSE_MACRO(g_yajl_hand, input, len);
	if (t == yajl_status_ok ||
		t == yajl_status_insufficient_data)
	{
		t = yajl_parse_complete(g_yajl_hand);
	}
	return t;
}
#define TMP_JSON_STRING_SIZE (102400)
static int parse_json_string_L(lua_State *L, const unsigned char* input, unsigned int len) {
	static yajl_callbacks CallBackFuncions = { JsonDoNullToTable, JsonDoBoolToTable, NULL, NULL,
		JsonDoNumberToTable, JsonDoStringToTable, JsonStartMapToTable, JsonMapKeyToTable,
		JsonEndMapToTable, JsonStartArrayToTable, JsonEndArrayToTable };
	char buf[TMP_JSON_STRING_SIZE];
    yajl_status js_parse_status;
	unsigned char* errorRetmessage = NULL;
	yajl_parser_config config = { 1, 0 };	
#ifdef DEBUG_OUT_JASON
	DBG_L("start alloc jason value");
#endif	
	g_yajl_hand = JSAPI_ALLOC_MACRO(&CallBackFuncions, &config, NULL, (void *)L);
#ifdef DEBUG_OUT_JASON
	DBG_L("alloc jason value");
#endif
	dojsoninit();
	js_parse_status = getjs_status_parse(input,len);
#ifdef DEBUG_OUT_JASON
	JSAPI_B_POP_MACRO(g_yajl_state_bytestack);
	HashORArray((lua_State *)luastate);
#endif
	if (js_parse_status == yajl_status_ok)
	{
		dojsonstatusOK();
    } else 
	{
		errorRetmessage = JSAPI_GET_ERROR_MACRO(g_yajl_hand, 1, input, len);
		strncpy(buf, (const char *)errorRetmessage, 102400);
		JSAPI_Free_ERROR_MACRO(g_yajl_hand, errorRetmessage);
		freejsobject();
		lua_pushstring(L, "Json ERROR: Parse JSON string Failed");
    }
	return TRUE;
}

int checkparamofdecode(lua_State*L, size_t l, const char * str)
{
	if (!str)
	{
#ifdef DEBUG_OUT_JASON
	}
	lua_pushnumber(L, 0);
	{
#endif
		lua_pushstring(L, "Json ERROR: Parse JSON string Failed");
		return ONE_RETRUN_VALUE;
	}

	if (!l)
	{
#ifdef DEBUG_OUT_JASON
	}
	lua_pushnumber(L, 0);
	{
#endif
		lua_pushnil(L);
		return ONE_RETRUN_VALUE;
	}
	return 0;
}
static int DecodeJsonLuaInterface(lua_State *L) {
    size_t jsonstr_length = 0;
	const char *jsonStringFromLua = lua_tolstring(L, -1, &jsonstr_length);
#ifdef DEBUG_OUT_JASON
	DBG_L("Decode json value %s",jsonString);
#endif

	if (checkparamofdecode(L, jsonstr_length, jsonStringFromLua) == ONE_RETRUN_VALUE)
		return ONE_RETRUN_VALUE;
	
	return parse_json_string_L(L, (const unsigned char *)jsonStringFromLua, jsonstr_length);
}

void JsonParse(lua_State *L, const char *jsonstring) {
	parse_json_string_L(L, (const unsigned char *)jsonstring, strlen(jsonstring));
}

static void ClearEncodeJsLua()
{
	JSAPI_G_CLEAR_MACRO(g_yajl_gen);
	JSAPI_G_FREE_MACRO(g_yajl_gen);
}
static int EncodeJsonLuaInterface(lua_State *L) {
	const unsigned char * buf;
	unsigned int length;
	yajl_gen_config config = { 0, "  " };
#ifdef DEBUG_OUT_JASON
	DBG_L("alloc jason value!!!!");
#endif
	g_yajl_gen = JSAPI_G_ALLOC_MACRO(&config, NULL);
	gen_value_for_lua(g_yajl_gen, L);
#ifdef DEBUG_OUT_JASON
	printf("call :" ,gen_value_for_lua(g_yajl_gen, L));
	DBG_L("generalloc a alloc jason value!!!!");
#endif
	yajl_gen_get_buf(g_yajl_gen, &buf, &length);
	lua_pushlstring(L, (const char *)buf, length);
#ifdef DEBUG_OUT_JASON
	printf("free resource of gen_value_for_lua of lsldkfasd :", gen_value_for_lua(g_yajl_gen, L));	
#endif
	ClearEncodeJsLua();
	return ONE_RETRUN_VALUE;
}
void generate_number(yajl_gen gen_handle, lua_State *L, int valuetype)
{
	size_t length;
	const char * strgenerate;
	lua_pushvalue(L, -1);
	strgenerate = lua_tolstring(L, -1, &length);
	lua_pop(L, 1);
	JSAPI_G_NUMBER_MACRO(gen_handle, strgenerate, (unsigned int)(length));
}

void pushnilandclear(lua_State *L)
{
	lua_pushvalue(L, -1);
	lua_pushnil(L);
}
void do_whilegeneratetable_for_number(lua_State *L)
{
#ifdef DEBUG_OUT_JASON
	printf("catch a number and do something for number");
#endif
}
bool checkisarrayoftablevalue(yajl_gen gen_handle, lua_State *L, int valuetype)
{
	bool isDictnotarray = false;
	bool isEmptyvalue = true;
	while (lua_next(L, -2)) {
		isEmptyvalue = false;
		if (lua_type(L, -2) == LUA_TNUMBER) {
			lua_pop(L, 1);
#ifdef DEBUG_OUT_JASON
			printf("if is lua number then pop ");
#endif
		}
		else {
			isDictnotarray = true;
#ifdef DEBUG_OUT_JASON
			printf("else is lua number then pop 2 ");
#endif
			lua_pop(L, 2);
			break;
		}
	}
	return  !(isDictnotarray || isEmptyvalue);
}
void generate_table_vaule(yajl_gen gen_handle, lua_State *L, int valuetype)
{	
	pushnilandclear(L);
 
	bool isarray = checkisarrayoftablevalue(gen_handle,L,valuetype);
	if (!isarray)
	{
		JSAPI_G_MAP_OPEN_MACRO(gen_handle);
#ifdef DEBUG_OUT_JASON
		printf("start map of jason!!!");
#endif
	}
	else
	{
#ifdef DEBUG_OUT_JASON
		printf("start array of jason!!!");
#endif
		JSAPI_G_ARRAY_OPEN_MACRO(gen_handle);
	}

	lua_pushnil(L);
	while (lua_next(L, -2)) {
		if (!isarray) {
			size_t length;
			const char *str;
			if (lua_type(L, -2) == LUA_TNUMBER) {
				lua_pushvalue(L, -2);
#ifdef DEBUG_OUT_JASON
				printf("lua number");
#endif
				str = lua_tolstring(L, -1, &length);
				do_whilegeneratetable_for_number(L);
				lua_pop(L, 1);
			}
			else {
				str = lua_tolstring(L, -2, &length);
			}
			JSAPI_G_STRING_MACRO(gen_handle, (const unsigned char*)str, (unsigned int)length);
		}
		gen_value_for_lua(gen_handle, L);
#ifdef DEBUG_OUT_JASON
		generate_over("close gen map");
#endif
		lua_pop(L, 1);
	}

	if (!isarray)
	{
#ifdef DEBUG_OUT_JASON
		printf("close gen map");
#endif
		JSAPI_G_MAP_CLOSE_MACRO(gen_handle);
	}
	else
	{
#ifdef DEBUG_OUT_JASON
		printf("close gen array");
#endif
		JSAPI_G_ARRAY_CLOSE_MACRO(gen_handle);
	}
	lua_pop(L, 1);
}
void generate_string(yajl_gen gen_handle, lua_State *L, int valuetype)
{
	size_t length;
	const char *strgenerate = lua_tolstring(L, -1, &length);
	JSAPI_G_STRING_MACRO(gen_handle, (const unsigned char*)strgenerate, (unsigned int)(length));
}
void generate_stringornumber(yajl_gen gen_handle, lua_State *L, int valuetype)
{
	
	if (valuetype == LUA_TSTRING)
	{
		generate_string(gen_handle,L,valuetype);
	}
	else
	{
		generate_number(gen_handle, L, valuetype);
	}
}

bool gen_boolvalue_forLua(int vtp, yajl_gen gen_handle, lua_State *L)
{
	if (vtp == LUA_TBOOLEAN)
	{
		JSAPI_G_BOOL_MACRO(gen_handle, lua_toboolean(L, -1));
		return true;
	}
	return false;
}
bool gen_tstrandnumber_for_lua(int vtp, yajl_gen gen_handle, lua_State *L)
{
	if (vtp == LUA_TSTRING || vtp == LUA_TNUMBER)
	{
		generate_stringornumber(gen_handle, L, vtp);
		return true;
	}
	return false;
}
bool gen_table_for_lua(int vtp, yajl_gen gen_handle, lua_State *L)
{
	if (vtp == LUA_TTABLE)
	{
		generate_table_vaule(gen_handle, L, vtp);
		return true;
	}
	return false;
}
void gen_value_for_lua(yajl_gen gen_handle, lua_State *L) {
    int valuetype = lua_type(L, -1);
	if (gen_boolvalue_forLua(valuetype, gen_handle, L))
		return;
	if (gen_tstrandnumber_for_lua(valuetype, gen_handle, L))
		return;
	if (gen_table_for_lua(valuetype,gen_handle,L))
		return;
/*	if(valuetype)
	{LUA_TNONE:LUA_TNIL:LUA_TTHREAD:LUA_TFUNCTION:LUA_TLIGHTUSERDATA:LUA_TUSERDATA:
        default:*/
	JSAPI_G_NULL_MACRO(gen_handle);
     /* 
    }*/
}

