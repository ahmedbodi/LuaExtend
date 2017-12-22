/*
 *  bscommon.h
 *  Sparkle
 *
 *  Created by Mayur Pawashe on 5/16/16.
 */

#ifndef BS_COMMON_H
#define BS_COMMON_H

#include <sys/types.h>
#include <stdio.h>
# define USE_MEMORY_STREAM
# include <stddef.h>
# include <stdint.h>
#include "bsmemoryfile.h"
typedef unsigned char u_char;
u_char *readfile(const char *filename, off_t *outSize);

 

int bsdiff(const uint8_t* old, int64_t oldsize, const uint8_t* new, int64_t newsize, Bsmf * stream);
#endif
