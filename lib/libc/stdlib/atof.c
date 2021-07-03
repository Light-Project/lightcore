/*
 * libc/stdlib/atof.c
 */

#include <mm.h>
#include <xboot/module.h>

double atof(const char * nptr)
{
	return (double)strtod(nptr, 0);
}
EXPORT_SYMBOL(atof);
