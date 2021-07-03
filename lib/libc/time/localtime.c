/*
 * libc/time/localtime.c
 */

#include <time.h>
#include <export.h>

struct tm * localtime(const time_t * t)
{
	return gmtime(t);
}
EXPORT_SYMBOL(localtime);
