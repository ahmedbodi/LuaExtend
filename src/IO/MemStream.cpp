
#include "stdafx.h"
#include "MemStream.h"
#if defined(OS_LINUX)
    #include "string.h"
#endif

MemStream::MemStream(int Sz) : BaseStream(ESM::FAM_READ | ESM::FAM_WRITE)
{
	setMode(ESM::FAM_READ | ESM::FAM_WRITE);
	reCreate(Sz);
	m_notshare = true;
}

MemStream::MemStream(const void* d, int s, bool notshare)
	: BaseStream(ESM::FAM_READ | ESM::FAM_WRITE)
{
	m_mem		= NULL;
	m_len		= 0;
	m_notshare	= notshare;
	m_len		= s;	
	m_mem		= (char*)d;
	m_p			= 0;
}

void MemStream::freemem()
{
	if (!m_notshare)
	{
		printf("mem will free outside!!");
	}
	else
	{
		CHECK_DEL_ARRAY(m_mem);
	}
}

MemStream::~MemStream()
{
	freemem();
}

void MemStream::reCreate(int sz)
{
	freemem();
	{
		m_notshare = true;
		m_len = sz;
		m_p = 0;
		m_mem = MARC_NEW char[m_len];
		seek(0, SeekType::PT_BEGIN);
	}
}

bool MemStream::CheckBufSize(int sz)
{	
	if (m_p + sz > m_len)
	{
		char* pO = getBuffer();
		int nA = 2 * (m_p + sz);
		m_mem = MARC_NEW char[nA];
		if (m_mem != NULL)
		{
			memcpy(m_mem, pO, m_len);
			if (m_notshare)
			{
				CHECK_DEL_ARRAY(pO);
			}
			m_len = nA;
		}
		else
		{
			m_mem = pO;
			return false;
		}
	}
	return true;
}

int MemStream::write(void *d, int s)
{
	if (CheckBufSize(s))
	{
		void * p = (void *)(m_mem + m_p);
		memcpy(p, d, s);
		m_p = m_p + s;
		return s;
	}
	else
	{
		printf("write error!!!");
		return -1;
	}
}

int MemStream::read(void *o, int s) const
{
	int t = m_len - m_p;
	int rS = s >t ? t : s;
	
	memcpy(o, m_mem + m_p, rS);
	m_p = m_p + rS;
	return rS;
}
bool MemStream::seek_set(int o)
{
	if (o < m_len)
	{
		m_p = o;
		return true;
	}
	else
	{
		return false;
	}
	return true;
}
bool MemStream::seek_cur(int o)
{
	if (m_p + o < m_len)
	{
		m_p = m_p + o;
		return true;
	}
	else
	{
		return false;
	}
	return true;
}
bool MemStream::seek_end(int o)
{
	if (o <= m_len)
	{
		m_p = m_len - o;
		return true;
	}
	else
	{
		return false;
	}
	return true;
}
bool MemStream::seek(int o, int t)
{
	if (t == SeekType::PT_CURRENT)
	{
		return seek_cur(o);
	}
	else if (t == SeekType::PT_BEGIN)
	{
		return seek_set(o);
	}
	else if (t == SeekType::PT_END)
	{
		return seek_end(o);
	}
	else
	{
		DBG_E("ERROR SEEK TYPE");
		return false;
	}
}


int MemStream::getLength() const
{
	return m_len;
}

char* MemStream::getBuffer()
{
	return m_mem;
}

bool MemStream::isValid() const
{
	return m_mem != NULL;
}

bool MemStream::isEof() const
{
	return m_p >= m_len;
}

bool MemStream::rOrw() const
{
	return m_mem != NULL;
}

bool MemStream::openFS()
{
	DBG_L("MemStream::openFS  can not call openFS of MemStream");
    return false;
}


