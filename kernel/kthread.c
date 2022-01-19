/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kthread.h>
#include <kmalloc.h>
#include <sched.h>

static LIST_HEAD(kthread_list);

state kthread_entry(void *data)
{
    struct kthread_worker *worker = data;
    struct kthread_work *work;

    list_for_each_entry(work, &worker->work, list) {

    }

}

state kthread_work_enqueue()
{

}

struct kthread_worker *
kthread_worker_create(kthread_t kthread, void *data, const char *name)
{
    struct kthread_worker *worker;

    task = kzalloc(sizeof(*task), GFP_KERNEL);
    if (task)
        return PTR_ERR(task);

    list_add(&kthread_list, task->);

    return task;
}
