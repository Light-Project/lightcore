/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kclock.h>
#include <driver/cpuidle.h>
#include <export.h>

unsigned int cpuidle_select(struct cpuidle_device *cdev)
{
    struct cpuidle_governor *governor = cpuidle_governor_curr;
    return governor->select(cdev);
}
EXPORT_SYMBOL(cpuidle_select);

void cpuidle_reflect(struct cpuidle_device *cdev, unsigned int index)
{
    struct cpuidle_governor *governor = cpuidle_governor_curr;
    if (governor->reflect)
        governor->reflect(cdev, index);
}
EXPORT_SYMBOL(cpuidle_reflect);

void cpuidle_activate(struct cpuidle_device *cdev)
{
    struct cpuidle_governor *governor = cpuidle_governor_curr;
    if (governor->activate)
        governor->activate(cdev);
}
EXPORT_SYMBOL(cpuidle_activate);

void cpuidle_deactivate(struct cpuidle_device *cdev)
{
    struct cpuidle_governor *governor = cpuidle_governor_curr;
    if (governor->deactivate)
        governor->deactivate(cdev);
}
EXPORT_SYMBOL(cpuidle_deactivate);

state cpuidle_enter(struct cpuidle_device *cdev, unsigned int *index)
{
    struct cpuidle_driver *cdrv = cdev->driver;
	const struct cpuidle_state *state = &cdrv->states[*index];
    uint64_t start_time, end_time;

    start_time = kclock_get();
    state->enter(cdev, index);
    end_time = kclock_get();

    if (*index > 0) {
        uint64_t diff;

        diff = end_time - start_time;
        cdev->last_residency = diff;
    }

    return -ENOERR;
}
EXPORT_SYMBOL(cpuidle_enter);
