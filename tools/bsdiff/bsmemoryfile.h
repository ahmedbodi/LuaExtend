#ifndef BSMEMEORYFILE_KLSKLDKE_OKKLFJKE_KLKLJFJKLEKLJKL
#define BSMEMEORYFILE_KLSKLDKE_OKKLFJKE_KLKLJFJKLEKLJKL
#include <stdio.h>
#define BOOL int
#define TRUE 1
#define FALSE 0
struct bsmemoryfile
{
	char * buf;
	BOOL ownData;
	int length;
	int buffsize;
	int pos;
};
typedef struct bsmemoryfile Bsmf;
void bsmf_init(Bsmf*bsmf, void * dataPtr, int dataSize, BOOL ownData);
void bsmf_init2(Bsmf*bsmf, int memsize);
void bsmf_init3(Bsmf*bsmf, FILE* fptr);
void bsmf_init4(Bsmf*bsmf, const char*filename);
void bsmf_free(Bsmf*bsmf);
int bsmf_getLength(Bsmf*bsmf);
char*bsmf_getBuffer(Bsmf*bsmf);
char*bsmf_reAlloc(Bsmf*bsmf, int size);
int bsmf_ensureBufferEnough(Bsmf*bsmf, int sizeToAdd);
int bsmf_read(Bsmf*bsmf, void*outBuf, int size);
int bsmf_write(Bsmf*bsmf, const char*outBuf, int size);
int bsmf_seek(Bsmf*bsmf, int offset, int pos_type);
int bsmf_tell(Bsmf*bsmf);
int bsmf_isReachEnd(Bsmf*bsmf);
int bsmf_isValid(Bsmf*bsmf);
int bsmf_isOwnData(Bsmf*bsmf);
int bsmf_accessAble(Bsmf*bsmf);

#endif
