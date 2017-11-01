/*-
 * Copyright 2003-2005 Colin Percival
 * Copyright 2012 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "bspatch.h"
#include "bsmemoryfile.h"
static int64_t offtin(uint8_t *buf)
{
	int64_t y;

	y=buf[7]&0x7F;
	y=y*256;y+=buf[6];
	y=y*256;y+=buf[5];
	y=y*256;y+=buf[4];
	y=y*256;y+=buf[3];
	y=y*256;y+=buf[2];
	y=y*256;y+=buf[1];
	y=y*256;y+=buf[0];

	if(buf[7]&0x80) y=-y;

	return y;
}

int bspatch(const uint8_t* old, int64_t oldsize, uint8_t* new, int64_t newsize, struct bspatch_stream* stream)
{
	uint8_t buf[8];
	int64_t oldpos,newpos;
	int64_t ctrl[3];
	int64_t i;

	oldpos=0;newpos=0;
	while(newpos<newsize) {
		/* Read control data */
		for(i=0;i<=2;i++) {
			if (stream->read(stream, buf, 8))
				return -1;
			ctrl[i]=offtin(buf);
		};

		/* Sanity-check */
		if(newpos+ctrl[0]>newsize)
			return -1;

		/* Read diff string */
		if (stream->read(stream, new + newpos, (size_t)ctrl[0]))
			return -1;

		/* Add old data to diff string */
		for(i=0;i<ctrl[0];i++)
			if((oldpos+i>=0) && (oldpos+i<oldsize))
				new[newpos+i]+=old[oldpos+i];

		/* Adjust pointers */
		newpos+=ctrl[0];
		oldpos+=ctrl[0];

		/* Sanity-check */
		if(newpos+ctrl[1]>newsize)
			return -1;

		/* Read extra string */
		if (stream->read(stream, new + newpos, (size_t)ctrl[1]))
			return -1;

		/* Adjust pointers */
		newpos+=ctrl[1];
		oldpos+=ctrl[2];
	};

	return 0;
}
#define BSPATCH_FILE
#if defined(BSPATCH_FILE)


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "zlib.h"
static int bz2_read(const struct bspatch_stream* stream, void* buffer, int length)
{
#ifdef USE_MEMORY_STREAM
	int n;
	Bsmf* bz2;
	bz2 = (Bsmf*)stream->opaque;
	n = bsmf_read(bz2, buffer, length);
	//n = fread(buffer, 1, length, bz2);
	if (n != length)
		return -1;
	return 0;
#else
	int n;
	FILE* bz2;
	bz2 = (FILE*)stream->opaque;
	n = fread(buffer, 1, length, bz2);
	if (n != length)
		return -1;
	return 0;
#endif
}

static int loadziptobuf(char ** outbuf, int *outsize, const char * filename)
{
	FILE*pf;
	if ((pf = fopen(filename, "rb")) == NULL)
	{
		return -1;
	}
	int buffsize = 0;
	int zipbufsize = 0;
	
	if (fread(&buffsize, sizeof(int), 1, pf) != 1 || fread(&zipbufsize, sizeof(int), 1, pf) != 1)
	{
		fclose(pf);
		return -1;
	}
	char * zipbuf = (char*)malloc(zipbufsize);
	if (fread(zipbuf, zipbufsize, 1, pf) != 1)
	{
		free(zipbuf);
		fclose(pf);
	}
	*outbuf = (char*)malloc(buffsize+10000);
	*outsize = buffsize;
	int ret = uncompress(*outbuf, outsize, zipbuf, zipbufsize);
	if (ret == Z_BUF_ERROR)
	{
		free(*outbuf);
		*outbuf = (char*)malloc(buffsize*2);
		*outsize = buffsize;
		ret = uncompress(*outbuf, outsize, zipbuf, zipbufsize);
	}
	if (ret != Z_OK)
	{
		free(zipbuf);
		free(*outbuf);
		fclose(pf);
		return -1;
	}
	free(zipbuf);
	fclose(pf);		
	return 1;
}

