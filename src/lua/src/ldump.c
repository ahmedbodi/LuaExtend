/*
** $Id: ldump.c,v 2.8.1.1 2007/12/27 13:02:25 roberto Exp $
** save precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#include <stddef.h>

#define ldump_c
#define LUA_CORE

#include "lua.h"

#include "lobject.h"
#include "lstate.h"
#include "lundump.h"

typedef struct {
 lua_State* L;
 lua_Writer writer;
 void* data;
 int strip;
 int status;
} DumpState;

#define DumpMem(b,n,size,D)	DumpBlock(b,(n)*(size),D)
#define DumpVar(x,D)	 	DumpMem(&x,1,sizeof(x),D)
#define MAXENDECODEBLOCKSIZE (32)
int luaendecode_xorarray_hklccljqchkw_sign20170626_hjdjjiehwhldf[64] = 
{	
	31, 92, 72, 84, 13, 34, 79, 95,
	10, 59, 17, 28, 37, 47, 96, 57,
	13, 29, 27, 48, 31, 43, 97, 59,
	71, 22, 78, 86, 14, 35, 78, 96,
	17, 92, 87, 68, 41, 53, 87, 69,
	30, 99, 70, 87, 43, 75, 0, 85,
	1, 95, 71, 82, 73, 74, 69, 75,
	3, 99, 7, 78, 34, 57, 90, 58,
};
int EnDecodeBlock(void * out, const void * in, size_t size)
{
	if (size >= MAXENDECODEBLOCKSIZE)
	{
		return 0; // return false;
	}
	else
	{
		int i = 0;
		for (i = 0; i < size; i ++)
		{
			const char * inc = in;
			char*outc = out;
			char v = inc[i] ^ (luaendecode_xorarray_hklccljqchkw_sign20170626_hjdjjiehwhldf[i]);
			if (inc[i] != 0 && v != 0)
			{
				outc[i] = v;
			}
			else
			{
				outc[i] = inc[i];
			}
		}
		return 1; // retun true;
	}
}
int g_useEnDecodeBlock = 0;
static void DumpBlock(const void* b, size_t size, DumpState* D)
{
 if (D->status==0)
 {
  lua_unlock(D->L);
  if (g_useEnDecodeBlock == 1)
  {
	  char sss[MAXENDECODEBLOCKSIZE + 1] = {0};
	  if (EnDecodeBlock(sss,b,size) == 1)
	  {  
		  D->status = (*D->writer)(D->L, sss, size, D->data);
		  lua_lock(D->L);
		  return;
	  }
  }

  D->status = (*D->writer)(D->L, b, size, D->data);
  lua_lock(D->L);
 }
}

static void DumpChar(int y, DumpState* D)
{
 char x=(char)y;
 DumpVar(x,D);
}

static void DumpInt(int x, DumpState* D)
{
 DumpVar(x,D);
}

static void DumpNumber(lua_Number x, DumpState* D)
{
 DumpVar(x,D);
}

static void DumpVector(const void* b, int n, size_t size, DumpState* D)
{
 DumpInt(n,D);
 DumpMem(b,n,size,D);
}

static void DumpString(const TString* s, DumpState* D)
{
 if (s==NULL || getstr(s)==NULL)
 {
  unsigned int size=0;
  DumpVar(size,D);
 }
 else
 {
  unsigned int size=s->tsv.len+1;		/* include trailing '\0' */
  DumpVar(size,D);
//  char ss[MAXENDECODEBLOCKSIZE] = { 0 };
//  if(EnDecodeBlock(ss, getstr(s), size) == 0)
	DumpBlock(getstr(s),size,D);
//  else 
//	DumpBlock(ss, size, D);
 }
}

#define DumpCode(f,D)	 DumpVector(f->code,f->sizecode,sizeof(Instruction),D)

static void DumpFunction(const Proto* f, const TString* p, DumpState* D);

static void DumpConstants(const Proto* f, DumpState* D)
{
 int i,n=f->sizek;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  const TValue* o=&f->k[i];
  DumpChar(ttype(o),D);
  switch (ttype(o))
  {
   case LUA_TNIL:
	break;
   case LUA_TBOOLEAN:
	DumpChar(bvalue(o),D);
	break;
   case LUA_TNUMBER:
	DumpNumber(nvalue(o),D);
	break;
   case LUA_TSTRING:
	DumpString(rawtsvalue(o),D);
	break;
   default:
	lua_assert(0);			/* cannot happen */
	break;
  }
 }
 n=f->sizep;
 DumpInt(n,D);
 for (i=0; i<n; i++) DumpFunction(f->p[i],f->source,D);
}

static void DumpDebug(const Proto* f, DumpState* D)
{
 int i,n;
 n= (D->strip) ? 0 : f->sizelineinfo;
 DumpVector(f->lineinfo,n,sizeof(int),D);
 n= (D->strip) ? 0 : f->sizelocvars;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  DumpString(f->locvars[i].varname,D);
  DumpInt(f->locvars[i].startpc,D);
  DumpInt(f->locvars[i].endpc,D);
 }
 n= (D->strip) ? 0 : f->sizeupvalues;
 DumpInt(n,D);
 for (i=0; i<n; i++) DumpString(f->upvalues[i],D);
}

static void DumpFunction(const Proto* f, const TString* p, DumpState* D)
{
 DumpString((f->source==p || D->strip) ? NULL : f->source,D);
 DumpInt(f->linedefined,D);
 DumpInt(f->lastlinedefined,D);
 DumpChar(f->nups,D);
 DumpChar(f->numparams,D);
 DumpChar(f->is_vararg,D);
 DumpChar(f->maxstacksize,D);
 g_useEnDecodeBlock = 1;
 DumpCode(f,D);
 DumpConstants(f,D);
 DumpDebug(f,D); 
 g_useEnDecodeBlock = 0;
}

static void DumpHeader(DumpState* D)
{
 char h[LUAC_HEADERSIZE];
 luaU_header(h);
 DumpBlock(h,LUAC_HEADERSIZE,D);
}

/*
** dump Lua function as precompiled chunk
*/
int luaU_dump (lua_State* L, const Proto* f, lua_Writer w, void* data, int strip)
{
 DumpState D;
 D.L=L;
 D.writer=w;
 D.data=data;
 D.strip=strip;
 D.status=0;
 DumpHeader(&D);
 DumpFunction(f,NULL,&D);
 return D.status;
}
