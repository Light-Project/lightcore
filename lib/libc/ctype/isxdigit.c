/*
 * libc/ctype/isxdigit.c
 */

#include <ctype.h>
#include <export.h>

int isxdigit(int c)
{
	return isdigit(c) || (((unsigned)c | 32) - 'a' < 6);
}
EXPORT_SYMBOL(isxdigit);