int bspatch_file(const char * oldfile,const char* newfile ,const char* patchfile)
{
#ifdef USE_MEMORY_STREAM
	Bsmf f;
#else
	FILE*f;
#endif
	FILE * fd;
	uint8_t header[24];
	uint8_t *old, *new1;
	int64_t oldsize, newsize;
	 
	struct bspatch_stream stream;
#ifdef USE_MEMORY_STREAM
	char * buf; int bufsize;
	if (loadziptobuf(&buf, &bufsize, patchfile) != 1)
	{
		return -1;
	}
	bsmf_init(&f, buf, bufsize, 1);	
	if(bsmf_read(&f,header,24)!=24)
	{
		bsmf_free(&f);
		return -1;
	}
	if(memcmp(header,"ENDSLEY/BSDIFF43",16) != 0)
	{
		printf("Corrupt patch\n");
		bsmf_free(&f);
		return -1;
	}
	newsize = offtin(header+16);
	if(newsize < 0)
	{
		printf("Corrupt patch\n");
		bsmf_free(&f);
		return -1;
	}
#else
	/* Open patch file */
	if ((f = fopen(patchfile, "rb")) == NULL)
	{
		printf("fopen(%s) \n", patchfile);
		return -1;
	}
	/* Read header */
	if (fread(header, 1, 24, f) != 24) {		
		printf("fread(%s)\n", patchfile);
		return -1;
	}

	/* Check for appropriate magic */
	if (memcmp(header, "ENDSLEY/BSDIFF43", 16) != 0)
	{
		printf("Corrupt patch\n");
		return -1;
	}
	/* Read lengths from header */
	newsize=offtin(header+16);
	if (newsize < 0)
	{
		printf("Corrupt patch\n");
		return -1;
	}
#endif

	/* Close patch file and re-open it via libbzip2 at the right places */

	if ((fd = fopen(oldfile, "rb")) == NULL)
	{
#ifdef USE_MEMORY_STREAM
		bsmf_free(&f);
#endif
		printf("open file %s error!\n", oldfile);
		return -1;
	}
	fseek(fd, 0, SEEK_END);
	oldsize = ftell(fd);
	old = malloc((size_t)oldsize + 1);
	fseek(fd, 0, SEEK_SET);
	
	size_t r_size = fread(old, (size_t)oldsize, 1, fd);
	fclose(fd);
	if (r_size != 1)
	{
		free(old);
		printf("read %s error!\n", oldfile);
#ifdef USE_MEMORY_STREAM
		bsmf_free(&f);
#endif
		return -1;
	}

	if ((new1 = malloc((size_t)newsize + 1)) == NULL)
	{
		printf("malloc space error!!\n");
#ifdef USE_MEMORY_STREAM
		bsmf_free(&f);
#endif
		return -1;
	}
	
	stream.read = bz2_read;
#ifdef USE_MEMORY_STREAM
	stream.opaque = &f;
#else
	stream.opaque = f;
#endif
	if (bspatch(old, oldsize, new1, newsize, &stream))
	{
#ifdef USE_MEMORY_STREAM
		bsmf_free(&f);
#else
		fclose(f);
#endif
	
		free(old);
		free(new1);
		printf("bspatch\n");
		return -1;
	}
#ifdef USE_MEMORY_STREAM
	bsmf_free(&f);
#else
	fclose(f);
#endif


	/* Write the new file */
	if ((fd = fopen(newfile, "wb")) == NULL)
	{
		free(old);
		free(new1);
		printf("create file %s error!\n", newfile);
		return -1;
	}
	if (fwrite(new1, (size_t)newsize, 1, fd) != 1)
	{
		free(old);
		free(new1);
		fclose(fd);
		printf("write file %s error !\n", newfile);
		return -1;
	}
	fclose(fd);
	
	free(new1);
	free(old);

	return 0;
}

int bspatch_file_mem(unsigned char * old,size_t oldsize,const char* newfile ,const char* patchfile)
{
#ifdef USE_MEMORY_STREAM
	Bsmf f;
#else
	FILE*f;
#endif
	FILE * fd;
	uint8_t header[24];
	uint8_t *new1;
	int64_t  newsize;

	struct bspatch_stream stream;
#ifdef USE_MEMORY_STREAM
	char * buf; int bufsize;
	if (loadziptobuf(&buf, &bufsize, patchfile) != 1)
	{
		return -1;
	}
	bsmf_init(&f, buf, bufsize, 1);
	if (bsmf_read(&f, header, 24) != 24)
	{
		bsmf_free(&f);
		return -1;
	}
	if (memcmp(header, "ENDSLEY/BSDIFF43", 16) != 0)
	{
		printf("Corrupt patch\n");
		bsmf_free(&f);
		return -1;
	}
	newsize = offtin(header + 16);
	if (newsize < 0)
	{
		printf("Corrupt patch\n");
		bsmf_free(&f);
		return -1;
	}
#else
	/* Open patch file */
	if ((f = fopen(patchfile, "rb")) == NULL)
	{
		printf("fopen(%s) \n", patchfile);
		return -1;
	}
	/* Read header */
	if (fread(header, 1, 24, f) != 24) {
		printf("fread(%s)\n", patchfile);
		return -1;
	}

	/* Check for appropriate magic */
	if (memcmp(header, "ENDSLEY/BSDIFF43", 16) != 0)
	{
		printf("Corrupt patch\n");
		return -1;
	}
	/* Read lengths from header */
	newsize = offtin(header + 16);
	if (newsize < 0)
	{
		printf("Corrupt patch\n");
		return -1;
	}
#endif

	/* Close patch file and re-open it via libbzip2 at the right places */

	if ((new1 = malloc((size_t)newsize + 1)) == NULL)
	{
		printf("malloc space error!!\n");
#ifdef USE_MEMORY_STREAM
		bsmf_free(&f);
#endif
		return -1;
	}

	stream.read = bz2_read;
#ifdef USE_MEMORY_STREAM
	stream.opaque = &f;
#else
	stream.opaque = f;
#endif
	if (bspatch(old, oldsize, new1, newsize, &stream))
	{
#ifdef USE_MEMORY_STREAM
		bsmf_free(&f);
#else
		fclose(f);
#endif

		free(old);
		free(new1);
		printf("bspatch\n");
		return -1;
	}
#ifdef USE_MEMORY_STREAM
	bsmf_free(&f);
#else
	fclose(f);
#endif


	/* Write the new file */
	if ((fd = fopen(newfile, "wb")) == NULL)
	{
		free(old);
		free(new1);
		printf("create file %s error!\n", newfile);
		return -1;
	}
	if (fwrite(new1, (size_t)newsize, 1, fd) != 1)
	{
		free(old);
		free(new1);
		fclose(fd);
		printf("write file %s error !\n", newfile);
		return -1;
	}
	fclose(fd);

	free(new1);
	free(old);

	return 0;
}

#ifdef BSPATCH_EXE
int main(int argc, char * argv[])
{

	return bspatch_file(argv[1],argv[2],argv[3]);
}
#endif
#endif
