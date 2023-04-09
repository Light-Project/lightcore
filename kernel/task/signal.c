/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>
#include <wait.h>
#include <syscall.h>
#include <export.h>

state kernel_pause(void)
{
    while (!current_test_notify_signal()) {
        current_set_state(SCHED_TASK_INTERRUPTIBLE);
        sched_yield();
    }
    return -ERESTARTNOHAND;
}
