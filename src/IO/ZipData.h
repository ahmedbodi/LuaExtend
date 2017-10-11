#ifndef _ZIP_H_
#define _ZIP_H_

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <zlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <map>
#include <iostream>
#define MAX_FILE_LENGHT 255
#pragma pack(1) 
typedef int S32;
typedef short S16;
typedef unsigned int U32;
typedef unsigned short U16;

struct CDirData
{
	U32 sign;
	U16 made_by;
	U16 extfild;
	U16 bflag;
	U16 cmethod;
	U16 mt;
	U16 md;
	U32 crc32;
	U32 csize;
	U32 ucsize;
	U16 fnlen;
	U16 extlen;
	U16 commentlen;
	U16 dns;
	U16 iattr;
	U32 eattr;
	U32 fheaderoff;
};

struct zFHData
{
	U32 sign;
	U16 extfild;
	U16 bflag;
	U16 cmethod;
	U16 mt;
	U16 md;
	U32 crc32;
	U32 csize;
	U32 ucsize;
	U16 fnlen;
	U16 extlen;
};


struct AEndData
{
	U32 sign;
	U16 dnum;
	U16 dnumstart;
	U16 dirnumondisk;
	U16 dirnum;
	U32 dirlen;
	U32 diroff;
	U16 fcomment;
};


struct CDirDataExt
{
	CDirData data;
	size_t         size;
};

struct zFHDataExt
{
	zFHData data;
	char file_name[MAX_FILE_LENGHT];
};

#pragma pack()

struct ZFData
{
    S32 fileOffset;
	S32 comSize;
	S32 fileSize;
	S32 flags;
    U32 crc32;
    std::string	fileName;
};


typedef struct zFHData zFHeader;
typedef struct zFHDataExt zFHeaderExt;
typedef struct FileDescData   zFileDesc;
typedef struct CDirData zCDir;
typedef struct CDirDataExt zCDirExt;
typedef struct AEndData zArchiveEnd;
typedef struct ZFData    ZipFile;

class ReadFileInt
{
public:
	ReadFileInt(){};
	virtual ~ReadFileInt(){};
    
public:
    virtual void close()=0;
    virtual void seek(int offset, int where)=0;
    virtual int read(void* buffer, int size)=0;
    virtual int length()=0;
    virtual bool open(std::string filepath)=0;
};

extern zFHeaderExt *zGetFileHeader(ReadFileInt *file, size_t offset);
extern char *zGetFileContent(ReadFileInt *file, size_t offset, size_t c_size);
extern zArchiveEnd *zGetArchiveEnd(ReadFileInt *file);
extern zCDirExt *zGetCentralDir(ReadFileInt *file, size_t offset);
bool zUncompressBuffer(char *source, size_t source_len, char *dest, size_t dest_len);
bool zVerifyCrc32(unsigned int source_crc32, char *source, size_t len);
int zGetFileList(ReadFileInt *file, std::map<std::string, ZipFile> &lsfile);
bool zGetFileContent(ReadFileInt *file, ZipFile info, char* &unc_buffer, int &size);
#endif /* _ZIP_H_ */
