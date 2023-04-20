/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IRQPOLL_H_
#define _IRQPOLL_H_

#include <list.h>
#include <bitflags.h>
#include <completion.h>

#define IRQPOLL_CAPACITY CONFIG_IRQPOLL_CAPACITY

struct irqpoll;
typedef unsigned int (*irqpoll_entry_t)(unsigned int weight, void *pdata);

enum irqpoll_flags {
    __IRQPOLL_QUEUE = 0,
    __IRQPOLL_KILL,

    IRQPOLL_QUEUE   = BIT(__IRQPOLL_QUEUE),
    IRQPOLL_KILL    = BIT(__IRQPOLL_KILL),
};

/**
 * struct irqpoll - describe a interrupt poller.
 */
struct irqpoll {
    struct list_head list;
    unsigned long flags;
    unsigned int capacity;
    struct completion comp;
    irqpoll_entry_t entry;
    void *pdata;
};

#define IRQPOLL_INIT(_capacity, _entry, _pdata) \
    (struct irqpoll){.capacity = (_capacity), .entry = (_entry), .pdata = (_pdata)}

#define DEFINE_IRQPOLL(name, capacity, entry, pdata) \
    struct irqpoll name = IRQPOLL_INIT(capacity, entry, pdata)

GENERIC_STRUCT_BITOPS(irqpoll, struct irqpoll, flags, false);
GENERIC_STRUCT_FLAG(irqpoll, struct irqpoll, flags, queue, __IRQPOLL_QUEUE);
GENERIC_STRUCT_FLAG(irqpoll, struct irqpoll, flags, kill, __IRQPOLL_KILL);

static inline void irqpoll_init(struct irqpoll *poll, unsigned int capacity,
                                irqpoll_entry_t entry, void *pdata)
{
    *poll = (struct irqpoll){};
    poll->capacity = capacity;
    poll->entry = entry;
    poll->pdata = pdata;
}

extern void irqpoll_enable(struct irqpoll *poll);
extern void irqpoll_disable(struct irqpoll *poll);
extern void irqpoll_complete(struct irqpoll *poll);
extern state irqpoll_enqueue(struct irqpoll *poll);
extern void irqpoll_setup(void);

#endif /* _IRQPOLL_H_ */
