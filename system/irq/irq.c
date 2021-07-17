/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  system/irq/irq.c
 *
 *  Copyright (C) 2021 CopyrightName
 */

#include <string.h>
#include <mm.h>
#include <system/irq.h>
#include <driver/irqchip.h>
#include <printk.h>

#include <asm/irq.h>

struct irq irq_map[IRQ_NR_MAX];

void irq_handle(irqnr_t vector)
{
    irq_handler_t handler = irq_map[vector].handler;
    if(handler)
        handler(vector, irq_map[vector].data);
}

state irq_request(irqnr_t vector, enum irq_flags flags,
            irq_handler_t handler, void *data, const char *name)
{
    if(irq_map[vector].handler)
        return -EINVAL;

    irq_map[vector].name = name;
    irq_map[vector].handler = handler;
    
    irqchip_pass(vector);

    return -ENOERR;
}

void irq_free(irqnr_t nr)
{
    
}


void irq_enable(irqnr_t nr)
{

}
