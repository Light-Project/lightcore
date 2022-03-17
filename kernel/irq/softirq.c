/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "softirq"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <linkage.h>
#include <softirq.h>

#define SOFTIRQ_RESTART_NR 16
static struct idr *softirq_idr;

asmlinkage __visible __softirq_entry void softirq_handle(void)
{
    unsigned int max_restart = SOFTIRQ_RESTART_NR;
    struct irq_cpustat *stat = thiscpu_ptr(&irq_stat);
    unsigned long *pending = stat->softirq_pending;
    unsigned int softirq_bit;

restart:
    irq_local_enable();

    while ((softirq_bit = find_first_bit(pending, CONFIG_SOFTIRQ_LIMIT)) != ULONG_MAX) {
        struct softirq *softirq;

        bit_clr(pending, softirq_bit);
        softirq = idr_find(softirq_idr, softirq_bit);
        softirq->entry(softirq->pdata);
    }

    irq_local_disable();

    if (!bitmap_empty(stat->softirq_pending, CONFIG_SOFTIRQ_LIMIT)) {
        if (max_restart--)
            goto restart;
    }
}

void noinline softirq_entry(void)
{

}

void noinline softirq_exit(void)
{
    softirq_handle();
}

static inline void raise_softirq(unsigned int irqnr)
{
    struct irq_cpustat *stat = thiscpu_ptr(&irq_stat);
    bit_set(stat->softirq_pending, irqnr);
}

static inline void decline_softirq(unsigned int irqnr)
{
    struct irq_cpustat *stat = thiscpu_ptr(&irq_stat);
    bit_clr(stat->softirq_pending, irqnr);
}

/**
 * softirq_pending - pending a softirq
 * @irq: the softirq to pending
 */
state softirq_pending(struct softirq *irq)
{
    irq_local_disable();
    raise_softirq(irq->node.index);
    irq_local_enable();
    return -ENOERR;
}

/**
 * softirq_clear - clear a softirq
 * @irq: the softirq to clear
 */
state softirq_clear(struct softirq *irq)
{
    irq_local_disable();
    decline_softirq(irq->node.index);
    irq_local_enable();
    return -ENOERR;
}

/**
 * softirq_regsiter - regsiter a softirq
 * @irq: the softirq to regsiter
 */
state softirq_regsiter(struct softirq *irq)
{
    unsigned long id;

    id = idr_node_alloc(softirq_idr, &irq->node, irq);
    if (id == IDR_NONE)
        return -ENOMEM;

    return -ENOERR;
}

/**
 * softirq_unregister - unregsiter a softirq
 * @irq: the softirq to unregsiter
 */
void softirq_unregister(struct softirq *irq)
{
    idr_node_free(softirq_idr, irq->node.index);
}

void __init softirq_init(void)
{
    softirq_idr = idr_create(0);
}
