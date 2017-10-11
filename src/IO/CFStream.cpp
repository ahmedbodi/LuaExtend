#include "stdafx.h"
#include "CEFile.h"
#include "FileBaseStream.h"
#include "CFStream.h"
using namespace std;
CFStream::CFStream(const char *f, int m) : FileBaseStream(CEFilePtr(), m), m_f(NULL)
{	
	CEFilePtr fp(new CEFile(f));
	bool isw = getAccessMode() & ESM::FAM_WRITE;
	if (fp->exist())
	{
		attach(fp);
        openFS();
	}
	else if (isw)
	{
		attach(fp);
		openFS();
	}
}

CFStream::CFStream(const CEFilePtr &file, int accessmode) :FileBaseStream(file, accessmode), m_f(NULL)
{	
	CEFile* t = file.get();
	bool e = file->exist();
	bool check = (!t || !e);
	if(check)
	{	
		return ;
	}
	openFS();
}

CFStream::~CFStream(void)
{
	if (m_f != NULL)
		fclose(m_f);
}

bool CFStream::openFS()
{
	string s = getFptr()->fname();
	const char * m = "rb";
	if (getAccessMode() & ESM::FAM_WRITE)
	{
		m = "wb";
		m_f = fopen(s.c_str(), "wb");
		return rOrw();
	}
	m_f = fopen(s.c_str(), m);
    return rOrw();
}

bool CFStream::rOrw() const
{
	return m_f != NULL;
}

int CFStream::read(void *o, int s) const
{
	return fread(o, sizeof(char), s, m_f);

}

int CFStream::write(void *i, int s)
{
	int retvalue = fwrite(i, sizeof(char), s, m_f);
	return retvalue;
}

bool CFStream::seek(int o, int t)
{
	if (fseek(m_f, o, t) != 0)
	{
		return false;
	}
	return true;
}


bool CFStream::isEof() const
{
	return feof(m_f);
}

