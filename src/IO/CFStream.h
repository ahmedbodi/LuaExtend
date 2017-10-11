#pragma once

#include "FileBaseStream.h"
#include "CEFile.h"

#include <string>

class CFStream : public FileBaseStream
{
public:
	CFStream(const char *filename, int m);
	CFStream(const CEFilePtr& file, int m);	
	~CFStream();
	int	read(void *o, int sz) const;
	bool rOrw() const;
	int write(void *d, int sz);
	bool isEof() const;
	bool seek(int offset, int pos_type);
	bool openFS();
	FILE	*m_f;
};
