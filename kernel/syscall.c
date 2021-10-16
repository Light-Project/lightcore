/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <stddef.h>
#include <syscall.h>
#include <export.h>

static struct syscall_entry *syscall_table[SYSCALL_NR_MAX];

void syscall_handle(unsigned int call_num, ...)
{

}

state syscall_register(unsigned int call_num, struct syscall_entry *syscall)
{
    /* Is it present? */
    if (syscall_table[call_num])
        return -EBUSY;

    if (!syscall->entry)
        return -EINVAL;

    syscall_table[call_num] = syscall;
    return -ENOERR;
}

void syscall_unregister(unsigned int call_num)
{
    syscall_table[call_num] = NULL;
}

EXPORT_SYMBOL(syscall_register);
EXPORT_SYMBOL(syscall_unregister);
