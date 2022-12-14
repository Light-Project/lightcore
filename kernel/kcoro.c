/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kcoro"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kcoro.h>
#include <task.h>
#include <kmalloc.h>
#include <delay.h>
#include <timekeeping.h>
#include <crash.h>
#include <printk.h>
#include <export.h>

static struct kcache *kcoro_work_cache;
static struct kcache *kcoro_worker_cache;

static long work_vtime_cmp(const struct rb_node *na, const struct rb_node *nb)
{
    const struct kcoro_work *work_a = rbnode_to_kcoro_work(na);
    const struct kcoro_work *work_b = rbnode_to_kcoro_work(nb);
    if (work_a->vtime == work_b->vtime) return 0;
    return work_a->vtime < work_b->vtime ? -1 : 1;
}

static long work_timeout_cmp(const struct rb_node *na, const struct rb_node *nb)
{
    const struct kcoro_work *work_a = rbnode_to_kcoro_work(na);
    const struct kcoro_work *work_b = rbnode_to_kcoro_work(nb);
    if (work_a->timeout == work_b->timeout) return 0;
    return work_a->timeout < work_b->timeout ? -1 : 1;
}

struct kcoro_work *kcoro_pick(struct kcoro_worker *worker)
{
    struct rb_node *first = rb_cached_first(&worker->ready);
    return rbnode_to_kcoro_work_safe(first);
}
EXPORT_SYMBOL(kcoro_pick);

static void worker_update(struct kcoro_worker *worker)
{
    struct kcoro_work *next = kcoro_pick(worker);

    if (next)
        worker->mvtime = next->vtime;
    else if (worker->curr)
        worker->mvtime = worker->curr->vtime;
    else
        worker->mvtime = 0;
}

/**
 * kcoro_enqueue - enqueue work to a worker.
 * @worker: the worker pointer.
 * @work: the work to be enqueue.
 */
void kcoro_enqueue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_insert(&worker->ready, &work->node, work_vtime_cmp);
    worker_update(worker);
}
EXPORT_SYMBOL(kcoro_enqueue);

/**
 * kcoro_dequeue - dequeue work form a worker.
 * @worker: the worker pointer.
 * @work: the work to be dequeue.
 */
void kcoro_dequeue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_delete(&worker->ready, &work->node);
    worker_update(worker);
}
EXPORT_SYMBOL(kcoro_dequeue);

void kcoro_activate(struct kcoro_worker *worker, struct kcoro_work *work)
{
    if (WARN_ON(worker->curr == work))
        return;

    work->vtime = worker->mvtime;
    kcoro_enqueue(worker, work);
}
EXPORT_SYMBOL(kcoro_activate);

/**
 * kcoro_work_prio - change the priority of current work.
 * @prio: the priority to change to.
 */
void kcoro_prio(struct kcoro_work *work, int prio)
{
    clamp_adj(prio, -20, 19);
    work->prio = work->dyprio = prio + 20;
}
EXPORT_SYMBOL(kcoro_prio);

void kcoro_timeout_wakeup(struct kcoro_worker *worker)
{
    struct rb_node *first;

    while ((first = rb_cached_first(&worker->blocked))) {
        struct kcoro_work *work;

        work = rbnode_to_kcoro_work(first);
        if (ttime_after(work->timeout, ticktime))
            break;

        rb_cached_delete(&worker->blocked, &work->node);
        kcoro_state_set(work, KCORO_READY);
        kcoro_enqueue(worker, work);
    }
}

void kcoro_yield(struct kcoro_work *work)
{
    struct kcoro_worker *worker = work->worker;
    struct kcoro_work *next;
    uint64_t delta;
    ktime_t now;

    if (unlikely(work->state == KCORO_EXITED)) {
        swapcontext(&work->context, &worker->retctx);
        unreachable();
    }

    now = timekeeping_get_time();
    delta = ktime_to_ns(ktime_sub(now, work->start));
    work->vtime += sched_calc_fair(work->dyprio, delta);

    /* Wakeup blocking work */
    kcoro_timeout_wakeup(worker);

    /* Re-enqueue the previous ready work */
    if (work->state == KCORO_BLOCK)
        rb_cached_insert(&worker->blocked, &work->node, work_timeout_cmp);
    else { /* state == KCORO_READY */
        if (work->vtime > worker->mvtime)
            kcoro_enqueue(worker, work);
        else {
            work->start = now;
            return;
        }
    }

    if (unlikely(!(next = kcoro_pick(worker))))
        swapcontext(&work->context, &worker->retctx);
    else {
        /* found higher priority work, switch to it */
        kcoro_dequeue(worker, next);
        next->start = now;

        if (work != next) {
            worker->curr = next;
            swapcontext(&work->context, &next->context);
        }
    }
}
EXPORT_SYMBOL(kcoro_yield);

void __noreturn kcoro_exit(struct kcoro_work *work, state retval)
{
    work->context.retval = retval;
    kcoro_state_set(work, KCORO_EXITED);
    kcoro_yield(work);

    /* never run here */
    BUG();
}
EXPORT_SYMBOL(kcoro_exit);

