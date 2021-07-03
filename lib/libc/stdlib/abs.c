/*
 * libc/stdlib/abs.c
 */

#include <export.h>

int abs(int n)
{
	return ((n < 0) ? -n : n);
}
EXPORT_SYMBOL(abs);
