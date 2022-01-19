/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <irq.h>
#include <printk.h>

void __weak irq_undefine_ack(irqnr_t vector)
{
    pr_crit("unexpected handle %ld can't ack\n", vector);
}

irqreturn_t irq_undefine_handle(irqnr_t vector, void *data)
{
    irq_undefine_ack(vector);
    return IRQ_RET_HANDLED;
}
