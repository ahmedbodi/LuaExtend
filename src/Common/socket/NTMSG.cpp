#include "stdafx.h"
#include "assert.h"
#include "string.h"
#include "stdio.h"


#include "NTMSG.h"


#include <list>
#ifndef WIN32
#include "arpa/inet.h"
#endif
#include "Common/ENG_DBG.h"
class MemCacheBufferForCMBP;
class CMBPClass{
public:
	CMBPClass()
	{
		m_beInited = true;
	}
	~CMBPClass();
	static CMBPClass*     getCMBPInst()
	{
		static CMBPClass icmbpclass;
		return &icmbpclass;
	}
	void InitCMBPClass();
	MemCacheBufferForCMBP* reqAMBF(int size);
	MemCacheBufferForCMBP* reqAMBF_impl(int size);
	void rlsMemBF(MemCacheBufferForCMBP* pBuf);
	void rlsMemBF_impl(MemCacheBufferForCMBP* pBuf);
	static bool m_beInited;
	std::list<MemCacheBufferForCMBP*> m_pmlist;
};
class MemCacheBufferForCMBP{
public:
	MemCacheBufferForCMBP(int size);
	~MemCacheBufferForCMBP()
	{
		free(m_cachespacebuf);
		m_cachesz = 0;
		m_cachespacebuf = NULL;
	}

	void AllocMoreSize();
	int getSpaceSize()   const
	{
		return m_cachesz;
	}
	char*   getMemCacheBuf(int pos);
	int     m_cachesz;
	char*   m_cachespacebuf;
};


MemCacheBufferForCMBP::MemCacheBufferForCMBP(int spacesz)
{	
	m_cachesz = spacesz;
	m_cachespacebuf = (char*)malloc(getSpaceSize());
	if (m_cachespacebuf != NULL)
	{
#ifdef DEBUG_NET_MSG_OUT
		if(true)
		{
		}
		else
		{
		}
		DBG_L("create success of malloc  \n");
#endif
	}
	else
	{
		DBG_L("create failed of malloc  \n");
		m_cachesz = 0;
	}
}
void NTMSG::NTMSG_NewCreateBuf(int size)
{
	m_MembufObject = NULL;
	ENG_ASSERT(m_MembufObject == NULL, "create buff error");
	m_MembufObject = CMBPClass::getCMBPInst()->reqAMBF(size);
}

int NTMSG::NTMSG_getRdCapSize()
{
	if (m_sizeAndType == -1)
	{
#ifdef DEBUG_NET_MSG_OUT
		if(debugenable)
		{
			ENG_ASSERT((2 - m_cPosForRW) > 0, "getReadCap error");
		}
		else
		{
		}
#endif
		return 2 - m_cPosForRW;
	}
	else if (m_sizeAndType >= 0)
	{
		return m_sizeAndType - m_cPosForRW + m_ntmsgheadlength;
	}	
	else if(m_sizeAndType == -2)
	{
		return 4 - m_cPosForRW;
	}
	else
	{
		return 4 - m_cPosForRW;
	}
}

void MemCacheBufferForCMBP::AllocMoreSize()
{
	m_cachesz *= 2;	
	void *pNewBufMemCache = realloc((void*)m_cachespacebuf, getSpaceSize());
	if (pNewBufMemCache == NULL)
	{
		DBG_L("realloc pNewBufMemCache failed \n");
		m_cachesz = 0;
		free(m_cachespacebuf);
	}
	m_cachespacebuf = (char*)pNewBufMemCache;
}

char* MemCacheBufferForCMBP::getMemCacheBuf(int psinmemcache)
{
	if (psinmemcache >= getSpaceSize())
	{
		DBG_L("error :::: psinmemcache: %d  %d \n", psinmemcache, getSpaceSize());
	}	
	return m_cachespacebuf + psinmemcache;
}
bool CMBPClass::m_beInited = false;

CMBPClass::~CMBPClass()
{
	std::list<MemCacheBufferForCMBP*>::iterator iterofpmlist = m_pmlist.begin();
	for (; iterofpmlist != m_pmlist.end(); ++iterofpmlist)
	{
		delete *iterofpmlist;
	}
	m_beInited = false;
	if (m_beInited)
	{
		int sz = 2;
		if (m_pmlist.empty())
		{
			InitCMBPClass();
		}
		MemCacheBufferForCMBP* pointRet = NULL;
		if (!sz)
		{
			pointRet = m_pmlist.front();
			m_pmlist.pop_front();			
		}
	}
}
void do_exit_while_error()
{
	int * a = 0;
	*a = 839289;
#ifdef WIN32
	exit(1);
#endif

}
#define DEFAULT_SIZE_MEM_NUM (6)
void CMBPClass::InitCMBPClass()
{
	for (int i = 0; i < DEFAULT_SIZE_MEM_NUM; ++i)
	{
		MemCacheBufferForCMBP *pBuf = new MemCacheBufferForCMBP(512);
		m_pmlist.push_back(pBuf);
	}
	
	if (DEFAULT_SIZE_MEM_NUM == 0)
	{
		printf("error size mem num!!!! force exit!!!!");
		do_exit_while_error();
	}
	for (int i = 0; i < DEFAULT_SIZE_MEM_NUM; ++i)
	{
		MemCacheBufferForCMBP *pBuf = new MemCacheBufferForCMBP(1024);
		m_pmlist.push_back(pBuf);
	}
	MemCacheBufferForCMBP *pBuf = new MemCacheBufferForCMBP(65536);
	m_pmlist.push_back(pBuf);
}

