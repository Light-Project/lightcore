/*
 * libc/errno/errno.c
 */

#include <errno.h>
#include <export.h>

volatile int * __task_errno_location(void)
{
// 	return &(task_self()->__errno);
}
EXPORT_SYMBOL(__task_errno_location);
