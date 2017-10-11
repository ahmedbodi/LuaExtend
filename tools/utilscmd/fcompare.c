#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fcompare.h"
int fcompare(const char * fn1, const char * fn2)
{
	FILE* f1 = NULL;
	FILE* f2 = NULL;
	if ((f1 = fopen(fn1, "rb")) == NULL)
	{
		return 1;
	}
	if ((f2 = fopen(fn2, "rb")) == NULL)
	{
		fclose(f1);
		return 1;
	}
	fseek(f1, 0, SEEK_END);
	fseek(f2, 0, SEEK_END);
	size_t tf1 = ftell(f1);
	size_t tf2 = ftell(f2);
	fseek(f1, 0, SEEK_SET);
	fseek(f2, 0, SEEK_SET);

	if (tf1 != tf2)
	{
		fclose(f1);
		fclose(f2);
		return -1;
	}
	for (size_t i = 0; i < tf1; i ++)
	{
		int cf1 = fgetc(f1);
		int cf2 = fgetc(f2);
		if (cf1 != cf2)
		{		
			fclose(f1);
			fclose(f2);
			return -1;
		}
	}	
	fclose(f1);
	fclose(f2);
	return 0;
}