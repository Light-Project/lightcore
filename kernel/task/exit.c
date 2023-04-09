/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>

void __noreturn kernel_exit(state exit_code)
{
    struct sched_task *curr = current;

    curr->exit_code = exit_code;
    sched_kill();
}
