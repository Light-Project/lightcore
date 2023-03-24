/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTHREAD_H_
#define _KTHREAD_H_

#include <sched.h>

typedef state (*kthread_entry_t)(void *pdata);

struct kthread_worker {
    spinlock_t lock;
    struct list_head list;
    struct list_head works;
    struct kthread_work *curr;
};

struct kthread_work {
    struct list_head list;
    struct kthread_worker *worker;
    kthread_entry_t entry;
    void *pdata;
};

extern struct kthread_worker *kthread_worker_vcreate(const char *namefmt, va_list args);
extern struct kthread_worker *kthread_worker_create(const char *namefmt, ...);
extern void kthread_worker_destroy(struct kthread_worker *worker);

extern struct kthread_work *kthread_work_create(kthread_entry_t entry, void *pdata);
extern void kthread_work_destroy(struct kthread_work *work);
extern state kthread_work_enqueue(struct kthread_worker *worker, struct kthread_work *work);
extern void kthread_work_dequeue(struct kthread_worker *worker, struct kthread_work *work);

extern state kthread_daemon(void *pdata);

#endif /* _KTHREAD_H_ */
