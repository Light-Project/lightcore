/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "sc"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <sched.h>
#include <task.h>
#include <linkage.h>
#include <string.h>
#include <init.h>
#include <irqflags.h>
#include <initcall.h>
#include <syscall.h>
#include <stackprotector.h>
#include <printk.h>
#include <panic.h>
#include <export.h>

static inline struct sched_task *
task_pick_next(struct sched_queue *queue, struct sched_task *curr)
{
    struct sched_type *sched;
    struct sched_task *task;

    list_for_each_entry(sched, &sched_list, list) {
        task = sched->task_pick(queue);
        if (task)
            return task;
    }

    BUG();
}

static inline void
task_enqueue(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    task->sched_type->task_enqueue(queue, task, flags);
}

static inline void
task_dequeue(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    task->sched_type->task_dequeue(queue, task, flags);
}

static inline void
task_activate(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    task->queued = SCHED_TASK_QUEUED;
    task_enqueue(queue, task, flags);
}

static inline void
task_deactivate(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    task->queued = (SCHED_DEQUEUE_SLEEP & flags) ? 0 : SCHED_TASK_MIGRATING;
    task_dequeue(queue, task, flags);
}

static __always_inline void
context_switch_finish(struct sched_task *prev)
{
    unsigned long state = READ_ONCE(prev->state);

    /**
     * Now that we have finished task switching,
     * we can release the previous task.
     */
    if (unlikely(state == SCHED_TASK_KILL)) {
        task_stack_free(prev->stack);
        sched_task_destroy(prev);
    }

    irq_local_enable();
}

asmlinkage __visible void
sched_switch_tail(struct sched_task *prev)
{
    context_switch_finish(prev);
    irq_local_enable();
}

static __always_inline void
context_switch(struct sched_queue *queue, struct sched_task *prev, struct sched_task *next)
{
    struct kcontext *prevc;

    if (next->mem) {
        /* switch to user */

        if (!prev->mem) {
            /* switch form kthread */
            queue->prev_mem = prev->active_mem;
            prev->active_mem = NULL;
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

    current_set(next);

    /* switch kernel context */
    prevc = swapcontext(&prev->kcontext, &next->kcontext);
    barrier();

    /* switch processor context */
    prev = kcontext_to_task(prevc);
    stack_canary_switch(current);

    proc_thread_switch(prev);
    context_switch_finish(prev);
}

static void __sched sched_dispatch(bool preempt)
{
    struct sched_task *curr, *next;
    struct sched_queue *queue;
    uint64_t *task_switch;
    unsigned long state;

    queue = current_queue;
    curr = queue->curr;

    irq_local_disable();

    task_switch = &curr->nivcsw;
    state = READ_ONCE(curr->state);

    if (state != SCHED_TASK_RUNNING && !preempt) {
        task_deactivate(queue, curr, SCHED_DEQUEUE_SLEEP | SCHED_QUEUE_NOCLOCK);
        task_switch = &curr->nvcsw;
    }

    next = task_pick_next(queue, curr);
    task_clr_resched(curr);

    /* No context switching is required */
    if (unlikely(curr == next)) {
        irq_local_enable();
        return;
    }

    ++queue->context_switches;
    ++*task_switch;

    context_switch(queue, curr, next);
}

static void __sched preempt_handle(void)
{
    BUG_ON(preempt_count() || !irq_local_disabled());

    do {
        irq_local_enable();
        sched_dispatch(true);
        irq_local_disable();
    } while (current_test_resched());
}

void __sched sched_resched(void)
{
    do {
        irq_local_disable();
        sched_dispatch(false);
        irq_local_enable();
    } while (current_test_resched());
}
EXPORT_SYMBOL(sched_resched);

void __sched sched_idle(void)
{
    WARN_ON(current->state);

    do {
        sched_dispatch(false);
    } while (current_test_resched());
}

void __noreturn sched_kill(void)
{
    /**
     * Set the exit flag bit and it will be
     * released in the next switching.
     */
    current_set_state(SCHED_TASK_KILL);

    sched_dispatch(false);
    BUG();

    /* never come here */
    proc_halt();
}
EXPORT_SYMBOL(sched_kill);

void sched_yield(void)
{
    struct sched_queue *queue = current_queue;
    struct sched_type *sched = current->sched_type;

    if (sched->task_yield)
        sched->task_yield(queue);

    sched_resched();
}
EXPORT_SYMBOL(sched_yield);

void sched_tick(void)
{
    struct sched_queue *queue = thiscpu_ptr(&sched_queues);
    struct sched_task *task = queue->curr;

    if (task->sched_type->task_tick)
        task->sched_type->task_tick(queue, task);
}

bool sched_cond_resched_handle(void)
{
    if (preempt_count()) {
        preempt_handle();
        return true;
    }

    return false;
}

void sched_preempt_handle(void)
{
    if (preempt_count())
        return;

    if (current_test_resched())
        preempt_handle();
}

DEFINE_SYSCALL0(sched_yield)
{
    sched_yield();
    return 0;
}

static inline void
wake_up_task(struct sched_queue *queue, struct sched_task *task, unsigned long flags)
{
    task_set_state(task, SCHED_TASK_RUNNING);
}

static void wake_up_queue(struct sched_task *task, int cpu, unsigned long flags)
{
    struct sched_queue *queue = percpu_ptr(cpu, &sched_queues);
    enum sched_queue_flags queue_flags = SCHED_ENQUEUE_WEAKUP | SCHED_QUEUE_NOCLOCK;

    task_activate(queue, task, queue_flags);
    wake_up_task(queue, task, flags);
}

static void wake_up_state_wake(struct sched_task *task, unsigned long flags)
{
    wake_up_queue(task, 0, flags);
}

void sched_wake_up(struct sched_task *task)
{
    wake_up_state_wake(task, SCHED_TASK_BLOCKED);
}
EXPORT_SYMBOL(sched_wake_up);

void sched_wake_up_new(struct sched_task *task)
{
    struct sched_queue *queue;
    task_set_state(task, SCHED_TASK_RUNNING);
    queue = thiscpu_ptr(&sched_queues);
    task_activate(queue, task, SCHED_QUEUE_NOCLOCK);
}
EXPORT_SYMBOL(sched_wake_up_new);

state sched_task_clone(struct sched_task *task, enum clone_flags flags)
{
    task->state = SCHED_TASK_NEW;
    task->sched_type = default_sched;
    return -ENOERR;
}
EXPORT_SYMBOL(sched_task_clone);

struct sched_task *sched_task_create(void)
{
    struct sched_type *sched = default_sched;
    struct sched_task *task;

#ifdef CONFIG_NUMA
    int numa = curr->cpu;
#else
    int numa = NUMA_NONE;
#endif

    if (sched->task_create)
        task = sched->task_create(numa);
    else
        task = kcache_alloc_numa(task_cache, GFP_KERNEL, numa);

    return task;
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
