/*-
 * Copyright 2003-2005 Colin Percival
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

#if 0
__FBSDID("$FreeBSD: src/usr.bin/bsdiff/bspatch/bspatch.c,v 1.1 2005/08/06 01:59:06 cperciva Exp $");
#endif
 
#include "bspatch.h"
#include "zlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "bscommon.h"

/* Compatibility layer for reading either the old BSDIFF40 or the new BSDIFN40
   patch formats: */
 

int loadziptobuf(char ** outbuf, int *outsize, const char * filename)
{
	FILE*pf;
	if ((pf = fopen(filename, "rb")) == NULL)
	{
		return -1;
	}
	int buffsize = 0;
	size_t zipbufsize = 0;

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
	char * tmp = (char*)malloc(buffsize + 1000);
	size_t osize = buffsize + 1000;
	int ret = uncompress(tmp, &osize, zipbuf, zipbufsize);
	if (ret == Z_BUF_ERROR)
	{
		free(tmp);
		tmp = (char*)malloc(buffsize * 2);
		osize = buffsize * 2;
		ret = uncompress(tmp, &osize, zipbuf, zipbufsize);
	}
	if (ret != Z_OK)
	{
		free(zipbuf);
		free(*outbuf);
		fclose(pf);
		return -1;
	}
	(*outbuf) = tmp;
	*outsize = buffsize;
	free(zipbuf);
	fclose(pf);
	return 1; 
}
 
#ifndef u_char
typedef unsigned char u_char;
#endif

