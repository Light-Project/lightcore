/*
 * libc/time/mktime.c
 */

#include <time.h>
#include <export.h>

time_t mktime(struct tm * tm)
{
	return __tm_to_time(tm);
}
EXPORT_SYMBOL(mktime);
