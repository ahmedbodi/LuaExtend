#include <stdlib.h>
#include "crc32.h"

#include "lua.hpp"

#include "stdio.h"
#include <sys/stat.h>
#include "IO/FileBaseStream.h"
#include "IO/CFSys.h"


#define CRC32_TO_LUA_INTERFACE_CRC32 crcCode32L
#define CRC32_TO_LUA_INTERFACE_CRC32_STR "crc32"
#define CRC32_TO_LUA_INTERFACE_GET_FILE_SIZE fszL
#define CRC32_TO_LUA_INTERFACE_GET_FILE_SIZE_STR "size"


extern bool GetFName(const char *fin, char *fout, int len);
#define CRC_CODE (0xffffffffUL)
const unsigned long c_hash_table[256] =
{
	CRCTABLE_VALUE
};
static int crc32_file_size(lua_State *L,const char * filefullname)
{
	char str[32];
	int filesize = GET_FS()->fileLength(filefullname);
	if (!filesize)
	{
		lua_pushstring(L, "");
		return 1;
	}
	else
	{
		int length_fromluastate = filesize;
		sprintf(str, "%lu", length_fromluastate);
		lua_pushstring(L, str);
	}
	return 1;
}
static int CRC32_TO_LUA_INTERFACE_GET_FILE_SIZE(lua_State *L)
{
	FileBaseStreamPtr filestreamingptr = 0;
	size_t length_fromluastate;
	const char* fname = luaL_checklstring(L, 1, &length_fromluastate);
	char fnameReal[1024];
	GetFName(fname, fnameReal, 1024);
	return crc32_file_size(L, fnameReal);
}


unsigned long getCrc32ForInputData(unsigned long crcCode, const unsigned char *input, unsigned len)
{
	if (input != 0){
		crcCode = crcCode ^ CRC_CODE;
		while (len >= 16)
		{
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8);
			len -= 16;
		}
		if (len == 0) return crcCode ^ CRC_CODE;
		do { crcCode = c_hash_table[((int)crcCode ^ (*input++)) & 0xff] ^ (crcCode >> 8); } while (--len);
		return crcCode ^ CRC_CODE;
	}
	else
	{
		return 0;
	}
}

static int crc32_file_crc32(lua_State *L, const char * filefullname)
{
	FileBaseStreamPtr filestreamingptr = 0;
	filestreamingptr = GET_FS()->OpenFile(filefullname, "r");
	if (filestreamingptr.get() && filestreamingptr->rOrw())
	{
		size_t file_size_record = 0;
		file_size_record = filestreamingptr->fileLength();
		unsigned char* buffiledata = 0;
		buffiledata = (unsigned char*)malloc(file_size_record);
		filestreamingptr->read(buffiledata, file_size_record);
		unsigned long res_of_crc32 = 0;
		res_of_crc32 = getCrc32ForInputData(CRC_CODE, buffiledata, file_size_record);
		char strreturntolua[32];
		sprintf(strreturntolua, "%lu", res_of_crc32);
		lua_pushstring(L, strreturntolua);
		free(buffiledata);
		return 1;
	}
	else
	{
		lua_pushstring(L, "");
		return 1;
	}
}

extern "C" void extCrc32(const char* src, int sz, std::string& outStr)
{
	unsigned long res_of_crc32 = getCrc32ForInputData(CRC_CODE, (const unsigned char *)src, sz);
	outStr.clear();
	char a[256];
	sprintf(a,"%lu",res_of_crc32);
	outStr += a;
	return;
}

static int CRC32_TO_LUA_INTERFACE_CRC32(lua_State *L)
{   
	size_t file_size_record = 0;
	const char* fname = luaL_checklstring(L, 1, &file_size_record);
    char fnameReal[1024];
	GetFName(fname, fnameReal, 1024);
	return crc32_file_crc32(L,fnameReal);
} 

static const struct luaL_Reg emptyfunsforclean[] = {
    {NULL, NULL}
};

static const struct luaL_Reg funsofcrcregister[] = {
	{ CRC32_TO_LUA_INTERFACE_CRC32_STR, CRC32_TO_LUA_INTERFACE_CRC32 },
	{ CRC32_TO_LUA_INTERFACE_GET_FILE_SIZE_STR, CRC32_TO_LUA_INTERFACE_GET_FILE_SIZE },
    {NULL, NULL}
};

extern "C" int eng_lua_crc32_register(lua_State *L)
{
	luaL_newmetatable(L, CRC32_NAMESPACE);
	luaL_register(L, NULL, emptyfunsforclean);
	printf("register crc32_namespace");
	luaL_register(L, CRC32_NAMESPACE, funsofcrcregister);
    lua_pop(L, 2); 
    return 0;
}