void kcoro_ndelay(struct kcoro_work *work, unsigned int nsec)
{
    ktime_t timeout = ktime_add_ns(timekeeping_get_time(), nsec);
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield(work);
    }
    kcoro_dyprio_reset(work);
}
EXPORT_SYMBOL(kcoro_ndelay);

void kcoro_udelay(struct kcoro_work *work, unsigned int usec)
{
    ktime_t timeout = ktime_add_us(timekeeping_get_time(), usec);
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield(work);
    }
    kcoro_dyprio_reset(work);
}
EXPORT_SYMBOL(kcoro_udelay);

void kcoro_mdelay(struct kcoro_work *work, unsigned int msec)
{
    ktime_t timeout = ktime_add_ms(timekeeping_get_time(), msec);
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield(work);
    }
    kcoro_dyprio_reset(work);
}
EXPORT_SYMBOL(kcoro_mdelay);

void kcoro_tsleep(struct kcoro_work *work, ttime_t timeout)
{
    work->timeout = ticktime + timeout;
    kcoro_state_set(work, KCORO_BLOCK);
    kcoro_yield(work);
}
EXPORT_SYMBOL(kcoro_tsleep);

void kcoro_msleep(struct kcoro_work *work, unsigned int msec)
{
    ttime_t timeout = ms_to_ttime(msec);
    kcoro_tsleep(work, timeout);
}
EXPORT_SYMBOL(kcoro_msleep);

struct kcoro_work *kcoro_function(struct kcoro_worker *worker)
{
    struct kcoro_work *work;
    struct kcontext *context;

    work = kcoro_pick(worker);
    if (unlikely(!work))
        return NULL;

    kcoro_dequeue(worker, work);
    work->start = timekeeping_get_time();
    worker->curr = work;

    /* Always retctx running here so don't need memory barriers */
    context = swapcontext(&worker->retctx, &work->context);
    work = container_of(context, struct kcoro_work, context);
    worker->curr = NULL;

    return work;
}
EXPORT_SYMBOL(kcoro_function);

state kcoro_schedule(struct kcoro_worker *worker)
{
    struct kcoro_work *work;
    enum kcoro_state cstate;
    state retval;

    for (;;) {
        work = kcoro_function(worker);
        if (unlikely(!work)) {
            struct rb_node *first;

            first = rb_cached_first(&worker->blocked);
            work = rbnode_to_kcoro_work_safe(first);

            if (unlikely(!work))
                break;
        }

        switch (cstate = kcoro_state_get(work)) {
            case KCORO_BLOCK:
                if (work->timeout == KTIME_MAX)
                    return -ETIMEDOUT;
                if (work->timeout > ticktime)
                    tsleep(work->timeout - ticktime);
                kcoro_timeout_wakeup(worker);
                break;

            case KCORO_READY:
                /* Exit from the end of a work */
                fallthrough;

            case KCORO_EXITED:
                /* Exit from kcoro_exit() */
                retval = work->context.retval;
                kcoro_work_destroy(work);
                if (retval)
                    return retval;
                break;

            default:
                return cstate;
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(kcoro_schedule);

struct kcoro_work *
kcoro_work_create(struct kcoro_worker *worker, kcoro_entry_t entry, void *pdata)
{
    struct kcoro_work *work;
    void *stack;

    work = kcache_zalloc(kcoro_work_cache, GFP_KERNEL);
    if (!work)
        return NULL;

    stack = task_stack_alloc();
    if (!stack) {
        kcache_free(kcoro_work_cache, work);
        return NULL;
    }

    work->prio = 20;
    work->dyprio = 20;
    work->start = timekeeping_get_time();
    work->vtime = worker->mvtime;
    work->worker = worker;

    work->context.stack = stack;
    work->context.ssize = THREAD_SIZE;
    work->context.link = &worker->retctx;
    makecontext(&work->context, (void *)entry, 2, work, pdata);

    return work;
}
EXPORT_SYMBOL(kcoro_work_create);

void kcoro_work_destroy(struct kcoro_work *work)
{
    task_stack_free(work->context.stack);
    kcache_free(kcoro_work_cache, work);
}
EXPORT_SYMBOL(kcoro_work_destroy);

struct kcoro_worker *kcoro_worker_create(void)
{
    struct kcoro_worker *worker;

    worker = kcache_zalloc(kcoro_worker_cache, GFP_KERNEL);
    if (unlikely(!worker))
        return NULL;

    return worker;
}
EXPORT_SYMBOL(kcoro_worker_create);

void kcoro_worker_destroy(struct kcoro_worker *worker)
{
    kfree(worker);
}
EXPORT_SYMBOL(kcoro_worker_destroy);

void __init kcoro_init(void)
{
    kcoro_worker_cache = KCACHE_CREATE(
        struct kcoro_worker, KCACHE_PANIC
    );
    kcoro_work_cache = KCACHE_CREATE(
        struct kcoro_work, KCACHE_PANIC
    );
}
