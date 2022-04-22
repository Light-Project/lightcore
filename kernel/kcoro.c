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
static SPIN_LOCK(worker_lock);

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
 * kcoro_worker_enqueue - enqueue work to a worker.
 * @worker: the worker pointer.
 * @work: the work to be enqueue.
 */
static void kcoro_worker_enqueue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_insert(&worker->ready, &work->node, kcoro_work_vtime_cmp);
    kcoro_worker_update(worker);
    work->state = KCORO_READY;
}

/**
 * kcoro_worker_dequeue - dequeue work form a worker.
 * @worker: the worker pointer.
 * @work: the work to be dequeue.
 */
static void kcoro_worker_dequeue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_delete(&worker->ready, &work->node);
    kcoro_worker_update(worker);
    work->state = KCORO_RUNNING;
}

/**
 * kcoro_resource_release - release all resources of a work.
 * @work: the work to be release.
 */
static void kcoro_resource_release(struct kcoro_work *work)
{
    struct kcoro_worker *worker = work->worker;
    irqflags_t irqflags;

    spin_lock_irqsave(&worker->lock, &irqflags);
    if (work->state == KCORO_READY)
        kcoro_worker_dequeue(worker, work);
    spin_unlock_irqrestore(&worker->lock, &irqflags);

    task_stack_free(work->context.stack);
    kcache_free(kcoro_work_cache, work);
}

/**
 * kcoro_current - gets the currently work.
 * @return: current coroutine work.
 */
struct kcoro_work *kcoro_current(void)
{
    struct kcoro_worker *worker = current->pdata;

    if (!current_test_kcoro()) {
        BUG();
        return NULL;
    }

    return worker->curr;
}
EXPORT_SYMBOL(kcoro_current);

/**
 * kcoro_work_prio - change the priority of current work.
 * @prio: the priority to change to.
 */
void kcoro_work_prio(int prio)
{
    struct kcoro_worker *worker;
    struct kcoro_work *work;
    irqflags_t irqflags;

    if (!current_test_kcoro()) {
        BUG();
        return;
    }

    worker = current->pdata;
    work = worker->curr;

    if (prio < -20)
        prio = 0;
    else if (prio > 19)
        prio = 39;
    else
        prio += 20;

    spin_lock_irqsave(&worker->lock, &irqflags);
    worker->weight -= sched_prio_to_weight[work->prio];
    worker->weight += sched_prio_to_weight[prio];
    work->prio = work->dyprio = prio;
    spin_unlock_irqrestore(&worker->lock, &irqflags);
}
EXPORT_SYMBOL(kcoro_work_prio);

/**
 * kcoro_yield - kcoro scheduling core.
 */
void kcoro_yield(void)
{
    struct kcoro_worker *worker;
    struct kcoro_work *next, *work;
    struct kcontext *context = NULL;
    irqflags_t irqflags;
    ktime_t now;

    if (!current_test_kcoro()) {
        BUG();
        return;
    }

    worker = current->pdata;
    work = worker->curr;
    now = timekeeping_get_time();

    spin_lock_irqsave(&worker->lock, &irqflags);
    work->vtime += kcoro_delta_fair(work, now - work->start);

    /* Re-enqueue the previous ready work */
    if (likely(work->state == KCORO_RUNNING)) {
        if (((int64_t)(work->vtime - worker->min_vtime) > 0))
            kcoro_worker_enqueue(worker, work);
        else {
            spin_unlock_irqrestore(&worker->lock, &irqflags);
            work->start = now;
            return;
        }
    }

    /* If a higher priority work is found, switch to it */
    if ((next = kcoro_work_pick(worker)))
        kcoro_worker_dequeue(worker, next);

    spin_unlock_irqrestore(&worker->lock, &irqflags);

    if (next) {
        next->start = now;
        if (next != work) {
            worker->curr = next;
            context = swapcontext(&work->context, &next->context);
        }
    }

    barrier();

    /* If we switch from a context, then we get the previous work */
    if (context)
        work = container_of(context, struct kcoro_work, context);

    /* Exit the previous work by circumstances */
    if (unlikely(work->state == KCORO_EXITED)) {
        if (context) {
            /**
             * If it has been switched to other tasks, then we
             * can directly release this work.
             */
            kcoro_resource_release(work);
        } else {
            /**
             * If there are no other tasks to switch out, then we
             * switch to the return context of the work.
             */
            swapcontext(&work->context, &worker->retc);
        }
    }
}
EXPORT_SYMBOL(kcoro_yield);

