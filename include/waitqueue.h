/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _WAITQUEUE_H_
#define _WAITQUEUE_H_

#include <spinlock.h>
#include <list.h>

struct waitqueue_entry;
typedef state (*waitqueue_func_t)(struct waitqueue_entry *entry, void *pdata);

struct waitqueue_head {
    spinlock_t lock;
    struct list_head waits;
};

struct waitqueue_entry {
    struct list_head list;
    unsigned long flags;
    void *pdata;
};

#endif  /* _WAITQUEUE_H_ */
