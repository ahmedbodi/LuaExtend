#pragma once

class CCodeProf
{
public:
	char b[512];	
	CCodeProf(const char * s);
	long t;
	~CCodeProf();
};


#define PRIFILE_CODE_BLOCK_TIME(a) CCodeProf tfortestwhichcodetimecostincblockfordebug(a);