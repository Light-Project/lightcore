/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <syscall.h>
#include <linkage.h>
#include <stddef.h>
#include <spinlock.h>
#include <export.h>
#include <panic.h>

static struct syscall_entry syscall_table[SYSCALL_NR_MAX] = {
    [0 ... SYSCALL_NR_MAX - 1] = {
        .name = "undefine",
        .args = UINT_MAX,
        .entry = syscall_undefine_handle,
    },
};

long syscall_undefine_handle(void)
{
    pr_info("unexpected syscall handle\n");
    return -EINVAL;
}

asmlinkage long syscall_handle(unsigned int call_num, ...)
{
    struct syscall_entry *syscall = &syscall_table[call_num];
    long retval, arg1, arg2, arg3, arg4, arg5, arg6;
    va_list args;

    if (call_num >= SYSCALL_NR_MAX || syscall->args > 6)
        return syscall_undefine_handle();

    va_start(args, call_num);

    if (syscall->args > 0)
        arg1 = va_arg(args, long);

    if (syscall->args > 1)
        arg2 = va_arg(args, long);

    if (syscall->args > 2)
        arg3 = va_arg(args, long);

    if (syscall->args > 3)
        arg4 = va_arg(args, long);

    if (syscall->args > 4)
        arg5 = va_arg(args, long);

    if (syscall->args > 5)
        arg6 = va_arg(args, long);

    va_end(args);

    switch (syscall->args) {
        case 0:
            retval = ((long (*)())
                (syscall->entry))();
            break;

        case 1:
            retval = ((long (*)(long))
                (syscall->entry))(arg1);
            break;

        case 2:
            retval = ((long (*)(long, long))
                (syscall->entry))(arg1, arg2);
            break;

        case 3:
            retval = ((long (*)(long, long, long))
                (syscall->entry))(arg1, arg2, arg3);
            break;

        case 4:
            retval = ((long (*)(long, long, long, long))
                (syscall->entry))(arg1, arg2, arg3, arg4);
            break;

        case 5:
            retval = ((long (*)(long, long, long, long, long))
                (syscall->entry))(arg1, arg2, arg3, arg4, arg5);
            break;

        case 6:
            retval = ((long (*)(long, long, long, long, long, long))
                (syscall->entry))(arg1, arg2, arg3, arg4, arg5, arg6);
            break;
    }

    return retval;
}

state syscall_register(unsigned int callnr, const char *name, unsigned int args, void *entry)
{
    if (syscall_table[callnr].args != UINT_MAX) {
        pr_crit("callnr (%u) already registered\n", callnr);
        return -EBUSY;
    }

    syscall_table[callnr].name = name;
    syscall_table[callnr].args = args;
    syscall_table[callnr].entry = entry;

    return -ENOERR;
}
EXPORT_SYMBOL(syscall_register);

void syscall_unregister(unsigned int callnr)
{
    if (syscall_table[callnr].args == UINT_MAX)
        return;

    syscall_table[callnr].name = "undefine";
    syscall_table[callnr].args = UINT_MAX;
    syscall_table[callnr].entry = syscall_undefine_handle;
}
EXPORT_SYMBOL(syscall_unregister);
