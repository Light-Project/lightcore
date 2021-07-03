/*
 * libc/ctype/isalnum.c
 */

#include <ctype.h>
#include <export.h>

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}
EXPORT_SYMBOL(isalnum);
