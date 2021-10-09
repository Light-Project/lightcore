/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/e820.h>

void __section(".startup") memdetect(void)
{

    asm("jmp to_protected_mode");
}
