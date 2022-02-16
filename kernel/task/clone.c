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
    void *stack = ncache_get(stack_cache);
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

static struct sched_task *task_alloc(struct task_clone_args *args, struct sched_task *curr)
{
    struct sched_task *child;
    void *stack;

    child = sched_task_create();
    if (child)
        return NULL;

    stack = task_stack_alloc();
    if (!stack)
        goto err_task;

    child->stack = stack;
    task_stack_magic(child);

    return child;

err_task:
    sched_task_destroy(child);
    return NULL;
}

static state copy_mem(struct task_clone_args *args, struct sched_task *child)
{
    struct sched_task *curr = args->curr;
    enum clone_flags flags = args->flags;
    struct memory *mem, *omem;

    /* clone a kernel thread */
    if (!(omem = curr->mem)) {
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

static struct sched_task *task_copy(struct task_clone_args *args)
{
    struct sched_task *curr = args->curr;
    enum clone_flags flags = args->flags;
    struct sched_task *child;
    state ret;

    child = task_alloc(args, curr);
    if (!child)
        return NULL;

    ret = copy_mem(args, child);
    if (ret)
        goto err_copy_mem;

    ret = proc_thread_copy(flags, curr, child, args->arg);
    if (ret)
        goto err_copy_proc;

    return child;

err_copy_mem:
err_copy_proc:
    return NULL;
}

pid_t task_clone(struct task_clone_args *args)
{
    struct sched_task *curr = sched_current();
    struct namespace *ns = curr->ns;
    struct sched_task *new;

    new = task_copy(args);
    pid_alloc_node(ns->pid, &new->pid);

    return new->pid.pid;
}

state fork_thread(enum clone_flags flags, int (*fn)(void *), void *arg)
{
    struct task_clone_args args = {

    };

    return task_clone(&args);
}

long syscall_fork(void)
{
    struct task_clone_args args = {

    };

    return task_clone(&args);
}

long syscall_vfork(void)
{
    struct task_clone_args args = {

    };

    return task_clone(&args);
}

long syscall_clone(void)
{
    struct task_clone_args args = {

    };

    return task_clone(&args);
}

void __init fork_init(void)
{
    stack_cache = ncache_create(task_stack_ncache_alloc,
        task_stack_ncache_free, STACK_CACHE_NR, NCACHE_PANIC, NULL);
    memory_cache = kcache_create("memory",
        sizeof(struct memory), KCACHE_PANIC);
}

EXPORT_SYMBOL(fork_thread);
SYSCALL_ENTRY(SYSCALL_NR_FORK, syscall_fork, 0);
SYSCALL_ENTRY(SYSCALL_NR_VFORK, syscall_vfork, 0);
SYSCALL_ENTRY(SYSCALL_NR_CLONE, syscall_clone, 5);
