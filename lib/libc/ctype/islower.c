/*
 * libc/ctype/islower.c
 */

#include <ctype.h>
#include <export.h>

int islower(int c)
{
	return ((unsigned)c - 'a') < 26;
}
EXPORT_SYMBOL(islower);
