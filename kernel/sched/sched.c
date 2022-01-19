/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <sched.h>
#include <string.h>
#include <cpu.h>
#include <initcall.h>
#include <export.h>
#include <printk.h>

struct kcache *task_cache;
struct sched_type *default_sched;
LIST_HEAD(sched_list);
DEFINE_PERCPU(struct sched_queue, sched_queues);

static int sched_sort(struct list_head *a, struct list_head *b, void *data)
{
    struct sched_type *sched_a = container_of(a, struct sched_type, list);
    struct sched_type *sched_b = container_of(b, struct sched_type, list);
    return sched_a->priority - sched_b->priority;
}

struct sched_type *sched_find(const char *name, unsigned char prio)
{
    struct sched_type *sched;

    list_for_each_entry(sched, &sched_list, list) {
        if (!strcmp(sched->name, name) ||
            sched->priority == prio)
            return sched;
    }

    return NULL;
}

static __always_inline state
sched_queue_create(struct sched_type *sched, struct sched_queue *queue)
{
    if (sched->queue_create)
        return sched->queue_create(queue);
    return -ENOERR;
}

static __always_inline void
sched_queue_destroy(struct sched_type *sched, struct sched_queue *queue)
{
    if (sched->queue_destroy)
        sched->queue_destroy(queue);
}

state sched_register(struct sched_type *sched)
{
    struct sched_type *other;
    struct sched_queue *queue;
    unsigned int cpu;
    state ret;

    if (!sched->name || !sched->task_pick)
        return -EINVAL;

    other = sched_find(sched->name, sched->priority);
    if (other) {
        pr_err("scheduler %s already registered\n", other->name);
        return -EINVAL;
    }

    cpu_for_each(cpu) {
        queue = percpu_ptr(cpu, &sched_queues);
        ret = sched_queue_create(sched, queue);
        if (ret)
            return ret;
    }

    pr_info("register scheduler %s\n", sched->name);
    list_add(&sched_list, &sched->list);
    list_sort(&sched_list, sched_sort, NULL);

    return -ENOERR;
}
EXPORT_SYMBOL(sched_register);

void sched_unregister(struct sched_type *sched)
{
    struct sched_queue *queue;
    unsigned int cpu;

    pr_info("unregister scheduler %s\n", sched->name);
    list_del(&sched->list);

    cpu_for_each(cpu) {
        queue = percpu_ptr(cpu, &sched_queues);
        sched_queue_destroy(sched, queue);
    }
}
EXPORT_SYMBOL(sched_unregister);

static void __init sched_initcall(void)
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

static void __init queue_setup(struct sched_queue *queue)
{
    list_head_init(&queue->run_list);
}

void __init sched_init(void)
{
    struct sched_queue *queue;
    unsigned int cpu;

    task_cache = kcache_create("sched-task",
        sizeof(struct sched_task), KCACHE_PANIC);

    cpu_for_each(cpu) {
        queue = percpu_ptr(cpu, &sched_queues);
        queue_setup(queue);
    }

    sched_initcall();
}
