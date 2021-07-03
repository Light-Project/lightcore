#ifndef _SYSTEM_REFCOUNT_H_
#define _SYSTEM_REFCOUNT_H_

#include <atomic.h>

typedef struct refcount_struct {
	atomic_t refs;
} refcount_t;

#endif