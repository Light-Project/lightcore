#ifndef _SYSTEM_KREF_H
#define _SYSTEM_KREF_H

#include <system/refcount.h>

typedef struct kref{
    refcount_t refcount; 
} kref_t;

#endif