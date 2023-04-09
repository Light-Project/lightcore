/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <irq.h>
#include <entry.h>
#include <asm/idt.h>

DEFINE_IDTENTRY_IRQ(generic_interrupt)
{
    irq_handle(vector);
}
