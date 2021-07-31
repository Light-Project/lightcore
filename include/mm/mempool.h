/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel/spinlock.h>

typedef void *(mempool_alloc_t)();
typedef void *(mempool_free_t)();

struct mempool{
    spinlock_t lock;
    int curr_nr;            /* Current number of elements */
    int min_nr;             /* Number of elements */
	void **elements;        /* Pool pointer */

    void *pool_data;        /* private data */
    mempool_alloc_t *alloc; /* relevant alloc function */
    mempool_alloc_t *free;  /* relevant free function */
};