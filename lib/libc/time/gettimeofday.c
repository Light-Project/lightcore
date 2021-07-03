/*
 * libc/time/gettimeofday.c
 */

#include <time.h>
#include <export.h>

int gettimeofday(struct timeval * tv, void * tz)
{
    if(!tv)
        return -1;

}
EXPORT_SYMBOL(gettimeofday);
