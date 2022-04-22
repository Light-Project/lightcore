/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KCORO_H_
#define _KCORO_H_

#include <task.h>
#include <rbtree.h>
#include <kcontext.h>

enum kcoro_state {
    KCORO_READY,
    KCORO_RUNNING,
    KCORO_EXITED,
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
    char name[TASK_NAME_LEN];

    struct kcontext context;
    enum kcoro_state state;

    ktime_t start;
    uint64_t vtime;
    int prio;
    int dyprio;
    void *pdata;
};

/**
 * kcoro_worker - describe a kcoroutine worker.
 * @lock: used to lock the red black tree and task status.
 * @list: list node for system management.
 * @task: point to the parent task.
 * @ready: ready queue ont this worker.
 * @retc: context use for task return.
 * @curr: current running kcoro work.
 * @min_vtime: minimum virtual time in this worker.
 * @weight: worker weight.
 */
struct kcoro_worker {
    spinlock_t lock;
    struct list_head list;
    struct sched_task *task;
    struct rb_root_cached ready;

    struct kcontext retc;
    struct kcoro_work *curr;
    uint64_t min_vtime;
    uint64_t weight;
};

typedef state (*kcoro_entry_t)(void *pdata);

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

/* Kernel coroutine sleep */
extern void kcoro_nsleep(unsigned int nsec);
extern void kcoro_usleep(unsigned int usec);
extern void kcoro_msleep(unsigned int msec);

/* Kernel coroutine dispatch */
extern struct kcoro_work *kcoro_current(void);
extern void kcoro_work_prio(int prio);
extern void kcoro_dispatch(struct kcoro_worker *worker);
extern void kcoro_yield(void);
extern void kcoro_exit(void);

/* Kernel coroutine create */
extern struct kcoro_worker *kcoro_worker_vcreate(const char *namefmt, va_list args);
extern struct kcoro_worker *kcoro_worker_create(const char *namefmt, ...);
extern void kcoro_worker_destroy(struct kcoro_worker *worker);
extern struct kcoro_work *kcoro_work_vcreate(struct kcoro_worker *worker, kcoro_entry_t entry, void *pdata, const char *namefmt, va_list args);
extern struct kcoro_work *kcoro_work_create(struct kcoro_worker *worker, kcoro_entry_t entry, void *pdata, const char *namefmt, ...);
extern void kcoro_work_destroy(struct kcoro_work *work);
extern void __init kcoro_init(void);

#endif  /* _KCORO_H_ */
