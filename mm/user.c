/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>

state sys_brk(size_t brk)
{
    brk = page_align(brk);
}
