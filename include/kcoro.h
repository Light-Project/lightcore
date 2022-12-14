/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KCORO_H_
#define _KCORO_H_

#include <list.h>
#include <rbtree.h>
#include <kcontext.h>

struct kcoro_work;
typedef state (*kcoro_entry_t)(struct kcoro_work *work, void *pdata);

enum kcoro_state {
    KCORO_READY     = 0,
    KCORO_BLOCK     = 1,
    KCORO_EXITED    = 2,
};

/**
 * kcoro_work - describe a kcoroutine work.
 * @node: node add to worker.
 * @worker: bound worker.
 * @name: kcoro work name.
 * @context: the work context.
 * @state: running state.
 * @start: work start time.
 * @vtime: relative running time.
 * @prio: static priority.
 * @dyprio: dynamic priority.
 * @pdata: work entry pdata.
 */
struct kcoro_work {
    struct rb_node node;
    struct kcoro_worker *worker;

    struct kcontext context;
    enum kcoro_state state;

    ktime_t start;
    uint64_t vtime;
    ttime_t timeout;

    int prio;
    int dyprio;
};

#define rbnode_to_kcoro_work(ptr) \
    container_of(ptr, struct kcoro_work, node)

#define rbnode_to_kcoro_work_safe(ptr) \
    container_of_safe(ptr, struct kcoro_work, node)

/**
 * kcoro_worker - describe a kcoroutine worker.
 * @lock: used to lock the red black tree and task status.
 * @list: list node for system management.
 * @task: point to the parent task.
 * @ready: ready queue ont this worker.
 * @retctx: context use for task return.
 * @curr: current running kcoro work.
 * @min_vtime: minimum virtual time in this worker.
 * @weight: worker weight.
 */
struct kcoro_worker {
    struct rb_root_cached ready;
    struct rb_root_cached blocked;
    uint64_t mvtime;

    struct kcoro_work *curr;
    struct kcontext retctx;
};

static inline enum kcoro_state kcoro_state_get(struct kcoro_work *work)
{
    return work->state;
}

static inline void kcoro_state_set(struct kcoro_work *work, enum kcoro_state state)
{
    work->state = state;
}

static inline void kcoro_dyprio_inc(struct kcoro_work *work)
{
    if (work->dyprio < 39)
        work->dyprio++;
}

static inline void kcoro_dyprio_dec(struct kcoro_work *work)
{
    if (work->dyprio > 0)
        work->dyprio--;
}

static inline void kcoro_dyprio_reset(struct kcoro_work *work)
{
    if (work->dyprio != work->prio)
        work->dyprio = work->prio;
}

/* kcoro delay */
extern void kcoro_ndelay(struct kcoro_work *work, unsigned int nsec);
extern void kcoro_udelay(struct kcoro_work *work, unsigned int usec);
extern void kcoro_mdelay(struct kcoro_work *work, unsigned int msec);
extern void kcoro_tsleep(struct kcoro_work *work, ttime_t timeout);
extern void kcoro_msleep(struct kcoro_work *work, unsigned int msec);

/* kcoro schedule */
extern struct kcoro_work *kcoro_pick(struct kcoro_worker *worker);
extern void kcoro_timeout_wakeup(struct kcoro_worker *worker);
extern void kcoro_enqueue(struct kcoro_worker *worker, struct kcoro_work *work);
extern void kcoro_dequeue(struct kcoro_worker *worker, struct kcoro_work *work);
extern void kcoro_activate(struct kcoro_worker *worker, struct kcoro_work *work);

extern struct kcoro_work *kcoro_function(struct kcoro_worker *worker);
extern state kcoro_schedule(struct kcoro_worker *worker);

/* kcoro dispatch */
extern void kcoro_prio(struct kcoro_work *work, int prio);
extern void kcoro_yield(struct kcoro_work *work);
extern void __noreturn kcoro_exit(struct kcoro_work *work, state retval);

/* kcoro worker work */
extern struct kcoro_work *kcoro_work_create(struct kcoro_worker *worker, kcoro_entry_t entry, void *pdata);
extern void kcoro_work_destroy(struct kcoro_work *work);
extern struct kcoro_worker *kcoro_worker_create(void);
extern void kcoro_worker_destroy(struct kcoro_worker *worker);
extern void __init kcoro_init(void);

#endif  /* _KCORO_H_ */
