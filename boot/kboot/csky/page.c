/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <asm/mmu.h>

void kernel_map(void)
{
    mmu_msa0_set(CONFIG_RAM_BASE | MSA_CACHE | MSA_DIRTY | MSA_VAILD);
}
