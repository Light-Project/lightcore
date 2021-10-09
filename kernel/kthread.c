/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <kthread.h>

struct task *kthread_create(kthread_t kthread, void *data,
                            const char *name, ...)
{
    struct task *task;

    task = kzalloc(sizeof(*task), GFP_KERNEL);
    if (task)


    return task;
}
