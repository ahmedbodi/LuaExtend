#ifndef _lsdkfjjf_ifcfilestream_h_lsdkfjle_lsdkjjef_h_ljdsf
#define _lsdkfjjf_ifcfilestream_h_lsdkfjle_lsdkjjef_h_ljdsf
#include "Common/ENG_DBG.h"
#include "BaseStream.h"
#include "CEFile.h"
class FileBaseStream : virtual public BaseStream
{
public:
	CEFilePtr m_fptr;
	int m_length;
	FileBaseStream(const CEFilePtr &file, int accessMode);	
	virtual CEFilePtr getFptr() const{ return m_fptr; }
	virtual int fileLength();
	virtual const char* fname() const{ return getFptr()->fname(); }
	virtual void attach(CEFilePtr f){ m_fptr = f; }
	virtual bool existFile();
	virtual ~FileBaseStream(){}
};

inline
FileBaseStream::FileBaseStream(const CEFilePtr &file, int accessMode)
: BaseStream(accessMode)
{
	m_fptr = file;
	m_length = -1;

	setMode(accessMode);
}
 

inline
bool FileBaseStream::existFile() 
{
	return m_fptr.get() && m_fptr->exist();
}

inline
int FileBaseStream::fileLength() 
{
	if (m_length != -1)
		return m_length;
	if (m_length == -1)
	{
		if (m_fptr.get() && m_fptr->exist())
		{
			m_length = m_fptr->length();
			return m_length;
		}	
	}
	else
	{
		return m_length;
	}
	return m_length;
}
typedef CPtr<FileBaseStream> FileBaseStreamPtr;
#endif