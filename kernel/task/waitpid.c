/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>
#include <syscall.h>
#include <export.h>

struct task_wait_args {


};

state task_wait()
{

    return -ENOERR;
}

state kernel_wait(pid_t pid, int *stat)
{


    return -ENOERR;
}

long syscall_waitpid(pid_t pid, int *stat, int options)
{
    return kernel_wait(pid, stat);
}

EXPORT_SYMBOL(kernel_wait);
SYSCALL_ENTRY(SYSCALL_NR_WAITPID, syscall_waitpid, 3);
