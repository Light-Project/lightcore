/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "sc"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <sched.h>
#include <string.h>
#include <initcall.h>
#include <export.h>
#include <printk.h>
#include <panic.h>

struct sched_task *sched_current(void)
{
    struct sched_queue *queue = thiscpu_ptr(&sched_queues);
    return queue->curr;
}
EXPORT_SYMBOL(sched_current);

static inline state task_start(struct sched_task *task)
{

    return -ENOERR;
}

static inline void task_stop(struct sched_task *task)
{

}

static inline void task_yield(void)
{
    struct sched_queue *queue;
    struct sched_task *task;

    queue = thiscpu_ptr(&sched_queues);
    task = queue->curr;
    task->sched_type->task_yield(queue);
}

static inline struct sched_task *
task_pick(struct sched_queue *queue, struct sched_task *curr)
{
    struct sched_type *sched;
    struct sched_task *task;

    list_for_each_entry(sched, &sched_list, list) {
        task = sched->task_pick(queue);
        if (task)
            return task;
    }

    return NULL;
}

static __always_inline void
context_switch_finish(struct sched_queue *queue, struct sched_task *prev)
{

}

static __always_inline void
context_switch(struct sched_queue *queue, struct sched_task *prev, struct sched_task *next)
{

    if (next->mem) {
        /* switch to user */

        if (prev->mem) {


        }
    } else {
        /* switch to kthread */
        next->active_mem = prev->active_mem;

        if (prev->mem)
            /* switch form user */
            prev->active_mem = NULL;
        else
            /* switch form kthread */
            prev->active_mem = NULL;
    }

    proc_thread_switch(prev, next);
}

/**
 * sched_dispatch - the core of scheduler
 *
 */
static void __sched sched_dispatch(void)
{
    struct sched_queue *queue;
    struct sched_task *curr, *next;

    queue = thiscpu_ptr(&sched_queues);
    curr = queue->curr;

    next = task_pick(queue, curr);

    if (unlikely(curr == next)) {

    } else {
        context_switch(NULL, curr, next);
    }
}

void __sched reschedule(void)
{

}

void __noreturn sched_task_kill(void)
{
    sched_dispatch();
    BUG();
    proc_halt();
}

void sched_task_fork(struct sched_task *child)
{

}
EXPORT_SYMBOL(sched_task_fork);

struct sched_task *sched_task_create(void)
{
    struct sched_task *curr = sched_current();
    struct sched_type *sched = curr->sched_type;

#ifdef CONFIG_NUMA
    int numa = curr->cpu;
#else
    int numa = NUMA_NONE;
#endif

    if (sched->task_create)
        return sched->task_create(numa);
    else
        return kcache_alloc_numa(task_cache, GFP_KERNEL, numa);
}
EXPORT_SYMBOL(sched_task_create);

void sched_task_destroy(struct sched_task *task)
{
    struct sched_type *sched = task->sched_type;

    if (sched->task_create)
        sched->task_destroy(task);
    else
        kcache_free(task_cache, task);
}
EXPORT_SYMBOL(sched_task_destroy);

void sched_relax(void)
{
}

void sched_tick(void)
{
    struct sched_queue *queue = thiscpu_ptr(&sched_queues);
    struct sched_task *task = queue->curr;
    struct sched_type *sched = task->sched_type;

    if (!sched->task_tick)
        task->sched_type->task_tick(queue, task);
}

void sched_preempt(void)
{

}