/**
 * kcoro_exit - voluntarily quit current work.
 */
void kcoro_exit(void)
{
    struct kcoro_worker *worker;
    struct kcoro_work *work;

    if (!current_test_kcoro()) {
        BUG();
        return;
    }

    worker = current->pdata;
    work = worker->curr;

    work->state = KCORO_EXITED;
    kcoro_yield();
}
EXPORT_SYMBOL(kcoro_exit);

/**
 * kcoro_dispatch - kcoroutine scheduler.
 * @worker: the worker to dispatch.
 */
void kcoro_dispatch(struct kcoro_worker *worker)
{
    struct kcoro_work *curr, *next;
    struct kcontext *context;
    irqflags_t irqflags;

    task_set_kcoro(current);
    current->pdata = worker;

    do {
        spin_lock_irqsave(&worker->lock, &irqflags);
        next = kcoro_work_pick(worker);
        if (next) {
            kcoro_worker_dequeue(worker, next);
            worker->curr = next;
            spin_unlock_irqrestore(&worker->lock, &irqflags);

            /* Always retc running here so don't need memory barriers */
            context = swapcontext(&worker->retc, &next->context);
            curr = container_of(context, struct kcoro_work, context);

            /* Exit from the end of a work */
            kcoro_resource_release(curr);
        }
    } while (next);

    spin_unlock_irqrestore(&worker->lock, &irqflags);
}
EXPORT_SYMBOL(kcoro_dispatch);

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

struct kcoro_work *
kcoro_work_vcreate(struct kcoro_worker *worker, kcoro_entry_t entry,
                   void *pdata, const char *namefmt, va_list args)
{
    struct kcoro_work *work;
    irqflags_t irqflags;
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
    work->worker = worker;

    work->context.stack = stack;
    work->context.ssize = THREAD_SIZE;
    work->context.link = &worker->retc;

    vsnprintf(work->name, sizeof(work->name), namefmt, args);
    makecontext(&work->context, (state (*)(void))entry, 1, pdata);

    spin_lock_irqsave(&worker->lock, &irqflags);
    kcoro_worker_enqueue(worker, work);
    spin_unlock_irqrestore(&worker->lock, &irqflags);

    return work;
}
EXPORT_SYMBOL(kcoro_work_vcreate);

/**
 * kcoro_work_create - create a kcoroutine work.
 * @worker: the worker pointer.
 * @entry: the work handle entry.
 * @pdata: handle entry pdata.
 * @namefmt: name format for the work.
 */
struct kcoro_work *
kcoro_work_create(struct kcoro_worker *worker, kcoro_entry_t entry,
                  void *pdata, const char *namefmt, ...)
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
    irqflags_t irqflags;

    spin_lock_irqsave(&worker->lock, &irqflags);

    if (work->state == KCORO_READY) {
        spin_unlock_irqrestore(&worker->lock, &irqflags);
        kcoro_resource_release(work);
        return;
    }

    else if (work->state == KCORO_RUNNING)
        work->state = KCORO_EXITED;

    spin_unlock_irqrestore(&worker->lock, &irqflags);
}
EXPORT_SYMBOL(kcoro_work_destroy);

struct kcoro_worker *kcoro_worker_vcreate(const char *name, va_list args)
{
    struct kcoro_worker *worker;

    worker = kcache_zalloc(kcoro_worker_cache, GFP_KERNEL);
    if (!worker)
        return NULL;

    spin_lock(&worker_lock);
    list_add(&worker_list, &worker->list);
    spin_unlock(&worker_lock);

    return worker;
}
EXPORT_SYMBOL(kcoro_worker_vcreate);

/**
 * kcoro_worker_create - create a kcoroutine worker
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
    spin_lock(&worker_lock);
    list_del(&worker->list);
    spin_unlock(&worker_lock);

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
