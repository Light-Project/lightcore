/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kasan.h>
#include <printk.h>
#include <export.h>

bool kasan_report(uintptr_t addr, size_t size, bool write, uintptr_t rip)
{
    return false;
}

void __init kasan_init(void)
{

}
