#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "crc32.h"
#include "crc64.h"
#include "fcompare.h"
int main(int argc, const char* argv[])
{
	if (argc == 3)
	{
		//unsigned long crc32(const char * filename)
		if (strcmp(argv[2], "-CRC32") == 0)
		{
			unsigned long crcres = crc32(argv[1]);
			printf("%lu", crcres);
			return 0;
		}
		else if (strcmp(argv[2], "-CRC64") == 0)
		{
			uint64_t crcres = crc64(argv[1]);
			printf("%llu", crcres);
			return 0;
		}
		else 
		{
			return 1;
		}
	}
	else if (argc == 4)
	{
		if (strcmp(argv[3], "-COMP") == 0)
		{
			int ret = fcompare(argv[1], argv[2]);
			if (ret == 0)
				printf("same");
			else
				printf("different");
		}
		else
		{
			return 1;
		}
	}
	else
	{
		printf("please use: %s filename -CRC32 \n", argv[0]);
		printf("please use: %s filename -CRC64 \n", argv[0]);
		printf("please use: %s filename1 filename2 -COMP \n", argv[0]);
		return 1;
	}
	return 0;
}