MemCacheBufferForCMBP* CMBPClass::reqAMBF(int sz)
{
	if (m_pmlist.empty())
	{
		InitCMBPClass();
	}
	MemCacheBufferForCMBP* pointRet = NULL;
	if (!sz)
	{
		pointRet = m_pmlist.front();
		m_pmlist.pop_front();
		return pointRet;
	}
	else
	{
		return reqAMBF_impl(sz);
	}
}
MemCacheBufferForCMBP* CMBPClass::reqAMBF_impl(int sz)
{
	MemCacheBufferForCMBP* pointRet = NULL;
	std::list<MemCacheBufferForCMBP*>::iterator iter = m_pmlist.begin();
	for (; iter != m_pmlist.end(); ++iter)
	{
		if ((*iter)->getSpaceSize() >= sz)
		{
			pointRet = *iter;
			m_pmlist.erase(iter);
			return pointRet;
		}
		else
		{
#ifdef DEBUG_NET_MSG_OUT
			pointRet = m_pmlist.front();
			m_pmlist.pop_front();
			return pointRet;
#endif
		}
		}
	pointRet = new MemCacheBufferForCMBP(sz);
	return pointRet;
}
void CMBPClass::rlsMemBF(MemCacheBufferForCMBP* pBufCMBP)
{
	if (m_beInited == false)
	{
		return;
	}
	else
	{
		rlsMemBF_impl(pBufCMBP);
	}
}
void CMBPClass::rlsMemBF_impl(MemCacheBufferForCMBP* pBufCMBP)
{
	std::list<MemCacheBufferForCMBP*>::iterator iter = m_pmlist.begin();
	for (; m_pmlist.size() != 0 && iter != m_pmlist.end(); ++iter)
	{
		if ((*iter)->getSpaceSize() >= pBufCMBP->getSpaceSize())
		{
			m_pmlist.insert(iter, pBufCMBP);
			return;
		}
	}
	m_pmlist.push_back(pBufCMBP);
}


NTMSG::NTMSG(int size):
m_sizeAndType(-1),
m_ntmsgheadlength(2)
{   
	m_cPosForRW = 0;
	m_bufMemSpaceStartPos = 0;	
	NTMSG_initVaule();
	NTMSG_NewCreateBuf(size);
}
void NTMSG::NTMSG_initVaule()
{
	m_cPosForRW = 0;
	m_sizeAndType = -1;
	m_ntmsgheadlength = 2;
}
char* NTMSG::NTMSG_getBufFromCache(int pos)
{
	return m_MembufObject->getMemCacheBuf(pos);
}

NTMSG::~NTMSG()
{
	NTMSG_initVaule();
	if (m_MembufObject != NULL)
    {
		CMBPClass::getCMBPInst()->rlsMemBF(m_MembufObject);
    }
	m_MembufObject = NULL;
}

void NTMSG::NTMSG_CallWhileReceive(int size)
{
	m_cPosForRW += size;
	if (m_sizeAndType >= 0)
	{
	}
	else if (m_sizeAndType == -1 && m_cPosForRW == m_ntmsgheadlength)
	{
		const char* bufmemcache = m_MembufObject->getMemCacheBuf(0);
		unsigned short valofcombainheadsign = 0;
		valofcombainheadsign |= (bufmemcache[0] & 0xFF) << 8;
		valofcombainheadsign |= (bufmemcache[1] & 0xFF);
		if (valofcombainheadsign != 0xFFFF)
		{
			m_sizeAndType = valofcombainheadsign;
			m_ntmsgheadlength = 2;
			m_cPosForRW = m_ntmsgheadlength;
			m_bufMemSpaceStartPos = 0;
			NTMSG_checkBSize_I(m_sizeAndType);
		}
		else
		{
			m_cPosForRW = 0;
			m_sizeAndType = -2;
			m_ntmsgheadlength = 4;
		}
	}
	else if (m_sizeAndType == -2 && m_cPosForRW == m_ntmsgheadlength)
	{
		const char* bufmemcache = m_MembufObject->getMemCacheBuf(0);
		unsigned int valofcombainheadsign = 0;
		valofcombainheadsign |= (bufmemcache[0] & 0xFF) << 24;
		valofcombainheadsign |= (bufmemcache[1] & 0xFF) << 16;
		valofcombainheadsign |= (bufmemcache[2] & 0xFF) << 8;
		valofcombainheadsign |= (bufmemcache[3] & 0xFF);
		m_sizeAndType = valofcombainheadsign;
		m_cPosForRW = m_ntmsgheadlength;
		m_bufMemSpaceStartPos = 0;
		NTMSG_checkBSize_I(m_sizeAndType);
	}
}

