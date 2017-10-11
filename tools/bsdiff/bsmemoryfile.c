#include <stdlib.h>
#include <memory.h>
#include "bsmemoryfile.h"

void bsmf_init(Bsmf*bsmf, void * dataPtr, int dataSize, BOOL ownData)
{
	memset(bsmf,0,sizeof(Bsmf));
	bsmf->buf = (char*)dataPtr;
	bsmf->length = dataSize;
	bsmf->buffsize = bsmf->length;
	bsmf->pos = 0;
	bsmf->ownData = ownData;
}
void bsmf_init2(Bsmf*bsmf, int memsize)
{
	memset(bsmf, 0, sizeof(Bsmf));
	bsmf_reAlloc(bsmf,memsize);
}
void bsmf_init3(Bsmf*bsmf, FILE* fptr)
{
	memset(bsmf, 0, sizeof(Bsmf));
	fseek(fptr,0,SEEK_END);
	long size = ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	bsmf->buf = (char*)malloc(size + 1);
	bsmf->length = size;
	bsmf->buffsize = bsmf->length + 1;
	bsmf->pos = 0;
	bsmf->ownData = TRUE;
	fread(bsmf->buf,size,1,fptr);
}
void bsmf_init4(Bsmf*bsmf, const char*filename)
{
	FILE* fptr = fopen(filename,"rb");
	memset(bsmf, 0, sizeof(Bsmf));
	if (fptr != NULL)
	{
		bsmf_init3(bsmf,fptr);
		fclose(fptr);
	}
}
void bsmf_free(Bsmf*bsmf)
{
	free(bsmf->buf);
	bsmf->buf = NULL;
	bsmf->length = 0;
	bsmf->buffsize = bsmf->length;
	bsmf->ownData = TRUE;
	bsmf->pos = 0;
}
int bsmf_getLength(Bsmf*bsmf)
{
	return bsmf->length;
}
char* bsmf_getBuffer(Bsmf*bsmf)
{
	return bsmf->buf;
}
char* bsmf_reAlloc(Bsmf*bsmf, int size)
{
	if (bsmf->ownData == TRUE)
	{
		free(bsmf->buf);
	}
	bsmf->length = size;
	bsmf->buffsize = bsmf->length;
	bsmf->pos = 0;
	bsmf->ownData = TRUE;
	bsmf->buf = (char*)malloc(size);
	bsmf_seek(bsmf,0,SEEK_SET);
	return bsmf->buf;
}
int bsmf_ensureBufferEnough(Bsmf*bsmf, int sizeToAdd)
{
	if (bsmf->pos + sizeToAdd > bsmf->length)
	{
		if (bsmf->pos + sizeToAdd > bsmf->buffsize)
		{
			char * pOldStream = bsmf_getBuffer(bsmf);
			int newAllocate = 2 * (bsmf->pos + sizeToAdd);
			bsmf->buf = (char*)malloc(newAllocate);
			memcpy(bsmf->buf, pOldStream, bsmf->length);
			bsmf->length = bsmf->pos + sizeToAdd;
			bsmf->buffsize = newAllocate;
			if (bsmf->ownData)
			{
				free(pOldStream);
			}
		}
		else
		{
			bsmf->length = bsmf->pos + sizeToAdd;
		}
	}
	return 1;
}
int bsmf_read(Bsmf*bsmf, void*outBuf, int size)
{
	int realSize = size > (bsmf->length - bsmf->pos) ? bsmf->length - bsmf->pos : size;
	if (realSize != size)
	{
	}
	memcpy(outBuf,bsmf->buf + bsmf->pos,realSize);
	bsmf->pos += realSize;
	return realSize;
}
int bsmf_write(Bsmf*bsmf, const char*data, int size)
{
	if (bsmf_ensureBufferEnough(bsmf,size))
	{
		memcpy(bsmf->buf+bsmf->pos,data,size);
		bsmf->pos += size;
		return size;
	}
	return -1;
}
int bsmf_seek(Bsmf*bsmf, int offset, int pos_type)
{
	if (pos_type == SEEK_CUR)
	{
		if (bsmf->pos + offset < bsmf->length)
		{
			bsmf->pos += offset;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (pos_type == SEEK_SET)
	{
		if (offset < bsmf->length)
		{
			bsmf->pos = offset;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (pos_type == SEEK_END)
	{
		if (offset <= bsmf->length)
		{
			bsmf->pos = bsmf->length - offset;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		printf("error param \n");
		return FALSE;
	}
}
int bsmf_tell(Bsmf*bsmf)
{
	return bsmf->pos;
}
int bsmf_isReachEnd(Bsmf*bsmf)
{
	return bsmf->pos >= bsmf->length;
}
int bsmf_isValid(Bsmf*bsmf)
{
	return bsmf->buf != NULL;
}
int bsmf_isOwnData(Bsmf*bsmf)
{
	return bsmf->ownData?1:0;
} 