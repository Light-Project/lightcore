/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>
#include <string.h>
#include <initcall.h>
#include <syscall.h>
#include <ncache.h>
#include <kmalloc.h>
#include <vmalloc.h>
#include <timekeeping.h>
#include <stackprotector.h>
#include <memory.h>
#include <proc.h>

#define STACK_CACHE_NR  10
static struct ncache *stack_cache;
static struct kcache *memory_cache;

/***  Allocate and free stack  ***/

void task_stack_magic(struct sched_task *task)
{
    uint32_t *magic = stack_end(task->stack);
    *magic = KMAGIC;
}

bool task_stack_check(struct sched_task *task)
{
    uint32_t *magic = task->stack;
    return *magic == KMAGIC;
}

static void *task_stack_ncache_alloc(void *pdata)
{
#ifdef CONFIG_VMAP_STACK
    return vmalloc(THREAD_SIZE);
#else
    return get_free_pages(THREAD_ORDER, GFP_ACCOUNT);
#endif
}

static void task_stack_ncache_free(void *stack, void *pdata)
{
#ifdef CONFIG_VMAP_STACK
    vfree(stack);
#else
    free_page(stack);
#endif
}

void *task_stack_alloc(void)
{
    void *stack= ncache_get(stack_cache);

    if (!stack)
        return NULL;

    memset(stack, 0, THREAD_SIZE);
    return stack;
}
EXPORT_SYMBOL(task_stack_alloc);

void task_stack_free(void *stack)
{
    ncache_put(stack_cache, stack);
}
EXPORT_SYMBOL(task_stack_free);

/***  Allocate and init memory struct  ***/

static struct memory *task_memory_init(struct memory *mm)
{
    return mm;
}

static void task_memory_remove(struct memory *mm)
{

}

struct memory *task_memory_alloc(void)
{
    struct memory *mem;

    mem = kcache_zalloc(memory_cache, GFP_KERNEL);
    if (!mem)
        return NULL;

    return task_memory_init(mem);
}
EXPORT_SYMBOL(task_memory_alloc);

void task_memory_free(struct memory *mem)
{
    if (memory_put(mem))
        task_memory_remove(mem);
}
EXPORT_SYMBOL(task_memory_free);

/***  Allocate task and sub struct  ***/

static state copy_memory(struct task_clone_args *args, struct sched_task *child)
{
    enum clone_flags flags = args->flags;
    struct memory *mem, *omem;

    /* clone a kernel thread */
    if (!(omem = current->mem)) {
        child->mem = NULL;
        child->active_mem = NULL;
        return -ENOERR;
    }

    if (flags & CLONE_VM) {
        memory_get(omem);
        mem = omem;
    } else {
        mem = kcache_zalloc(memory_cache, GFP_KERNEL);
        if (!mem)
            return -ENOMEM;

        memcpy(mem, omem, sizeof(*mem));
    }

    child->mem = mem;
    child->active_mem = mem;

    return -ENOERR;
}

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
    stack_cache = ncache_create(task_stack_ncache_alloc,
        task_stack_ncache_free, STACK_CACHE_NR, NCACHE_PANIC, NULL);
    memory_cache = kcache_create("memory",
        sizeof(struct memory), KCACHE_PANIC);
}
