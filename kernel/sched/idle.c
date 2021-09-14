/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <sched.h>

void idle_task()
{

}

static struct sched_type idle_sched = {
    .name = "idle",
    .priority = SCHED_IDLE_PRIO,
};

static state idle_sched_init(void)
{
    return sched_register(&idle_sched);
}
scheduler_initcall(idle_sched_init);