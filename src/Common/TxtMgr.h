#ifndef _mltglsf_klsdkf_oi3oir9jf_jajlsiehhgalkefj_h
#define _mltglsf_klsdkf_oi3oir9jf_jajlsiehhgalkefj_h
#include <string>
#include <stdio.h>
#include <map>
#include <stdlib.h>
#include <vector>

#include "IO/FileBaseStream.h"
typedef std::map<std::string, size_t> TxtSizeList;
using namespace std;
class TxtMgr
{
private:	
    string m_LT;
	map<string, char**> m_txtlst;
    map<string, short> m_c;
	map<string, TxtSizeList> m_ids;
public:
	TxtMgr();
	~TxtMgr();
	static TxtMgr* Inst();
public:
	void release();
	void releaseMap();
	void releaseCharPointPoint(char ** pp,short size);
	bool pldPkSIds(const string &s);
	bool pldPkSIdsfstrptr(FileBaseStreamPtr fs, string sheet);
	bool changeswchPk(vector<string> & s,const string & l);
	const char* gS(const std::string & s, const std::string & n);
	void  setlower_sheetname(char*ls,string s);
	string preDoWhileGetStr(string s);
    const char* gStr(const char* u) ;
	bool preldPk(const vector<string> & sts,const string & l);
	bool preldPkSt(const string & st,const string & l);
};

#define GetStrMgr() TxtMgr::Inst()
#endif