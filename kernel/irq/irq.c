/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "irq: " fmt

#include <irq.h>
#include <string.h>
#include <kmalloc.h>
#include <printk.h>

static struct kcache *channel_cache;

struct irq_desc irq_desc[IRQ_NR_MAX] = {
    [0 ... IRQ_NR_MAX - 1] = {
        .handler = irq_undefine_handle,
    },
};

void irq_handle(irqnr_t vector)
{
    struct irq_desc *channel = &irq_desc[vector];

    if (channel->handler)
        channel->handler(vector, channel->data);
}

state irq_request(irqnr_t vector, enum irq_flags flags, irq_handler_t handler,
                  void *data, const char *name)
{
    struct irq_desc *channel = &irq_desc[vector];

    channel->name = name;
    channel->handler = handler;
    channel->data = data;

    return -ENOERR;
}

void __init irq_init(void)
{
    channel_cache = kcache_create("irqchannel", sizeof(struct irq_desc), KCACHE_PANIC);
}
