/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KCORO_H_
#define _KCORO_H_

#include <list.h>
#include <bitmap.h>
#include <kthread.h>
#include <kcontext.h>

#define KCORO_PRIO_MAX  64

struct kcoro_work {
    struct list_head list;
    struct kcoro_worker *worker;
    struct kcontext context;
    void *pdata;
};

struct kcoro_worker {
    const char *name;
    struct list_head list;
    DEFINE_BITMAP(bitmap, KCORO_PRIO_MAX);
    struct list_head work_list[KCORO_PRIO_MAX];
    struct kcoro_work *curr;
};

/* kcoro work operations */
extern struct kcoro_work *kcoro_work_create(state (*entry)(void), void *data, ...);
extern void kcoro_work_destroy(struct kcoro_work *work);
extern void kcoro_work_enqueue(struct kcoro_worker *worker, struct kcoro_work *work);
extern void kcoro_work_dequeue(struct kcoro_work *work);
extern void kcoro_work_relax(void);
extern void kcoro_work_exit(void);

/* kcoro worker operations */
extern struct kcoro_worker *kcoro_worker_create(const char *name, ...);
extern void kcoro_worker_remove(struct kcoro_worker *worker);
extern void *kcoro_worker_loop(void *data);
extern int kcoro_worker_wakeup(struct kcoro_worker *worker);
extern void kcoro_worker_suspend(struct kcoro_worker *worker);

#endif  /* _KCORO_H_ */
