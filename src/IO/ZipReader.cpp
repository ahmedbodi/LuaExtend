#include "stdafx.h"
#include <stdio.h>
#include "IO/CFStream.h"
#include <stdlib.h>
#include <string>
#include "IO/MemStream.h"
#include <vector>
#include "ZipReader.h"
#include "IO/CMemToFile.h"

CZFRder::CZFRder(const FileBaseStreamPtr &fs)
{
	m_fbsp = fs;
	readscan();
}


int zFRder::read(void* buffer, int size)
{
	if (m_point == NULL || buffer == NULL)
	{
		return -1;
	}
	else
	{
		int rSizeoffread = fread(buffer, size, 1, m_point);
		return rSizeoffread;
	}
}

size_t CZFRder::fileLength(const char *fn)
{
	ZipFile f;
	int id = searchFile(fn, f);
	int sz = f.fileSize;
	if (id != -1)
	{
		return sz;
	}
	else
	{
		return 0;
	}
}

bool CZFRder::readscan()
{
	std::string fn = m_fbsp->fname();
	std::string ext = fn.substr(fn.rfind('.') + 1, fn.length());
	if (ext == "obb" || ext == "zip" || ext == "dat")
	{
		bool t = m_zFRder.open(fn);
		if (t)
		{
			zGetFileList(&m_zFRder, m_lst);
			return true;
		}
		else
		{
			return false;
		}
		
	}
	else
	{
		return false;
	}
}

int zFRder::length()
{
	if (m_point == NULL)
	{
		return 0;
	}
	else
	{
		fseek(m_point, 0, SEEK_END);
		int retValue = ftell(m_point);
		return retValue;
	}
}

FileBaseStreamPtr CZFRder::openFile(const char *fn)
{
	ZipFile f1zip;    
	int ff = searchFile(fn, f1zip);
	if (ff != -1)
	{
		return openFile(f1zip);
	}
	else
	{
		return FileBaseStreamPtr();
	}
}


bool CZFRder::exist(const char *fn)
{
	ZipFile f;
	int ret = searchFile(fn, f);
	if( ret != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
FileBaseStreamPtr CZFRder::createFileBaseStreamFromMem(char * data, int size, const char * fn)
{
	return FileBaseStreamPtr(MARC_NEW CMemToFile(data, size, fn));
}


FileBaseStreamPtr CZFRder::openFile(ZipFile &zf)
{   
    char *d = NULL;
    int sz = 0;
	if (zGetFileContent(&m_zFRder, zf, d, sz))
	{
		return createFileBaseStreamFromMem(d, sz, zf.fileName.c_str());
	}
	else
	{
		return FileBaseStreamPtr();
	}
}

string CZFRder::getSearchFileName(const char *fn)
{
	string s = fn;
	string r;
	if (s.find('/') != -1)
	{
		r = s.substr(s.rfind('/') + 1);
	}
	else
	{
		r = s;
	}
	return r;
}

int CZFRder::searchFile(const char *sfn, ZipFile &fileOut)
{	
	map<string, ZipFile>::iterator i = m_lst.find(getSearchFileName(sfn));
	if (i == m_lst.end())
	{
		return -1;
	}
	else
	{
		fileOut = i->second;
		return 1;
	}
}


bool zFRder::open(string f)
{
	m_fath = f;
	m_point = fopen(f.c_str(), "rb");
	if (m_point != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}
