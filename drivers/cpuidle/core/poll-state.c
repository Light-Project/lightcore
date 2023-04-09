/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kclock.h>
#include <ticktime.h>
#include <sched.h>
#include <driver/cpuidle.h>
#include <export.h>

#define CPUIDLE_POLL_RELAX 200
#define CPUIDLE_POLL_MIN 10000
#define CPUIDLE_POLL_MAX (SYSTICK_NSEC / 16)

uint64_t cpuidle_poll_time(struct cpuidle_device *cdev)
{
    struct cpuidle_driver *cdrv = cdev->driver;
    uint64_t residency, limit;
    unsigned int index;

    if (likely(cdev->poll_limit))
        return cdev->poll_limit;

    limit = CPUIDLE_POLL_MAX;
    for (index = 1; index < cdrv->ladders; ++index) {
        residency = cdrv->states[index].target_residency;
        if (residency < CPUIDLE_POLL_MIN)
            continue;
        limit = min(residency, CPUIDLE_POLL_MAX);
        break;
    }

    cdev->poll_limit = limit;
    return limit;
}
EXPORT_SYMBOL(cpuidle_poll_time);

static __cpuidle state poll_entry(struct cpuidle_device *cdev, unsigned int *index)
{
    uint64_t limit, start_time = kclock_get();
    unsigned int count = 0;

    cdev->poll_allow_limit = false;
    irq_local_enable();
    limit = cpuidle_poll_time(cdev);

    while (!current_test_resched()) {
        cpu_relax();
        if (count++ < CPUIDLE_POLL_RELAX)
            continue;

        count = 0;
        if (kclock_get() - start_time > limit) {
            cdev->poll_allow_limit = true;
            break;
        }
    }

    return -ENOERR;
}

static const struct cpuidle_state cpuidle_poll_state = {
    .name = "poll",
    .name = "cpuidle poll idle",
    .target_residency = 0,
    .exit_latency = 0,
    .power_usage = -1,
    .enter = poll_entry,
};

void cpuidle_poll_init(struct cpuidle_state *state)
{
    *state = cpuidle_poll_state;
}
EXPORT_SYMBOL(cpuidle_poll_init);
