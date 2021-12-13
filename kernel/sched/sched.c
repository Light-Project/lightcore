/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "sched: " fmt

#include <sched.h>
#include <string.h>
#include <initcall.h>
#include <cpu.h>
#include <export.h>
#include <printk.h>

#include <asm/percpu.h>
#include <asm/proc.h>

static LIST_HEAD(sched_list);
struct sched_type *default_sched;
DEFINE_PERCPU(struct sched_queue, queues);

static int sched_sort(struct list_head *a, struct list_head *b, void *data)
{
    struct sched_type *sched_a = list_entry(a, struct sched_type, list);
    struct sched_type *sched_b = list_entry(b, struct sched_type, list);
    return sched_a->priority - sched_b->priority;
}

static struct sched_type *sched_find(const char *name, unsigned char prio)
{
    struct sched_type *sched;

    list_for_each_entry(sched, &sched_list, list) {
        if (!strcmp(sched->name, name) ||
            sched->priority == prio)
            return sched;
    }

    return NULL;
}

static __always_inline void
context_switch(struct sched_queue *queue, struct task *prev, struct task *next)
{

    if (next->kthread) {

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
        task = sched->task_pick(queue);
        if (task)
            return task;
    }

    return NULL;
}

void sched_dispatch(void)
{
    struct sched_queue *queue;
    struct task *curr, *next;

    queue = thiscpu_ptr(&queues);
    curr = queue->curr;

    next = sched_task_get(queue, curr);
    context_switch(NULL, curr, next);
}

asmlinkage __visible void sched_preempt(void)
{

}

void sched_relax(void)
{

}

void sched_tick(void)
{
    struct sched_queue *queue = thiscpu_ptr(&queues);
    struct task *task = queue->curr;

    return;
    task->sched_type->task_tick(queue, task);
}

state sched_task_start(struct task *task)
{

    return -ENOERR;
}

void sched_task_stop(struct task *task)
{

}

struct task *sched_task_create(const char *schedn, int flags)
{
    struct sched_type *sched;
    struct task *task;

    if (!schedn)
        sched = default_sched;
    else {
        sched = sched_find(schedn, SCHED_PRIO_MAX);
        if (!sched)
            return ERR_PTR(-EINVAL);
    }

    task = sched->task_create(flags);
    if (PTR_ERR(task))
        return task;

    return task;
}

void sched_task_destroy(struct task *task)
{
    struct sched_type *sched = task->sched_type;
    sched->task_destroy(task);
}

void __noreturn sched_task_exit(void)
{
    for (;;)
    cpu_relax();
}

state sched_register(struct sched_type *sched)
{
    struct sched_type *other;

    if (!sched->name || !sched->task_tick)
        return -EINVAL;

    other = sched_find(sched->name, sched->priority);
    if (other) {
        pr_err("scheduler %s already registered\n", other->name);
        return -EINVAL;
    }

    pr_info("register scheduler %s\n", sched->name);

    list_add(&sched_list, &sched->list);
    list_sort(&sched_list, sched_sort, NULL);
    return -ENOERR;
}

void sched_unregister(struct sched_type *sched)
{
    pr_info("unregister scheduler %s\n", sched->name);
    list_del(&sched->list);
}

static void queue_setup(struct sched_queue *queue)
{

}

static void sched_initcall(void)
{
    initcall_entry_t *fn;
    initcall_t call;
    state ret;

    initcall_for_each_fn(fn, scheduler_initcall) {
        call = initcall_from_entry(fn);
        if ((ret = call()))
            pr_crit("%s init failed, error code [%d]\n", fn->name, ret);
    }
}

void __init sched_init(void)
{
    struct sched_queue *queue;
    unsigned int cpu;

    sched_initcall();

    cpu_for_each(cpu) {
        queue = percpu_ptr(cpu, &queues);
        queue_setup(queue);
    }
}

EXPORT_SYMBOL(sched_register);
EXPORT_SYMBOL(sched_unregister);
