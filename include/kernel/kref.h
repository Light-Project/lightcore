#ifndef _KERNEL_KREF_H
#define _KERNEL_KREF_H

#include <kernel/refcount.h>

typedef struct kref{
    refcount_t refcount; 
} kref_t;

#endif