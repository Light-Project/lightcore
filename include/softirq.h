/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SOFTIRQ_H_
#define _SOFTIRQ_H_

#include <irqtypes.h>
#include <idr.h>

struct softirq {
    struct idr_node node;
    const char *name;
    void (*entry)(void *pdata);
    void *pdata;
};

#define SOFTIRQ_INITIALIZER(_name, _entry, _pdata) \
    (struct softirq) { .name = (_name), .entry = (_entry), .pdata = (_pdata) }

#define DEFINE_SOFTIRQ(name, entry, pdata) \
    struct softirq name = SOFTIRQ_INITIALIZER(#name, entry, pdata)

extern state softirq_pending(struct softirq *irq);
extern state softirq_clear(struct softirq *irq);
extern state softirq_regsiter(struct softirq *irq);
extern void softirq_unregister(struct softirq *irq);
extern void __init softirq_init(void);

#endif  /* _SOFTIRQ_H_ */
