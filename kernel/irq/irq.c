/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <mm.h>
#include <irq.h>
#include <driver/irqchip.h>
#include <printk.h>

#include <asm/irq.h>

struct irq irq_map[IRQ_NR_MAX];

void irq_handle(irqnr_t vector)
{
    struct irq *handler = &irq_map[vector];

    if(handler->handler)
        handler->handler(vector, handler->data);
}

state irq_request(irqnr_t vector, enum irq_flags flags,
            irq_handler_t handler, void *data, const char *name)
{
    if(irq_map[vector].handler)
        return -EINVAL;

    irq_map[vector].name = name;
    irq_map[vector].handler = handler;
    irq_map[vector].data = data;

    return -ENOERR;
}

void irq_free(irqnr_t vector)
{
    irq_map[vector].handler = NULL;
}
