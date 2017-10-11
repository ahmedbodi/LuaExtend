/*
** $Id: lopcodes.c,v 1.37.1.1 2007/12/27 13:02:25 roberto Exp $
** See Copyright Notice in lua.h
*/


#define lopcodes_c
#define LUA_CORE


#include "lopcodes.h"


/* ORDER OP */

const char *const luaP_opnames[NUM_OPCODES+1] = {
	"NOTUSED0",
	"NOTUSED1",
	"NOTUSED2",
	"NOTUSED3",
	"NOTUSED4",
	"NOTUSED5",
	"NOTUSED6",

	/*36*/ "CLOSURE",
	/*35*/ "CLOSE",
	/*34*/ "SETLIST",
	/*33*/ "TFORLOOP",
	/*32*/ "FORPREP",
	/*31*/ "FORLOOP",
	/*30*/ "RETURN",
 /*10*/ "NEWTABLE",
 /*11*/ "SELF",
 /*12*/ "ADD",
 /*13*/ "SUB",
 /*14*/ "MUL",
 /*15*/ "DIV",
 /*16*/ "MOD",
 /*17*/ "POW",
 /*18*/ "UNM",
 /*19*/ "NOT",
 /*9*/ "SETTABLE",
 /*8*/ "SETUPVAL",
 /*7*/ "SETGLOBAL",
 /*6*/ "GETTABLE",
 /*5*/ "GETGLOBAL",
 /*4*/ "GETUPVAL",
 /*3*/ "LOADNIL",
 /*2*/ "LOADBOOL",
 /*1*/ "LOADK",
 /*0*/ "MOVE",
 /*20*/ "LEN",
 /*21*/ "CONCAT",
 /*22*/ "JMP",
 /*23*/ "EQ",
 /*24*/ "LT",
 /*25*/ "LE",
 /*26*/ "TEST",
 /*27*/ "TESTSET",
 /*28*/ "CALL",
 /*29*/ "TAILCALL",

 /*37*/ "VARARG",
  NULL
};


#define opmode(t,a,b,c,m) (((t)<<7) | ((a)<<6) | ((b)<<4) | ((c)<<2) | (m))

const lu_byte luaP_opmodes[NUM_OPCODES] = {
	opmode(0, 1, OpArgR, OpArgN, iABC),/*NOTUSED0*/
	opmode(0, 1, OpArgR, OpArgN, iABC),/*NOTUSED1*/
	opmode(0, 1, OpArgR, OpArgN, iABC),/*NOTUSED2*/
	opmode(0, 1, OpArgR, OpArgN, iABC),/*NOTUSED3*/
	opmode(0, 1, OpArgR, OpArgN, iABC),/*NOTUSED4*/
	opmode(0, 1, OpArgR, OpArgN, iABC),/*NOTUSED5*/
	opmode(0, 1, OpArgR, OpArgN, iABC),/*NOTUSED6*/

/*       T  A    B       C     mode		   opcode	*/
/*36*/ opmode(0, 1, OpArgU, OpArgN, iABx)		/* OP_CLOSURE */
/*35*/, opmode(0, 0, OpArgN, OpArgN, iABC)		/* OP_CLOSE */
/*34*/, opmode(0, 0, OpArgU, OpArgU, iABC)		/* OP_SETLIST */
/*33*/, opmode(1, 0, OpArgN, OpArgU, iABC)		/* OP_TFORLOOP */
/*32*/, opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_FORPREP */
/*31*/, opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_FORLOOP */
/*30*/, opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RETURN */
/*10*/, opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_NEWTABLE */
/*11*/, opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_SELF */
/*12*/, opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_ADD */
/*13*/, opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_SUB */
/*14*/, opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_MUL */
/*15*/, opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_DIV */
/*16*/, opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_MOD */
/*17*/, opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_POW */
/*18*/, opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_UNM */
/*19*/, opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_NOT */
/*9*/, opmode(0, 0, OpArgK, OpArgK, iABC)		/* OP_SETTABLE */
/*8*/, opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_SETUPVAL */
/*7*/, opmode(0, 0, OpArgK, OpArgN, iABx)		/* OP_SETGLOBAL */
/*6*/, opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_GETTABLE */
/*5*/, opmode(0, 1, OpArgK, OpArgN, iABx)		/* OP_GETGLOBAL */
/*4*/, opmode(0, 1, OpArgU, OpArgN, iABC)		/* OP_GETUPVAL */
/*3*/, opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_LOADNIL */
/*2*/, opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_LOADBOOL */
/*1*/, opmode(0, 1, OpArgK, OpArgN, iABx)		/* OP_LOADK */
/*0*/, opmode(0, 1, OpArgR, OpArgN, iABC) 		/* OP_MOVE */
/*20*/, opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_LEN */
/*21*/, opmode(0, 1, OpArgR, OpArgR, iABC)		/* OP_CONCAT */
/*22*/, opmode(0, 0, OpArgR, OpArgN, iAsBx)		/* OP_JMP */
/*23*/, opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_EQ */
/*24*/, opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_LT */
/*25*/, opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_LE */
/*26*/, opmode(1, 1, OpArgR, OpArgU, iABC)		/* OP_TEST */
/*27*/, opmode(1, 1, OpArgR, OpArgU, iABC)		/* OP_TESTSET */
/*28*/, opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_CALL */
/*29*/, opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_TAILCALL */
/*37*/, opmode(0, 1, OpArgU, OpArgN, iABC)		/* OP_VARARG */
};

