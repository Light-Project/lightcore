/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SOFTIRQ_H_
#define _SOFTIRQ_H_

#include <idr.h>
#include <irqtypes.h>
#include <bitflags.h>

typedef void (*softirq_entry_t)(void *pdata);

enum sofirq_flags {
    __SOFTIRQ_IRQ_SAFE      = 0,
    __SOFTIRQ_PERIODIC      = 1,
};

#define SOFTIRQ_IRQ_SAFE    BIT(__SOFTIRQ_IRQ_SAFE)
#define SOFTIRQ_PERIODIC    BIT(__SOFTIRQ_PERIODIC)

/**
 * struct softirq - describe a softirq node.
 * @node: idr used to assign softirq index number.
 * @name: node name for management.
 * @flags: characteristics flags of nodes.
 * @entry: entry function of soft interrupt.
 * @pdata: parameters of the entry function.
 */
struct softirq {
    struct idr_node node;
    const char *name;
    unsigned long flags;
    softirq_entry_t entry;
    void *pdata;
};

#define SOFTIRQ_INITIALIZER(_name, _entry, _pdata, _flags) \
    (struct softirq) { .name = (_name), .entry = (_entry), .pdata = (_pdata), .flags = (_flags) }

#define DEFINE_SOFTIRQ(name, entry, pdata, flags) \
    struct softirq name = SOFTIRQ_INITIALIZER(#name, entry, pdata, flags)

GENERIC_STRUCT_BITOPS(softirq, struct softirq, flags, false)
GENERIC_STRUCT_FLAG(softirq, struct softirq, flags, irq_safe, __SOFTIRQ_IRQ_SAFE)
GENERIC_STRUCT_FLAG(softirq, struct softirq, flags, periodic, __SOFTIRQ_PERIODIC)

extern state softirq_pending(struct softirq *irq);
extern void softirq_clear(struct softirq *irq);
extern state softirq_register(struct softirq *irq);
extern void softirq_unregister(struct softirq *irq);
extern struct softirq *softirq_create(const char *name, softirq_entry_t entry, void *pdata, unsigned long flags);
extern void softirq_destroy(struct softirq *irq);
extern void __init softirq_init(void);

#endif  /* _SOFTIRQ_H_ */
