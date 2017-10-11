#include <stdio.h>
#include "ZipData.h"

#define SIGNCODE (0x04034b50)
#define max(x, y)   (x) > (y) ? (x) : (y)
#define SINGCODE2 (0x06054b50)
#define SIGNCODE3 (0x02014b50)
using namespace std;
void zGetFileHeader_part1(ReadFileInt * file, zFHeaderExt * head,size_t off)
{
	file->seek(off, SEEK_SET);
	file->read(&head->data, sizeof(zFHeader));
}
void zGetFileHeader_part2_readname(ReadFileInt * file, zFHeaderExt * h)
{
	file->read(h->file_name, h->data.fnlen);
	h->file_name[h->data.fnlen] = 0;
}
zFHeaderExt *zGetFileHeader(ReadFileInt *f, size_t off)
{
	zFHeaderExt *zf = new zFHeaderExt;
	zGetFileHeader_part1(f, zf, off);
	if (zf->data.sign == SIGNCODE || zf->data.sign == 0x01010101)
    {
		zGetFileHeader_part2_readname(f, zf);
		return zf;
	}
	else
	{
		delete zf;
		return NULL;
	}
}
ZipFile getZipFileByzCDirExt(zCDirExt *cenDir)
{
	ZipFile zinfo;
	zinfo.flags = cenDir->data.cmethod;
	zinfo.fileOffset = cenDir->data.fheaderoff;
	zinfo.comSize = cenDir->data.csize;
	zinfo.fileSize = cenDir->data.ucsize;
	return zinfo;
}
std::string zGetFileListGetZFfileName(ZipFile zinfo)
{
	if (zinfo.fileName.find('/') != -1)
		return zinfo.fileName.substr(zinfo.fileName.rfind('/') + 1);
	else
		return zinfo.fileName;
}

bool checkzCDirExtCmethod(zCDirExt *cenDir)
{
	if (cenDir->data.cmethod != 8 &&
		cenDir->data.cmethod != 0)
		return false;
	return true;
}
bool checkzCDirExtCmethodZhEADERNAME(zFHeaderExt * zfHeader)
{
	if (zfHeader->data.fnlen == 0 || zfHeader->file_name[zfHeader->data.fnlen - 1] == '/')
		return false;
	return true;
}
int zGetFileList(ReadFileInt *file, map<string, ZipFile> &lisf)
{
	zArchiveEnd *aE = zGetArchiveEnd(file);
	zCDirExt *DExt = NULL;
	unsigned long DirExtOff = 0;	
	for (int i = 0; i < aE->dirnum; i++)
	{		
		DExt = zGetCentralDir(file, (DirExtOff + aE->diroff));
		if (DExt == NULL)
			break;
		DirExtOff = DirExtOff + DExt->size;
		zFHeaderExt *hd = NULL;
		if (checkzCDirExtCmethod(DExt) == false)
		{
			printf("zcomp format errrro !\n");
			delete DExt;
			continue;
		}
		hd = zGetFileHeader(file, DExt->data.fheaderoff);
		if (hd == NULL)
		{
			if (DExt != NULL)
				delete DExt;
			DExt = NULL;
			continue;
		}
		string name;
		if (checkzCDirExtCmethodZhEADERNAME(hd) == false)
		{
			if (DExt != NULL)
				delete DExt;
			DExt = NULL;
			if (hd != NULL)
				delete hd;
			hd = NULL;
			continue;
		}
		else
		{

			ZipFile zinfo = getZipFileByzCDirExt(DExt);
			zinfo.fileName = hd->file_name;
			zinfo.crc32 = hd->data.crc32;
			name = zGetFileListGetZFfileName(zinfo);
			if (!name.empty())
				lisf.insert(make_pair(name, zinfo));
		}
		
		if (hd != NULL)
			delete hd;
		hd = NULL;
		if (DExt != NULL)
			delete DExt;
		DExt = NULL;
	}
	delete aE;
	return 1;
}

