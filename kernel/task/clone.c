/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>
#include <string.h>
#include <initcall.h>
#include <syscall.h>
#include <kmalloc.h>
#include <vmalloc.h>
#include <ncache.h>
#include <timekeeping.h>
#include <stackprotector.h>
#include <memory.h>
#include <proc.h>
#include <export.h>

#include "clone/memory.c"
#include "clone/stack.c"

static struct sched_task *task_alloc(struct task_clone_args *args)
{
    struct sched_task *child;
    void *stack;

    child = sched_task_create();
    if (!child)
        return NULL;

    stack = task_stack_alloc();
    if (!stack)
        goto err_task;

    memcpy(child, current, sizeof(*child));
    list_head_init(&child->child);

    child->stack = stack;
    task_stack_magic(child);

#ifdef CONFIG_STACKPROTECTOR
    child->stack_canary = random_canary();
#endif

    return child;

err_task:
    sched_task_destroy(child);
    return NULL;
}

static struct sched_task *task_copy(struct task_clone_args *args, struct pid *pid)
{
    enum clone_flags flags = args->flags;
    struct sched_task *child;
    state ret;

    child = task_alloc(args);
    if (!child)
        return NULL;

    ret = sched_task_clone(child, flags);
    if (unlikely(ret))
        goto err_sched_clone;

    ret = copy_memory(args, child);
    if (unlikely(ret))
        goto err_copy_mem;

    ret = proc_thread_copy(args, child);
    if (unlikely(ret))
        goto err_copy_proc;

    if (pid != &init_task_pid) {
        pid = pid_alloc(child->namespace->pid, args->tid, args->tid_size);
        if (unlikely(IS_INVAL(pid))) {
            ret = PTR_INVAL(pid);
            goto err_alloc_pid;
        }
    }

    child->start_time = timekeeping_get_time_ns();

    return child;

err_alloc_pid:
err_copy_proc:
err_copy_mem:
err_sched_clone:
    return ERR_PTR(ret);
}

pid_t task_clone(struct task_clone_args *args)
{
    struct sched_task *child;
    pid_t pid = 0;

    child = task_copy(args, NULL);
    if (unlikely(IS_INVAL(child)))
        return PTR_INVAL(child);

    sched_wake_up_new(child);

    return pid;
}

state kernel_clone(enum clone_flags flags, int (*fn)(void *), void *arg)
{
    struct task_clone_args args = {
        .flags = (flags | CLONE_VM) & ~CLONE_SIGNAL,
        .exit_signal = flags & CLONE_SIGNAL,
        .entry = fn,
        .arg = arg,
    };

    return task_clone(&args);
}
EXPORT_SYMBOL(kernel_clone);

DEFINE_SYSCALL0(fork)
{
#ifdef CONFIG_MMU
    struct task_clone_args args = {
        .exit_signal = SIGCHLD,
    };

    return task_clone(&args);
#else
    return -EINVAL;
#endif
}

DEFINE_SYSCALL0(vfork)
{
    struct task_clone_args args = {
        .flags = CLONE_VM | CLONE_VFORK,
        .exit_signal = SIGCHLD,
    };

    return task_clone(&args);
}

DEFINE_SYSCALL5(clone, unsigned long, clone_flags, unsigned long, newsp,
                int *, ptidptr, int *, ctidptr, unsigned long, tls)
{
    struct task_clone_args args = {
        .flags = clone_flags & ~CLONE_SIGNAL,
        .exit_signal = clone_flags & CLONE_SIGNAL,
        .stack = newsp,
    };

    return task_clone(&args);
}

void __init clone_init(void)
{
    clone_memory_init();
    clone_stack_init();
}
