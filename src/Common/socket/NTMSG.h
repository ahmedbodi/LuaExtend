#ifndef _Njklasdfjklsdfjklei_ldklfklklNNNNNNNNNNNKLASDF_H__
#define _Njklasdfjklsdfjklei_ldklfklklNNNNNNNNNNNKLASDF_H__
#include "string.h"
#include "assert.h"
#include "vector"
#include "stdio.h"


typedef unsigned char	UInt8;
typedef signed char	Int8;
#ifndef WIN32
typedef long long Int64;
#else
typedef __int64 Int64;
#endif

class MemCacheBufferForCMBP;

class NTMSG
{
	void    NTMSG_checkBSize_I(int size);

	// -1  : unknown 
	// -2  : need 4 byte HeadSize
	// >=0 : size known
    int m_sizeAndType;
	int m_ntmsgheadlength; //2 or 4
	int m_bufMemSpaceStartPos;

public:
	NTMSG(int size = 512);
	~NTMSG();
	int			NTMSG_GetSizeAndType() const          { return m_sizeAndType; }	
	void		NTMSG_NewCreateBuf(int bufSize = 0);
	char*		NTMSG_getBufFromCache(int pos);
	void		NTMSG_initVaule();
	char*		NTMSG_getReadBufFr();
    int			NTMSG_getRdCapSize();
	int			NTMSG_getSdCapSize();
	void		NTMSG_CallWhileReceive(int size);
	void		NTMSG_CallWhileSend(int size);	
	char*		NTMSG_getCBuf()          { return NTMSG_getBufFromCache(m_cPosForRW); }    
	bool		NTMSG_IsOver() const;
	void		NTMSG_ResetFSendPos()					{ m_cPosForRW = m_bufMemSpaceStartPos; }
	void		NTMSG_ResetFReadPos()					{ m_cPosForRW = m_bufMemSpaceStartPos + m_ntmsgheadlength; } 
	void		NTMSG_wUShortValue(unsigned short v);
	void		NTMSG_wIntValue(int v);
	int			NTMSG_rIntValue();
	void		NTMSG_wUIntValue(unsigned int v);
	void		NTMSG_wRawValue(const char* buf, int sz);
	const char* NTMSG_rRawValue(int sz);
	void		NTMSG_dumpData();
	void		NTMSG_beginWriteData();
	void		NTMSG_endWriteData();
	void		NTMSG_endWriteData_impl_type1();
	void		NTMSG_endWriteData_impl_type2();
protected:
  
    int			m_cPosForRW;
	MemCacheBufferForCMBP* m_MembufObject;
	void sBufDataValue(char value);
	char gBufDataValue();
};

#endif