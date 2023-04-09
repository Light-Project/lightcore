/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>

state sys_brk(size_t brk)
{
    brk = page_align(brk);
}
