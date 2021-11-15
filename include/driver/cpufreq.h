/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CPUFREQ_H_
#define _DRIVER_CPUFREQ_H_

#include <types.h>
#include <state.h>
#include <device.h>

struct cpufreq_policy {

};

struct cpufreq_device {
    struct cpufreq_ops *ops;
};

struct cpufreq_ops {

};

struct cpufreq_sched {
    const char *name;
    struct list_head list;

};

extern state cpufreq_sched_register(struct cpufreq_sched *);
extern void cpufreq_sched_unregister(struct cpufreq_sched *);

#endif  /* _DRIVER_CPUFREQ_H_ */
