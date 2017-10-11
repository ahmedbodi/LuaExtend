#ifndef iowoeifjiooioifwei_common_halsfldl
#define iowoeifjiooioifwei_common_halsfldl
//#pragma once
#include <map>
#include <string>

#ifndef WIN32
#define MAX(valuea, valueb)		std::max(valuea,valueb)
#else 
#define MAX(valuea, valueb)		max(valuea,valueb)
#endif

#define CHECK_DEL(pointtmem) if(pointtmem) {	delete pointtmem;		pointtmem = NULL;	}
#define CHECK_DEL_ARRAY(pointtmem) if(pointtmem) {	delete[] pointtmem;	pointtmem = NULL;	}

#ifndef FALSE
#define FALSE 0
#endif

#define MARC_NEW new
#define MARC_DELETE delete
#define MARC_DELETE_ARRAY delete[]
 
#define RECORD_GET_LUA_SDK(L) int __saveStackIndex_makelong_fornot_conflict = lua_gettop((L));
#define RECOVER_SVD_LUA_SDK(L, i) while(lua_gettop((L)) > (__saveStackIndex_makelong_fornot_conflict + (i))) lua_remove((L), __saveStackIndex_makelong_fornot_conflict + 1);


#ifdef WIN32
#define strcasecmp				_stricmp
#define strcmpmyscc				_strcmp
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifdef WIN32
#define MIN(valuea, valueb)		std::min(valuea,valueb)
#else 
#define MIN(valuea, valueb)		min(valuea,valueb)
#endif




#endif