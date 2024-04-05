#include "common.h"

#define DG_DYNARR_IMPLEMENTATION
#include <DG_dynarr.h>

char *loadfile(const char *filename)
{
	FILE *f = fopen(filename, "r");
	char *buf;
	long fsize;

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = malloc(fsize + 1);
	fread(buf, 1, fsize, f);
	buf[fsize] = '\0';

	fclose(f);
	return buf;
}
