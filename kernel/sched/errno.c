/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <state.h>
#include <sched.h>
#include <export.h>

state *sched_current_errno(void)
{
    return &current->errno_code;
}
EXPORT_SYMBOL(sched_current_errno);
