#pragma once
#include "IO/BaseStream.h"

class MemStream : virtual public BaseStream
{
public:
	MemStream(int);
	MemStream(const void* d, int size, bool notshare);
	~MemStream();
	int getLength() const;
	char* getBuffer();
	void reCreate(int sz);
	bool CheckBufSize(int sz);
	int	read(void *o, int sz) const;
	int write(void *d, int sz);
	bool seek_set(int o);
	bool seek_cur(int o);
	bool seek_end(int o);
	bool seek(int o, int t);
	bool isEof() const;
	bool isValid() const;	
	bool rOrw() const;    
	bool openFS();
	void freemem();
private:
	char*	m_mem;
	bool	m_notshare;
	int		m_len;
	mutable int	m_p;
};

typedef CPtr<MemStream> MemStreamPtr;
