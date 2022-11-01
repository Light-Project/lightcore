/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <time.h>
#include <proc.h>
#include <initcall.h>
#include <driver/cpuidle.h>

static DEFINE_PERCPU(struct cpuidle_device, haltpoll_devices);

static state haltpoll_idle(struct cpuidle_device *cdev, unsigned int *index)
{
    cpu_idle();
    return -ENOERR;
}

static struct cpuidle_state haltpoll_state[] = {
    { }, {
        .name = "haltpoll idle",
        .desc = "architecture halt",
        .target_residency = NSEC_PER_USEC,
        .exit_latency = NSEC_PER_USEC,
        .power_usage = -1,
        .enter = haltpoll_idle,
    },
};

static struct cpuidle_driver haltpoll_driver = {
    .name = "haltpoll",
    .states = haltpoll_state,
    .ladders = ARRAY_SIZE(haltpoll_state),
};

static state haltpoll_init(void)
{
    unsigned int count;
    state retval = -ENOERR;

    cpuidle_poll_init(haltpoll_state);
    cpu_for_each(count) {
        struct cpuidle_device *cdev;

        cdev = percpu_ptr(count, &haltpoll_devices);
        cdev->cpu = count;
        cdev->driver = &haltpoll_driver;
        retval = cpuidle_device_register(cdev);

        if (retval)
            break;
    }

    return retval;
}
driver_initcall(haltpoll_init);
