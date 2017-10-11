#ifndef _io3090fjklsiloioweiofeiojwp0923_eng_klasdjklfjklaslkdfjl_json_h
#define _io3090fjklsiloioweiofeiojwp0923_eng_klasdjklfjklaslkdfjl_json_h
#include "lua.hpp"
#define JSON_NAMESPACE "eng.json"
#ifdef __cplusplus
extern "C" {
#endif
	int eng_lua_json_register(lua_State *L);
#if __cplusplus
}
#endif
void JsonParse(lua_State *L, const char *input);
#endif