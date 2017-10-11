#ifndef _lsdfjlk_klsdkjfdkl_klsdkldskl_klsdklds_h_JSIDEJF_LD
#define _lsdfjlk_klsdkjfdkl_klsdkldskl_klsdklds_h_JSIDEJF_LD
#include <stdio.h>
#include "ZipData.h"
#include <stdlib.h>
#include "CFStream.h"
#include <string>
using namespace std;
class zFRder : public ReadFileInt
{
private:
	FILE* m_point;
	string m_fath;
public:
	zFRder(){ m_point = NULL; }
	virtual ~zFRder(){ close(); }
	virtual void close(){ fclose(m_point); m_point = NULL; }
	virtual void seek(int o, int w){ fseek(m_point, o, w); }
	virtual void seek_c(int o){ fseek(m_point, o, SEEK_CUR); }
	virtual int read(void* o, int s);
	virtual void seek_s(int o){ fseek(m_point, o, SEEK_SET); }
	virtual void seek_e(int o){ fseek(m_point, o, SEEK_END); }
	virtual int length();
	virtual bool open(string filepath);

};

class CZFRder
{
private:
	bool readscan();
	FileBaseStreamPtr			m_fbsp;
	map<string, ZipFile> m_lst;
	zFRder                 m_zFRder;
	FILE* m_f;
public:
	CZFRder(const FileBaseStreamPtr &sFS);

	virtual ~CZFRder(){}
	bool exist(const char *fn);
	FileBaseStreamPtr openFile(ZipFile &f);
	FileBaseStreamPtr createFileBaseStreamFromMem(char * data, int size, const char * fn);
	virtual void close(){ fclose(m_f); m_f = NULL; }
	virtual void seek(int o, int w){ fseek(m_f, o, w); }
	virtual int read(void* o, int s){ return fread(o,s,1,m_f); }
	virtual int length(){ return 0; }	
	size_t fileLength(const char *fn);
	string getSearchFileName(const char *fn);
	virtual FileBaseStreamPtr openFile(const char *fn);
	int searchFile(const char *fn, ZipFile &f);

 
};
#endif