char *zGetFileContent(ReadFileInt *f, size_t off, size_t c_s)
{
	zFHeaderExt *hd = zGetFileHeader(f, off);
	if (hd != NULL)
	{
		char *c = new char[c_s];
		int o = off;
		o += sizeof(zFHeader);
		o += hd->data.fnlen;
		o += hd->data.extlen;
		f->seek(o, SEEK_SET);
		if (hd != NULL)
			delete hd;
		hd = NULL;
		if (c != NULL)
		{
			f->read(c, c_s);
			return c;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}



bool zGetFileContent(ReadFileInt *f, ZipFile zf, char* &o, int &size)
{
	char *c = zGetFileContent(f, zf.fileOffset, zf.comSize);

	if (8 == zf.flags)
	{
		o = new char[zf.fileSize+100];
		if (!zUncompressBuffer(c, zf.comSize, o, zf.fileSize+100))
		{	
			if (o != NULL)
			{
				delete[]o;
			}
			o = NULL;
			printf("Uncompress error\n");
			if (c != NULL)
			{
				delete[]c;
			}
			c = NULL;
			return false;
		}
		delete[]c;
		c = NULL;
		size = zf.fileSize;
	}
	else if (0 == zf.flags)
	{
		o = c;
		size = zf.fileSize;
	}
	return true;
}
void zUncompressBuffer_SetSign(char * a,char *s,size_t sl)
{
	a[0] = 0x78;
	a[1] = 0x9c;
	memcpy(a + 2, s, sl);
}
bool zUncompressBufferCheckRet(int n)
{
	if (Z_BUF_ERROR == n) {
		return false;
	}
	else if (Z_MEM_ERROR == n) {
		return false;
	}
	else if (Z_DATA_ERROR == n) {

	}
	return true;
}
bool zUncompressBuffer(char *source, size_t source_len, char *dest, size_t dest_len)
{
	char *z = new char[source_len + 2];
	zUncompressBuffer_SetSign(z, source, source_len);
	int n = uncompress((unsigned char *)dest, (uLongf*)&dest_len, (unsigned char *)z, (uLong)(source_len + 2));
	if (zUncompressBufferCheckRet(n) == false)
		return false;
	delete[]z;
	return true;
}
zCDirExt *zGetCentralDir(ReadFileInt *f, size_t o)
{
	zCDirExt *cd = new zCDirExt;
	if (cd == NULL)
	{
		return NULL;
	}
    f->seek(o, SEEK_SET);
	f->read(cd, sizeof(zCDir));
	if (SIGNCODE3 == cd->data.sign)
    {
		int t = sizeof(zCDir);
		t += cd->data.extlen;
		t += cd->data.fnlen;
		t += cd->data.commentlen;
		cd->size = t;
		return cd;
	}
	else
	{
		delete cd;
		return NULL;
	}
}

bool zVerifyCrc32(unsigned int s, char *sp, size_t l)
{
	unsigned int initvalue = crc32(0L, Z_NULL, 0);
	
	unsigned int ret = crc32(initvalue, (unsigned char *)sp, l);
	if (ret == s) 
	{
		return true;
	}
	else
	{
		return false;
	}
}


zArchiveEnd* zGetaLLOCEndArchiveFromf(ReadFileInt *f)
{
	zArchiveEnd *a = new zArchiveEnd;
	f->seek(-(int)sizeof(zArchiveEnd), SEEK_END);	
	f->read(a, sizeof(zArchiveEnd));
	return a;
}
int getmsc_achived(int sz)
{
	int msc = sz - (1 << 16) - sizeof(zArchiveEnd);
	msc = max(msc, 0);
	return sz - msc;
}
zArchiveEnd *zGetArchiveEnd(ReadFileInt *rfi)
{
	int sz = rfi->length();
	zArchiveEnd *a = zGetaLLOCEndArchiveFromf(rfi);
	if (SINGCODE2 == a->sign && 0 == a->fcomment)
	{
		return a;
	}
	else
	{	 
		int begin = 0;
		int mCL = getmsc_achived(sz);
		rfi->seek(mCL, SEEK_SET);
		char* d = new char[mCL];
		int cl = rfi->read(d, mCL);
		for (d = d + cl; *(int *)d != SINGCODE2 && begin > cl; d--)
		{
			begin++;
		}
		if (begin >= 0)
		{
			memcpy(a, d, sizeof(zArchiveEnd));
		}
		delete[]d;
		return a;
	}
}
