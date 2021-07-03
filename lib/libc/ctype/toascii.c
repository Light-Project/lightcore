/*
 * libc/ctype/toascii.c
 */

#include <ctype.h>
#include <export.h>

int toascii(int c)
{
	return (c & 0x7f);
}
EXPORT_SYMBOL(toascii);
