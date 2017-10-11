#include "stdafx.h"
#include "CEFile.h"
#include "CFStream.h"
#include <errno.h>

int	CEFile::getfilesize(FILE*fpte)
{
	fseek(fpte, 0, SEEK_END);
	int m_length = ftell(fpte);
	fseek(fpte, 0, SEEK_SET);
	return m_length;
}
CEFile::CEFile(const char* f) : m_filename(f)
{
	m_length = -1;
	m_exist = false;	
	FILE* l = fopen((f == NULL ? "":f), "rb");
	if(l == NULL)
	{
		return ;
	}
	else
	{
		m_exist = true;
		m_length = getfilesize(l);		
		fclose(l);
	}
}

CEFile::~CEFile(void)
{
	m_length = -1;
	m_exist = false;	
}

BaseStreamPtr CEFile::openFS(ESM::MD mode)
{
	if(exist())
	{	
		return BaseStreamPtr(new CFStream(CEFilePtr(this), (int)mode));		
	}
	else
	{
		return BaseStreamPtr();
	}
}
