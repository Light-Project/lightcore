/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "irqchip: " fmt

#include <init/initcall.h>
#include <driver/irqchip.h>
#include <printk.h>

state irqchip_pass(irqnr_t vector)
{
    return -ENOERR;
}

state irqchip_register(struct irqchip_device *idev)
{

    if(!idev || !idev->ops)
        return -EINVAL;



    return -ENOERR;
}

void __init irqchip_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;
 
    initcall_for_each_fn(fn, irqchip_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }
}
