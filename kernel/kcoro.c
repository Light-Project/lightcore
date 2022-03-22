/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kcoro"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kcoro.h>
#include <panic.h>
#include <timekeeping.h>
#include <printk.h>
#include <export.h>

static struct kcache *kcoro_work_cache;
static struct kcache *kcoro_worker_cache;
static LIST_HEAD(worker_list);

static inline uint64_t calc_delta(struct kcoro_work *task, uint64_t delta)
{
    uint64_t fact = 1024;
    int shift = 32;

    if (unlikely(fact >> 32)) {
        while(fact >> 32)
        {
            fact >>= 1;
            shift--;
        }
    }

    fact = (uint64_t)(uint32_t)fact * sched_prio_to_wmult[task->dyprio];

    while (fact >> 32) {
        fact >>= 1;
        shift--;
    }

    return mul_u64_u32_shr(delta, fact, shift);
}

static inline uint64_t kcoro_delta_fair(struct kcoro_work *task, uint64_t delta)
{
    if (unlikely(task->dyprio != 20))
        delta = calc_delta(task, delta);

    return delta;
}

/**
 * kcoro_current - gets the currently coroutine
 * @return: current coroutine work
 */
struct kcoro_work *kcoro_current(void)
{
    struct kcoro_worker *worker = current->pdata;

    if (!worker) {
        BUG();
        return NULL;
    }

    return worker->curr;
}
EXPORT_SYMBOL(kcoro_current);

static long kcoro_work_vtime_cmp(const struct rb_node *a, const struct rb_node *b)
{
    const struct kcoro_work *work_a = container_of(a, struct kcoro_work, node);
    const struct kcoro_work *work_b = container_of(b, struct kcoro_work, node);
    return work_a->vtime - work_b->vtime;
}

static inline struct kcoro_work *kcoro_work_pick(struct kcoro_worker *worker)
{
    struct rb_node *lestmost = rb_cached_first(&worker->ready);
    return container_of_safe(lestmost, struct kcoro_work, node);
}

static inline void kcoro_worker_update(struct kcoro_worker *worker)
{
    struct kcoro_work *next = kcoro_work_pick(worker);

    if (next)
        worker->min_vtime = next->vtime;
    else if (worker->curr)
        worker->min_vtime = worker->curr->vtime;
    else
        worker->min_vtime = 0;
}

/**
 * kcoro_worker_enqueue - enqueue work to a worker
 * @worker: the worker pointer
 * @work: the work to be enqueue
 */
static void kcoro_worker_enqueue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_insert(&worker->ready, &work->node, kcoro_work_vtime_cmp);
    kcoro_worker_update(worker);
    work->worker = worker;
}

/**
 * kcoro_worker_dequeue - dequeue work form a worker
 * @worker: the worker pointer
 * @work: the work to be dequeue
 */
static void kcoro_worker_dequeue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_delete(&worker->ready, &work->node);
    kcoro_worker_update(worker);
    work->worker = NULL;
}

/**
 * kcoro_work_prio - coroutine scheduling core
 * @prio: new priority to replace
 */
void kcoro_work_prio(int prio)
{
    struct kcoro_worker *worker = current->pdata;
    struct kcoro_work *work = worker->curr;

    if (prio < -20)
        prio = 0;
    else if (prio > 19)
        prio = 39;
    else
        prio += 20;

    worker->weight -= sched_prio_to_weight[work->prio];
    worker->weight += sched_prio_to_weight[prio];
    work->prio = work->dyprio = prio;
}
EXPORT_SYMBOL(kcoro_work_prio);

void kcoro_yield(void)
{
    struct kcoro_worker *worker = current->pdata;
    struct kcoro_work *next, *work = worker->curr;
    struct kcontext *context = NULL;
    ktime_t now;

    now = timekeeping_get_time();
    work->vtime += kcoro_delta_fair(work, now - work->start);

    if (likely(work->state == KCORO_READY)) {
        if (((int64_t)(work->vtime - worker->min_vtime) > 0))
            kcoro_worker_enqueue(worker, work);
        else {
            work->start = now;
            return;
        }
    }

    next = kcoro_work_pick(worker);
    if (next) {
        kcoro_worker_dequeue(worker, next);
        next->start = now;
        if (next != work) {
            worker->curr = next;
            context = swapcontext(&work->context, &next->context);
        }
    }

    barrier();

    if (unlikely(context))
        work = container_of(context, struct kcoro_work, context);

    if (unlikely(work->state == KCORO_EXITED)) {
        if (context)
            kcoro_work_destroy(work);
        else
            swapcontext(&work->context, &worker->retc);
    }
}
EXPORT_SYMBOL(kcoro_yield);

void kcoro_exit(void)
{
    struct kcoro_worker *worker = current->pdata;
    struct kcoro_work *work = worker->curr;
    work->state = KCORO_EXITED;
    kcoro_yield();
}
EXPORT_SYMBOL(kcoro_exit);

