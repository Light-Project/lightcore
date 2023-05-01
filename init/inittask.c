/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>

struct sched_task init_task = {
    .child = LIST_HEAD_INIT(init_task.child),
    .stack = init_stack,
    .namespace = &root_ns,
    .sched_type = &idle_sched,
    .flags = SCHED_TASK_KTHREAD,
};
