/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <syscall.h>
#include <sched.h>
#include <kmalloc.h>
#include <export.h>

static struct task *task_alloc()
{
    struct task *task;

    task = sched_task_create(NULL, 0);
    if (task)
        return NULL;

    if ()
        vmallo

    task->stack =

}

static struct task *task_copy()
{
    struct task *task;

    task = task_alloc();
    if (!task)
        return NULL;

    return task;
}

pid_t task_fork()
{
    struct task *task;

}

// long syscall_fork(void)
// {
//     return 0;
// }

// SYSCALL_ENTRY(SYSCALL_NR_FORK, syscall_fork, 0);
// EXPORT_SYMBOL(kernel_fork);

void __init fork_init(void)
{
    task_cache = kcache_create("task", sizeof(struct task), KCACHE_PANIC);
}
