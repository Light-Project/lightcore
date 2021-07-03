/*
 * libc/stdlib/labs.c
 */

#include <mm.h>
#include <xboot/module.h>

long labs(long n)
{
	return ((n < 0L) ? -n : n);
}
EXPORT_SYMBOL(labs);
