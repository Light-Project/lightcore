/*
 * libc/ctype/isblank.c
 */

#include <ctype.h>
#include <export.h>

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
EXPORT_SYMBOL(isblank);
