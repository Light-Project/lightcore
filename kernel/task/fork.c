/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <sched.h>
#include <string.h>
#include <initcall.h>
#include <syscall.h>
#include <kmalloc.h>
#include <vmalloc.h>
#include <export.h>

#include <asm/sched.h>
#include <asm/page.h>

void task_stack_magic(struct task *task)
{
    uint32_t *magic = task->stack;
    *magic = KMAGIC;
}

state __weak task_stuct_copy(struct task *dest, struct task *src)
{
    memcpy(dest, src, sizeof(*src));
    return -ENOERR;
}

static struct task *task_alloc(struct task *old)
{
    struct task *task;
    void *stack;

    task = sched_task_create(NULL, 0);
    if (task)
        return NULL;

    stack = vmalloc(THREAD_SIZE);
    if (!stack)
        goto err_task;

    if (task_stuct_copy(task, old))
        goto err_stack;

    task->stack = stack;
    task_stack_magic(task);

    return task;

err_stack:
    vfree(stack);
err_task:
    sched_task_destroy(task);
    return NULL;
}

static struct task *task_copy()
{
    struct task *task;

    task = task_alloc(current);
    if (!task)
        return NULL;

    return task;
}

pid_t task_fork()
{
    struct namespace *ns = current->ns;
    struct task *new;
    struct pid *pid;

    new = task_copy();

    pid_alloc_node(ns->pid, &new->pid);

    return new->pid.pid;
}

long syscall_fork(void)
{
    return 0;
}

void __init fork_init(void)
{

}

SYSCALL_ENTRY(SYSCALL_NR_FORK, syscall_fork, 0);
EXPORT_SYMBOL(task_fork);
