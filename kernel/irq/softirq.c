/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "softirq"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <linkage.h>
#include <softirq.h>
#include <bottom-half.h>
#include <export.h>
#include <crash.h>

#define SOFTIRQ_LIMIT CONFIG_SOFTIRQ_LIMIT
static struct idr_root *softirq_idr;

static inline void bh_handle_entry(void)
{
    bh_local_enable_count(SOFTIRQ_OFFSET);
}

static inline void bh_handle_exit(void)
{
    bh_local_disable_count(SOFTIRQ_OFFSET);
    BUG_ON(in_hardirq());
}

asmlinkage __visible __softirq_entry void softirq_irq_entry(void)
{
    struct irq_cpustat *stat = thiscpu_ptr(&irq_stat);
    unsigned long *pending = stat->softirq_pending;
    unsigned int softirq_bit;

    bh_handle_entry();

    while ((softirq_bit = find_first_bit(pending, SOFTIRQ_LIMIT)) < SOFTIRQ_LIMIT) {
        struct softirq *softirq;

        softirq = idr_find(softirq_idr, softirq_bit + 1);
        if (BUG_ON(!softirq))
            continue;

        if (!softirq_test_periodic(softirq))
            bit_clr(pending, softirq_bit);

        if (softirq_test_irq_safe(softirq))
            softirq->entry(softirq->pdata);
        else {
            irq_local_enable();
            softirq->entry(softirq->pdata);
            irq_local_disable();
        }
    }

    bh_handle_exit();
}

void noinline softirq_handle(void)
{
    struct irq_cpustat *stat = thiscpu_ptr(&irq_stat);
    unsigned long *pending = stat->softirq_pending;

    if (!in_irq() && !bitmap_empty(pending, SOFTIRQ_LIMIT))
        softirq_irq_entry();
}

static inline void raise_softirq(unsigned int irqnr)
{
    struct irq_cpustat *stat = thiscpu_ptr(&irq_stat);
    bit_set(stat->softirq_pending, irqnr - 1);
}

static inline void decline_softirq(unsigned int irqnr)
{
    struct irq_cpustat *stat = thiscpu_ptr(&irq_stat);
    bit_clr(stat->softirq_pending, irqnr - 1);
}

/**
 * softirq_pending - pending a softirq
 * @irq: the softirq to pending
 */
state softirq_pending(struct softirq *irq)
{
    irqflags_t irqflags;

    if (BUG_ON(!irq || !irq->entry))
        return -EINVAL;

    irqflags = irq_local_save();
    raise_softirq(irq->node.index);
    irq_local_restore(irqflags);

    return -ENOERR;
}
EXPORT_SYMBOL(softirq_pending);

/**
 * softirq_clear - clear a softirq
 * @irq: the softirq to clear
 */
void softirq_clear(struct softirq *irq)
{
    irqflags_t irqflags;

    if (BUG_ON(!irq))
        return;

    irqflags = irq_local_save();
    decline_softirq(irq->node.index);
    irq_local_restore(irqflags);
}
EXPORT_SYMBOL(softirq_clear);

/**
 * softirq_register - register a softirq
 * @irq: the softirq to register
 */
state softirq_register(struct softirq *irq)
{
    unsigned long id;

    if (BUG_ON(!irq || !irq->entry))
        return -EINVAL;

    id = idr_node_alloc_cyclic_max(softirq_idr, &irq->node, irq, SOFTIRQ_LIMIT);
    if (id == IDR_NONE)
        return -ENOMEM;

    return -ENOERR;
}
EXPORT_SYMBOL(softirq_register);

/**
 * softirq_unregister - unregister a softirq.
 * @irq: the softirq to unregister.
 */
void softirq_unregister(struct softirq *irq)
{
    softirq_clear(irq);
    idr_node_free(softirq_idr, irq->node.index);
}
EXPORT_SYMBOL(softirq_unregister);

void __init softirq_init(void)
{
    softirq_alloc_init();
    softirq_idr = idr_create(0);
}
