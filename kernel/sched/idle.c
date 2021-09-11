/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <sched.h>

static struct sched_type idle_sched = {
    .name = "idle",
    .priority = 255,
};

static state idle_sched_init(void)
{
    return sched_register(&idle_sched);
}
sched_initcall(idle_sched_init);