void kcoro_nsleep(unsigned int nsec)
{
    ktime_t timeout = ktime_add_ns(timekeeping_get_time(), nsec);
    struct kcoro_work *work = ((struct kcoro_worker *)current->pdata)->curr;
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield();
    }
    kcoro_dyprio_reset(work);
}

void kcoro_usleep(unsigned int usec)
{
    ktime_t timeout = ktime_add_us(timekeeping_get_time(), usec);
    struct kcoro_work *work = ((struct kcoro_worker *)current->pdata)->curr;
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield();
    }
    kcoro_dyprio_reset(work);
}

void kcoro_msleep(unsigned int msec)
{
    ktime_t timeout = ktime_add_ms(timekeeping_get_time(), msec);
    struct kcoro_work *work = ((struct kcoro_worker *)current->pdata)->curr;
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield();
    }
    kcoro_dyprio_reset(work);
}

/**
 * kcoro_dispatch - coroutine scheduler
 * @pdata: the worker pointer
 */
void kcoro_dispatch(void *pdata)
{
    struct kcoro_worker *worker = pdata;
    struct kcoro_work *next, *curr;
    struct kcontext *context;

    current->pdata = worker;

    while ((next = kcoro_work_pick(worker))) {
        kcoro_worker_dequeue(worker, next);
        worker->curr = next;
        context = swapcontext(&worker->retc, &next->context);
        barrier();
        curr = container_of(context, struct kcoro_work, context);
        kcoro_work_destroy(curr);
    }
}
EXPORT_SYMBOL(kcoro_dispatch);

struct kcoro_work *kcoro_work_vcreate(struct kcoro_worker *worker, kcoro_entry_t entry,
                                      void *pdata, const char *namefmt, va_list args)
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
    work->vtime = worker->min_vtime;

    work->context.stack = stack;
    work->context.ssize = THREAD_SIZE;
    work->context.link = &worker->retc;

    vsnprintf(work->name, sizeof(work->name), namefmt, args);
    makecontext(&work->context, (state (*)(void))entry, 1, pdata);
    kcoro_worker_enqueue(worker, work);

    return work;
}
EXPORT_SYMBOL(kcoro_work_vcreate);

/**
 * kcoro_work_create - create a kcoroutine work
 * @worker: the worker pointer
 * @entry: the work handle entry
 * @pdata: handle entry pdata
 * @namefmt: name format for the work
 */
struct kcoro_work *kcoro_work_create(struct kcoro_worker *worker,
                                     kcoro_entry_t entry, void *pdata,
                                     const char *namefmt, ...)
{
    struct kcoro_work *work;
    va_list args;

    va_start(args, namefmt);
    work = kcoro_work_vcreate(worker, entry, pdata, namefmt, args);
    va_end(args);

    return work;
}
EXPORT_SYMBOL(kcoro_work_create);

/**
 * kcoro_work_destroy - destroy a kcoroutine work
 * @work: the work to destroy
 */
void kcoro_work_destroy(struct kcoro_work *work)
{
    struct kcoro_worker *worker = work->worker;

    if (worker) {
        kcoro_worker_dequeue(worker, work);
    }

    task_stack_free(work->context.stack);
    kcache_free(kcoro_work_cache, work);
}
EXPORT_SYMBOL(kcoro_work_destroy);

struct kcoro_worker *kcoro_worker_vcreate(const char *name, va_list args)
{
    struct kcoro_worker *worker;

    worker = kcache_zalloc(kcoro_worker_cache, GFP_KERNEL);
    if (!worker)
        return NULL;

    current->pdata = worker;
    list_add(&worker_list, &worker->list);

    return worker;
}
EXPORT_SYMBOL(kcoro_worker_vcreate);

/**
 * kthread_create_worker - create a kcoroutine worker
 * @namefmt: name format for the worker
 */
struct kcoro_worker *kcoro_worker_create(const char *namefmt, ...)
{
    struct kcoro_worker *worker;
    va_list args;

    va_start(args, namefmt);
    worker = kcoro_worker_vcreate(namefmt, args);
    va_end(args);

    return worker;
}
EXPORT_SYMBOL(kcoro_worker_create);

/**
 * kcoro_worker_destroy - destroy a kcoroutine worker
 * @worker: the worker to destroy
 */
void kcoro_worker_destroy(struct kcoro_worker *worker)
{
    kfree(worker);
}
EXPORT_SYMBOL(kcoro_worker_destroy);

void __init kcoro_init(void)
{
    kcoro_worker_cache = kcache_create(
        "kcoro-worker", sizeof(struct kcoro_worker),
        KCACHE_PANIC
    );
    kcoro_work_cache = kcache_create(
        "kcoro-work", sizeof(struct kcoro_work),
        KCACHE_PANIC
    );
}
