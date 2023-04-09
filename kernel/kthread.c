/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kthread"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kthread.h>
#include <kmalloc.h>
#include <task.h>
#include <export.h>

struct kthread_create {
    struct list_head list;
    state (*entry)(void *pdata);
    void *pdata;
};

static struct kcache *worker_cache;
static struct kcache *work_cache;

static LIST_HEAD(kthread_list);
static LIST_HEAD(kthread_create_list);
static SPIN_LOCK(kthread_lock);
static SPIN_LOCK(kthread_create_lock);

static inline state kthread_entry(void *pdata)
{


    return -ENOERR;
}

state __noreturn kthread_daemon(void *pdata)
{
    for (;;) {
        current_set_state(SCHED_TASK_INTERRUPTIBLE);
        if (list_check_empty(&kthread_create_list))
            sched_resched();

        current_set_state(SCHED_TASK_RUNNING);

        spin_lock(&kthread_create_lock);
        while (!list_check_empty(&kthread_create_list)) {
            struct kthread_create *create;

            create = list_first_entry(&kthread_create_list, typeof(*create), list);
            list_del_init(&create->list);

            spin_unlock(&kthread_create_lock);
            kernel_clone(0, create->entry, create->pdata);
            spin_lock(&kthread_create_lock);
        }
        spin_unlock(&kthread_create_lock);
    }
}

state kthread_work_enqueue(struct kthread_worker *worker, struct kthread_work *work)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&worker->lock, &irqflags);


    spin_unlock_irqrestore(&worker->lock, &irqflags);
    return -ENOERR;
}

void kthread_work_dequeue(struct kthread_worker *worker, struct kthread_work *work)
{

}

struct kthread_work *kthread_work_create(kthread_entry_t entry, void *pdata)
{
    struct kthread_work *work;

    work = kcache_zalloc(work_cache, GFP_KERNEL);
    if (!work)
        return NULL;

    work->entry = entry;
    work->pdata = pdata;

    return work;
}
EXPORT_SYMBOL(kthread_work_create);

void kthread_work_destroy(struct kthread_work *work)
{
    struct kthread_worker *worker = work->worker;

    if (worker)
        kthread_work_dequeue(worker, work);

    kcache_free(work_cache, work);
}
EXPORT_SYMBOL(kthread_work_destroy);

struct kthread_worker *kthread_worker_vcreate(const char *name, va_list args)
{
    struct kthread_worker *worker;

    worker = kcache_zalloc(worker_cache, GFP_KERNEL);
    if (!worker)
        return NULL;

    spin_lock(&kthread_lock);
    list_add(&kthread_list, &worker->list);
    spin_unlock(&kthread_lock);

    return worker;
}
EXPORT_SYMBOL(kthread_worker_vcreate);

struct kthread_worker *kthread_worker_create(const char *namefmt, ...)
{
    struct kthread_worker *worker;
    va_list args;

    va_start(args, namefmt);
    worker = kthread_worker_vcreate(namefmt, args);
    va_end(args);

    return worker;
}
EXPORT_SYMBOL(kthread_worker_create);

void __init kthread_init(void)
{
    worker_cache = KCACHE_CREATE(struct kthread_worker, KCACHE_PANIC);
    work_cache = KCACHE_CREATE(struct kthread_work, KCACHE_PANIC);
}
