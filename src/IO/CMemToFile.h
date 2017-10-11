#ifndef _ldskflsld_cmdmdmdmdmdmdmdmd_ikdjjfjkskskskfj_h_lsldkfj
#define _ldskflsld_cmdmdmdmdmdmdmdmd_ikdjjfjkskskskfj_h_lsldkfj
#include "FileBaseStream.h"
#include "MemStream.h"

#include <string>
using namespace std;
class CMemToFile : public FileBaseStream, public MemStream
{
public:
	CMemToFile(const char *d, int s, const char *n);
	~CMemToFile(void){}
	const char* fname() const {return m_fn.c_str();}
	CEFilePtr getFptr() const {return CEFilePtr();}
	void attach(CEFilePtr file){}
	bool isValid() const { return m_fn.length() > 0 && MemStream::isValid(); }
	int fileLength() {return MemStream::getLength();}
	bool existFile()  { return m_fn.length() > 0 && MemStream::isValid(); }
	bool rOrw() { return MemStream::rOrw(); }
	bool openFS(){ return MemStream::openFS(); }
private:
	string m_fn;
};
#endif
