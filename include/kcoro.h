/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KCORO_H_
#define _KCORO_H_

#include <task.h>
#include <rbtree.h>
#include <kcontext.h>

enum kcoro_state {
    KCORO_READY,
    KCORO_EXITED,
};

/**
 * kcoro_work -
 *
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
 * kcoro_worker -
 *
 */
struct kcoro_worker {
    struct list_head list;
    struct sched_task *task;
    struct rb_root_cached ready;
    struct kcontext retc;
    struct kcoro_work *curr;
    uint64_t min_vtime;
    uint64_t weight;
};

typedef state (*kcoro_entry_t)(void *pdata);

extern struct kcoro_worker *kcoro_worker_create(const char *name, ...);
extern void kcoro_worker_destroy(struct kcoro_worker *worker);
extern struct kcoro_work *kcoro_work_create(struct kcoro_worker *worker, kcoro_entry_t entry, void *pdata, const char *namefmt, ...);
extern void kcoro_work_destroy(struct kcoro_work *work);
extern void kcoro_dispatch(void);
extern void kcoro_work_prio(int prio);
extern void kcoro_msleep(unsigned int msec);
extern void kcoro_yield(void);
extern void kcoro_exit(void);
extern void __init kcoro_init(void);

#endif  /* _KCORO_H_ */