static off_t offtin(u_char *buf)
{
    off_t y;

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
int bspatch(const uint8_t* old, int64_t oldsize, uint8_t* new1, int64_t newsize, uint8_t* patch, int64_t patchsize)
{
	Bsmf  f, cpf, dpf, epf;
	Bsmf* cstream = NULL, *dstream = NULL, *estream = NULL;
	 
	long bzctrllen = 0, bzdatalen = 0;
	u_char header[32] = { 0 }, buf[8] = { 0 };
	off_t oldpos = 0, newpos = 0;
	off_t ctrl[3] = { 0 };
	off_t lenread = 0;
	off_t i = 0;
	int exitstatus = -1;
	 
	char*bufd = patch; int bufsize = patchsize;	
	/* Open patch file */
	bsmf_init(&f, bufd, bufsize, 0);


	/*
	File format:
	0   8   "BSDIFF40" (bzip2) or "BSDIFN40" (raw)
	8   8   X
	16  8   Y
	24  8   sizeof(newfile)
	32  X   bzip2(control block)
	32+X    Y   bzip2(diff block)
	32+X+Y  ??? bzip2(extra block)
	with control block a set of triples (x,y,z) meaning "add x bytes
	from oldfile to x bytes from the diff block; copy y bytes from the
	extra block; seek forwards in oldfile by z bytes".
	*/

	/* Read header */
	if (bsmf_read(&f, header, 32) != 32) {
		printf("fread(%s)", "patchfile");
		goto cleanup;
	}

	/* Check for appropriate magic */
	if (memcmp(header, "BSDIFN40", 8) == 0)
	{
	}
	else {
		printf("Corrupt patch\n");
		goto cleanup;
	}

	/* Read lengths from header */
	bzctrllen = offtin(header + 8);
	bzdatalen = offtin(header + 16);
	off_t newsize1 = offtin(header + 24);
	if (newsize != newsize1)
	{
		printf("Corrupt param of newsize\n");
		goto cleanup;
	}
	if ((bzctrllen<0) || (bzdatalen<0) || (newsize<0)) {
		printf("Corrupt patch\n");
		goto cleanup;
	}

	/* Close patch file and re-open it via libbzip2 at the right places */
	bsmf_free(&f);
	bsmf_init(&cpf, bufd, bufsize, 0);
	if (bsmf_seek(&cpf, 32, SEEK_SET) == FALSE) {
		printf("fseeko(%s, %lld)", "patchfile",
			(long long)32);
		goto cleanup;
	}
	cstream = &cpf;


	bsmf_init(&dpf, bufd, bufsize, 0);
	if (bsmf_seek(&dpf, 32 + bzctrllen, SEEK_SET) == FALSE) {
		printf("fseeko(%s, %lld)", "patchfile",
			(long long)(32 + bzctrllen));
		goto cleanup;
	}
	dstream = &dpf;

	bsmf_init(&epf, bufd, bufsize, 0);
	if (bsmf_seek(&epf, 32 + bzctrllen + bzdatalen, SEEK_SET) == FALSE) {
		printf("fseeko(%s, %lld)", "patchfile",
			(long long)(32 + bzctrllen + bzdatalen));
		goto cleanup;
	}
	estream = &epf;

	oldpos = 0; newpos = 0;
	while (newpos<newsize) {
		/* Read control data */
		for (i = 0; i <= 2; i++) {
			lenread = bsmf_read(cstream, buf, 8);
			if (lenread < 8) {
				printf("Corrupt patch\n");
				goto cleanup;
			}
			ctrl[i] = offtin(buf);
		};

		/* Sanity-check */
		if (newpos + ctrl[0]>newsize) {
			printf("Corrupt patch\n");
			goto cleanup;
		}

		/* Read diff string */
		lenread = bsmf_read(dstream, new1 + newpos, ctrl[0]);
		if (lenread < 0 || lenread < ctrl[0]) {
			printf("Corrupt patch\n");
			goto cleanup;
		}

		/* Add old data to diff string */
		for (i = 0; i<ctrl[0]; i++)
			if ((oldpos + i >= 0) && (oldpos + i<oldsize))
				new1[newpos + i] += old[oldpos + i];

		/* Adjust pointers */
		newpos += ctrl[0];
		oldpos += ctrl[0];

		/* Sanity-check */
		if (newpos + ctrl[1]>newsize) {
			printf("Corrupt patch\n");
			goto cleanup;
		}

		/* Read extra string */
		lenread = bsmf_read(estream, new1 + newpos, ctrl[1]);
		if (lenread < 0 || lenread < ctrl[1]) {
			printf("Corrupt patch\n");
			goto cleanup;
		}

		/* Adjust pointers */
		newpos += ctrl[1];
		oldpos += ctrl[2];
	};

	/* Clean up the bzip2 reads */
	cstream = NULL;
	dstream = NULL;
	estream = NULL;

	bsmf_free(&cpf);
	bsmf_free(&dpf);
	bsmf_free(&epf);
	exitstatus = 0;
cleanup:	
	bsmf_free(&cpf);	
	return exitstatus;
}
int bspatch_file(const char * oldfile, const char* newfile, const char* patchfile)
{
	Bsmf  f, cpf, dpf,epf;
	Bsmf* cstream = NULL, *dstream = NULL, *estream = NULL;
	long oldsize = 0, newsize = 0;
	long bzctrllen = 0, bzdatalen = 0;
	u_char header[32] = { 0 }, buf[8] = { 0 };
	u_char *old = NULL, *new1 = NULL;
	off_t oldpos = 0, newpos = 0;
	off_t ctrl[3] = { 0 };
	off_t lenread = 0;
	off_t i = 0;	
	int exitstatus = -1;
	FILE* fnew = NULL;
	char*bufd; int bufsize;
	if (loadziptobuf(&bufd, &bufsize, patchfile) != 1)
	{
		return -1;
	}
	/* Open patch file */
	bsmf_init(&f, bufd, bufsize, 0);
    /* Read header */
	if (bsmf_read(&f,header, 32) != 32) {
		printf("fread(%s)", patchfile);
		goto cleanup;
	}

	/* Check for appropriate magic */
	if (memcmp(header, "BSDIFN40", 8) == 0)
	{
	}
	else {
		printf("Corrupt patch\n");
		goto cleanup;
	}

	/* Read lengths from header */
	bzctrllen = offtin(header + 8);
	bzdatalen = offtin(header + 16);
	newsize = offtin(header + 24);
	if ((bzctrllen<0) || (bzdatalen<0) || (newsize<0)) {
		printf("Corrupt patch\n");
		goto cleanup;
	}

	/* Close patch file and re-open it via libbzip2 at the right places */
	bsmf_free(&f);


	off_t size = 0;
	old = readfile(oldfile, &size);
	if (old == NULL) {
		printf("old file: %s", oldfile);
		goto cleanup;
	}

	oldsize = size;

	if ((new1 = malloc((size_t)newsize + 1)) == NULL) {
		printf("Failed to allocate memory for new");
		goto cleanup;
	}

	exitstatus = bspatch(old, oldsize, new1, newsize, bufd, bufsize);
	if (exitstatus != 0)
	{
		printf("bapatch failed:");
		goto cleanup;
	}
	/* Write the new file */
	fnew = fopen(newfile, "wb");
	if (fnew == NULL) {
		printf("failed to write new file: %s", newfile);
		goto cleanup;
	}

	if (fwrite(new1, 1, (size_t)newsize, fnew) < (size_t)newsize) {
		printf("failed to write to new file: %s", newfile);
		goto cleanup;
	}

	if (fclose(fnew) != 0) {
		printf("failed to close new file: %s", newfile);
		fnew = NULL;
		goto cleanup;
	}
	fnew = NULL;
	exitstatus = 0;
cleanup:
	free(new1);
	free(old);
	free(bufd);	
	if (fnew != NULL)
		fclose(fnew);
	return exitstatus;
}


int bspatch_file_mem(unsigned char * old, size_t oldsize, const char* newfile, const char* patchfile)
{
	Bsmf  f, cpf, dpf, epf;
	Bsmf* cstream = NULL, *dstream = NULL, *estream = NULL;
	long newsize = 0;
	long bzctrllen = 0, bzdatalen = 0;
	u_char header[32] = { 0 }, buf[8] = { 0 };
	u_char *new1 = NULL;
	off_t oldpos = 0, newpos = 0;
	off_t ctrl[3] = { 0 };
	off_t lenread = 0;
	off_t i = 0;
	int exitstatus = -1;
	FILE* fnew = NULL;
	char*bufd; int bufsize;
	if (loadziptobuf(&bufd, &bufsize, patchfile) != 1)
	{
		return -1;
	}
	/* Open patch file */
	bsmf_init(&f, bufd, bufsize, 0);
	/* Read header */
	if (bsmf_read(&f, header, 32) != 32) {
		printf("fread(%s)", patchfile);
		goto cleanup;
	}

	/* Check for appropriate magic */
	if (memcmp(header, "BSDIFN40", 8) == 0)
	{
	}
	else {
		printf("Corrupt patch\n");
		goto cleanup;
	}

	/* Read lengths from header */
	bzctrllen = offtin(header + 8);
	bzdatalen = offtin(header + 16);
	newsize = offtin(header + 24);
	if ((bzctrllen<0) || (bzdatalen<0) || (newsize<0)) {
		printf("Corrupt patch\n");
		goto cleanup;
	}

	/* Close patch file and re-open it via libbzip2 at the right places */
	bsmf_free(&f);


	off_t size = 0;

	if ((new1 = malloc((size_t)newsize + 1)) == NULL) {
		printf("Failed to allocate memory for new");
		goto cleanup;
	}

	exitstatus = bspatch(old, oldsize, new1, newsize, bufd, bufsize);
	if (exitstatus != 0)
	{
		printf("bapatch failed:");
		goto cleanup;
	}
	/* Write the new file */
	fnew = fopen(newfile, "wb");
	if (fnew == NULL) {
		printf("failed to write new file: %s", newfile);
		goto cleanup;
	}

	if (fwrite(new1, 1, (size_t)newsize, fnew) < (size_t)newsize) {
		printf("failed to write to new file: %s", newfile);
		goto cleanup;
	}

	if (fclose(fnew) != 0) {
		printf("failed to close new file: %s", newfile);
		fnew = NULL;
		goto cleanup;
	}
	fnew = NULL;
	exitstatus = 0;
cleanup:
	free(new1);
	free(old);
	free(bufd);
	if (fnew != NULL)
		fclose(fnew);
	return exitstatus;
}