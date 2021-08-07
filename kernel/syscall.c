/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <kernel/syscall.h>
#include <kernel/sched.h>

void *syscall_table[SYS_NR_MAX] = {
    [SYS_READ]  = sys_read,
    [SYS_WRITE] = sys_write,
    [SYS_OPEN]  = sys_read,
    [SYS_CLOSE] = sys_close,
};
