/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "sched: " fmt

#include <string.h>
#include <initcall.h>
#include <sort.h>
#include <sched.h>
#include <printk.h>

#include <asm/proc.h>

static LIST_HEAD(sched_list);
DEFINE_PERCPU(struct sched_queue, sched_queues);

static int sched_sort(struct list_head *a, struct list_head *b, void *data)
{
    struct sched_type *sched_a = list_entry(a, struct sched_type, list);
    struct sched_type *sched_b = list_entry(b, struct sched_type, list);
    return sched_a->priority - sched_b->priority;
}

static inline struct sched_type *sched_find(const char *name)
{
    struct sched_type *sched;
    list_for_each_entry(sched, &sched_list, list)
        if (!strcmp(sched->name, name))
            return sched;
    return NULL;
}

static __always_inline void
context_switch(struct sched_queue *queue, struct task *prev, struct task *next)
{

    if (next->mm) {

    } else {

    }

    proc_thread_switch(prev, next);
}

static inline struct task *
sched_task_get(struct sched_queue *queue, struct task *curr)
{
    struct sched_type *sched;
    struct task *task;

    list_for_each_entry(sched, &sched_list, list) {
        task = sched->task_next(queue);
        if (task)
            return task;
    }

    return NULL;
}

void sched_dispatch(void)
{
    struct sched_queue *queue;
    struct task *curr, *next;

    queue = thiscpu_ptr(&sched_queues);
    curr = queue->current;

    next = sched_task_get(queue, curr);
    context_switch(NULL, curr, next);
}

void __noreturn task_exit(void)
{
    for (;;)
        cpu_relax();
}

struct task *task_create(const char *schedn)
{
    struct sched_type *sched = sched_find(schedn);
    struct task *task;

    task = sched->task_create(0);
    if (task)
        return NULL;

    return task;
}

state task_start(struct task *task)
{

    return -ENOERR;
}

void task_stop(struct task *task)
{

}

void sched_relax(void)
{

}

void sched_tick(void)
{

}

state sched_register(struct sched_type *sched)
{
    if (!sched || !sched->name)
        return -EINVAL;

    pr_info("register scheduler %s\n", sched->name);

    list_add(&sched_list, &sched->list);
    sort_list(&sched_list, sched_sort, NULL);
    return -ENOERR;
}

void sched_unregister(struct sched_type *sched)
{
    if (!sched)
        return;

    pr_info("unregister scheduler %s\n", sched->name);
    list_del(&sched->list);
}

void __init sched_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, scheduler_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }
}
