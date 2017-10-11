#ifndef _cnjkjweiofjskdkfjkfjfjfjjeijfiejfiefjiasdkfjl_cnfhfj_h
#define _cnjkjweiofjskdkfjkfjfjfjjeijfiejfiefjiasdkfjl_cnfhfj_h

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "IO/BaseStream.h"
class CEFile
{
public:
	CEFile(const char* f);
	~CEFile(void);
	bool exist() const {
		return m_exist;
	}
	BaseStreamPtr openFS(ESM::MD mode);
	int length() const
	{
		return m_length;
	}
	const char* fname()  const {
		return m_filename.c_str();
	}
private:
	bool	m_exist;
	int		getfilesize(FILE*fpte);
	int		m_length;
	std::string m_filename;
};
typedef CPtr<CEFile> CEFilePtr;
#endif