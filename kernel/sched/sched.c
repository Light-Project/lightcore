/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <sched.h>
#include <string.h>
#include <cpu.h>
#include <init.h>
#include <initcall.h>
#include <export.h>
#include <printk.h>

struct kcache *task_cache;
struct sched_type *default_sched;

LIST_HEAD(sched_list);
DEFINE_PERCPU(struct sched_queue, sched_queues);
DEFINE_PERCPU(unsigned long, sched_preempt_count);

static long sched_sort(struct list_head *a, struct list_head *b, void *data)
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
    list_head_init(&queue->tasks);
}

#ifdef CONFIG_PREEMPT_DYNAMIC

const char *sched_preempt_name[SCHED_PREEMPT_NR_MAX] = {
    [SCHED_PREEMPT_UNINIT]      = "uninit",
    [SCHED_PREEMPT_NONE]        = "none",
    [SCHED_PREEMPT_VOLUNTARY]   = "voluntary",
    [SCHED_PREEMPT_FULL]        = "full",
};

enum sched_preempt sched_preempt_mode;

DEFINE_STATIC_CALL_RET0(sched_cond_resched, sched_cond_resched_handle);
DEFINE_STATIC_CALL_RET0(sched_might_resched, sched_cond_resched_handle);
DEFINE_STATIC_CALL(sched_preempt, sched_preempt_handle);

state sched_preempt_updata(enum sched_preempt mode)
{
    switch (mode) {
        case SCHED_PREEMPT_NONE:
            static_call_update(sched_cond_resched, sched_cond_resched_handle);
            static_call_update(sched_might_resched, (void *)__static_call_ret0);
            static_call_update(sched_preempt, (void *)__static_call_nop);
            break;

        case SCHED_PREEMPT_VOLUNTARY:
            static_call_update(sched_cond_resched, sched_cond_resched_handle);
            static_call_update(sched_might_resched, sched_cond_resched_handle);
            static_call_update(sched_preempt, (void *)__static_call_nop);
            break;

        case SCHED_PREEMPT_FULL:
            static_call_update(sched_cond_resched, (void *)__static_call_ret0);
            static_call_update(sched_might_resched, (void *)__static_call_ret0);
            static_call_update(sched_preempt, sched_preempt_handle);
            break;

        default:
            return -EINVAL;
    }

    pr_info("Dynamic Preempt: %s\n", sched_preempt_name[mode]);
    sched_preempt_mode = mode;

    return -ENOERR;
}

static void __init preempt_dynamic_init(void)
{
    if (sched_preempt_mode != SCHED_PREEMPT_UNINIT)
        return;

    if (IS_ENABLED(CONFIG_PREEMPT_NONE))
        sched_preempt_updata(SCHED_PREEMPT_NONE);

    else if (IS_ENABLED(CONFIG_PREEMPT_VOLUNTARY))
        sched_preempt_updata(SCHED_PREEMPT_VOLUNTARY);

    else if (IS_ENABLED(CONFIG_PREEMPT_FULL)) {
        pr_info("Dynamic Preempt: full\n");
        sched_preempt_mode = SCHED_PREEMPT_FULL;
    }
}

#else /* !CONFIG_PREEMPT_DYNAMIC */

static void __init preempt_dynamic_init(void) {}

#endif /* CONFIG_PREEMPT_DYNAMIC */

static void __init idle_init(struct sched_queue *queue, struct sched_task *task)
{
    queue->idle = task;
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

    queue = thiscpu_ptr(&sched_queues);
    queue->curr = &init_task;

    sched_initcall();
    preempt_dynamic_init();
    idle_init(current_queue, current);
}