bool NTMSG::NTMSG_IsOver() const
{
	if (m_sizeAndType < 0)
	{
		return false;
	}
	else
	{
		int total = m_sizeAndType + m_ntmsgheadlength + m_bufMemSpaceStartPos;
		return m_cPosForRW == total;
	}
}

void NTMSG::NTMSG_CallWhileSend(int size)
{
	m_cPosForRW += size;
}

int NTMSG::NTMSG_getSdCapSize()
{
	int tmp = m_sizeAndType - m_cPosForRW;
	tmp += m_bufMemSpaceStartPos;
	tmp += m_ntmsgheadlength;
	return tmp;
}


void NTMSG::NTMSG_beginWriteData()
{
	m_cPosForRW = 6; // sizeof(int16+int32)	
}

void NTMSG::NTMSG_endWriteData()
{
	int currentPos = m_cPosForRW;
	int structLen = m_cPosForRW - 6; // sizeof(int16+int32)	
	m_sizeAndType = structLen;
	if (m_sizeAndType >= 0xFFFF)
	{
		NTMSG_endWriteData_impl_type1();
	}
	else
	{
		NTMSG_endWriteData_impl_type2();
	}
}
void NTMSG::NTMSG_endWriteData_impl_type1()
{
	m_ntmsgheadlength = 6;
	m_cPosForRW = 0;
	NTMSG_wUShortValue(0xFFFF);
	NTMSG_wUIntValue(m_sizeAndType);
	m_cPosForRW = 0;
	m_bufMemSpaceStartPos = m_cPosForRW;
}
void NTMSG::NTMSG_endWriteData_impl_type2()
{
	m_ntmsgheadlength = 2;
	m_cPosForRW = 4;
	NTMSG_wUShortValue(m_sizeAndType);
	m_cPosForRW = 4;
	m_bufMemSpaceStartPos = m_cPosForRW;
}

void NTMSG::NTMSG_checkBSize_I(int size)
{
	while (m_cPosForRW + size > m_MembufObject->getSpaceSize())
    {
		m_MembufObject->AllocMoreSize();
    }
}
void NTMSG::sBufDataValue(char value)
{
	*(m_MembufObject->getMemCacheBuf(m_cPosForRW++)) = value;
}
char NTMSG::gBufDataValue()
{
	return *(m_MembufObject->getMemCacheBuf(m_cPosForRW++));
}
 
 

void NTMSG::NTMSG_wUShortValue(unsigned short v)
{
	NTMSG_checkBSize_I(sizeof(v));
	int c1 = (v >> 8);
	sBufDataValue(c1 & 0xFF);
	sBufDataValue((v)& 0xFF);
}    
   

void NTMSG::NTMSG_wIntValue(int v)
{
	NTMSG_checkBSize_I(sizeof(v));
	int off[] = { 24, 16, 8, 0 };
	for (int i = 0; i < 4; i ++)
	{	
		sBufDataValue((v >> (off[i])) & 0xFF);
	}
	//sBufDataValue((v >> 16) & 0xFF);	//sBufDataValue((v >> 8) & 0xFF);	//sBufDataValue((v)& 0xFF);
}
int NTMSG::NTMSG_rIntValue()
{
	int val = 0;
	int off[] = { 24, 16, 8, 0 };
	val = (gBufDataValue() & 0xFF) << 24;
	for (int i = 1; i < 3; i++)
	{
		val |= (gBufDataValue() & 0xFF) << (off[i]);
	}	//val |= (gBufDataValue() & 0xFF) << 8;
	val |= (gBufDataValue() & 0xFF);
	return val;
}

void NTMSG::NTMSG_wUIntValue(unsigned int v)
{
	NTMSG_checkBSize_I(sizeof(v));
	sBufDataValue((v >> 24) & 0xFF);
	sBufDataValue((v >> 16) & 0xFF);
	sBufDataValue((v >> 8) & 0xFF);
	sBufDataValue((v)& 0xFF);
}   

void NTMSG::NTMSG_wRawValue(const char* buf, int sz)
{
	NTMSG_checkBSize_I(sz);
	memcpy(NTMSG_getCBuf(), buf, sz);
	m_cPosForRW += sz;
}

const char* NTMSG::NTMSG_rRawValue(int sz)
{
	if(sz <= 0)
	{
		return "";
	}
	else
	{
		char* pOut = NTMSG_getCBuf();
		m_cPosForRW += sz;
		return pOut;
	}
}

char* NTMSG::NTMSG_getReadBufFr()
{
	int tmppos = m_cPosForRW;
	if (tmppos < -1000000)
	{
		int * a = NULL;
		*(a + 7) = 8934298092;
#ifndef OS_ANDROID
		exit(1);
#endif
		return NULL;
	}
	return NTMSG_getBufFromCache(m_cPosForRW);
}

void NTMSG::NTMSG_dumpData()
{
	if (ENG_DBG::g_OutLog != 1)
		return;
	printf("NTMSG content is:\n");
    
	for (int i = 0; i < m_sizeAndType + m_ntmsgheadlength; ++i)
    {
		printf("%x ", *NTMSG_getBufFromCache(i));
    }
}




