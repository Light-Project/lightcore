/*
 * libc/ctype/isspace.c
 */

#include <ctype.h>
#include <export.h>

int isspace(int c)
{
	return (c == ' ') || ((unsigned)c - '\t' < 5);
}
EXPORT_SYMBOL(isspace);
