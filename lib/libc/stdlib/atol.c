/*
 * libc/stdlib/atol.c
 */

#include <stddef.h>
#include <mm.h>
#include <xboot/module.h>

long atol(const char * nptr)
{
	return (long)strtol(nptr, NULL, 10);
}
EXPORT_SYMBOL(atol);
