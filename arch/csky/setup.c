/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/pgtable.h>
#include <asm/irq.h>

void arch_setup(void)
{
    // arch_page_setup();
    arch_irq_init();
}
