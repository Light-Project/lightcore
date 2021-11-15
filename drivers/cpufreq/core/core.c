/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "cpufreq: " fmt

#include <driver/cpufreq.h>
#include <export.h>

state cpufreq_register(struct cpufreq_device *cpufreq)
{
    return -ENOERR;
}

void cpufreq_unregister(struct cpufreq_device *cpufreq)
{

}

state cpufreq_sched_register(struct cpufreq_sched *sched)
{
    return -ENOERR;
}

void cpufreq_sched_unregister(struct cpufreq_sched *sched)
{

}
