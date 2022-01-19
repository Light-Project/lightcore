/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "kcoro: " fmt

#include <kcoro.h>
#include <task.h>
#include <export.h>
#include <panic.h>
#include <printk.h>

static LIST_HEAD(worker_list);

static void kcoro_dispatch(struct kcoro_worker *worker, struct kcoro_work *work)
{
    kcoro_state_t retval;
    void *priv = work->data;

    retval = work->entry(work, priv);

    switch (retval) {
        case KCORO_EXITED:
            kcoro_work_dequeue(work);
            break;
        case KCORO_BLOCK:
            break;
        default:
            kcoro_debug("kcoro work return unknow value\n");
    }
}

/**
 * kcoro_current - gets the currently coroutine
 * @return: current coroutine work
 *
 * NOTE, must on the coroutine thread to use this function
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

void kcoro_work_exit(void)
{
    struct kcoro_work *work = kcoro_current;

}
EXPORT_SYMBOL(kcoro_work_exit);

void kcoro_relax(void)
{
    struct kcoro_worker *worker = work->worker;
    list_move_tail(&worker->work_list, &work->list);
}
EXPORT_SYMBOL(kcoro_relax);

void kcoro_work_enqueue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    work->worker = worker;
    worker->work_num++;
    list_add(&worker->work_list, &work->list);
}

void kcoro_work_dequeue(struct kcoro_work *work)
{
    struct kcoro_worker *worker = work->worker;

    work->worker = NULL;
    worker->work_num--;
    list_del(&work->list);
}

struct kcoro_worker *kcoro_worker_best(void)
{
    struct kcoro_worker *walk, *best = NULL;

    list_for_each_entry(walk, &worker_list, list)
        if (!best || best->work_num > walk->work_num)
            best = walk;

    return best;
}

int kcoro_worker_wakeup(struct kcoro_worker *worker, kcoro_pth_flag *flags)
{
    return kcoro_pthread_create(&worker->pth, flags,
                                kcoro_worker_loop, worker);
}

struct kcoro_work *kcoro_work_create(kcoro_fun_t fun, void *data, const char *name, ...)
{
    struct kcoro_work *work;

    work = kcoro_zalloc(sizeof(*work));
    if (!work)
        return NULL;

    work->name = name;
    work->entry = fun;
    work->data = data;
    return work;
}

void kcoro_work_destroy(struct kcoro_work *work)
{
    if (work->worker)
        kcoro_work_dequeue(work);
    kcoro_free(work);
}

void kcoro_worker_loop(void *data)
{
    struct kcoro_worker *worker = data;
    struct kcoro_work *work;

    for (;;)
    list_for_each_entry(work, &worker->work_list, list)
        kcoro_dispatch(worker, work);
}

struct kcoro_worker *kcoro_worker_create(const char *name)
{
    struct kcoro_worker *worker;

    worker = kzalloc(sizeof(*worker), GFP_KERNEL);
    if (!worker)
        return NULL;

    worker->name = name;

    list_add_prev(&worker_list, &worker->list);
    list_head_init(&worker->work_list);
    return worker;
}

void kcoro_worker_destroy(struct kcoro_worker *worker)
{
    kcoro_free(worker);
}
