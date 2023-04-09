/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <asm/cache.h>
#include <asm/pgtable.h>
#include <irqflags.h>

void arch_setup(void)
{
    arch_page_setup();
}
