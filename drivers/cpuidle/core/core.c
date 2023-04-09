/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/cpuidle.h>
#include <export.h>

MUTEX_LOCK(cpuidle_lock);
LIST_HEAD(cpuidle_detected_devices);
DEFINE_PERCPU(struct cpuidle_device *, cpuidle_devices);

bool cpuidle_available(struct cpuidle_device *cdev)
{
    return cdev && cdev->driver && cdev->enabled;
}

state cpuidle_device_enable(struct cpuidle_device *cdev)
{
    state retval;

    if (cdev->enabled)
        return -ENOERR;

    if (cpuidle_governor_curr->activate) {
        retval = cpuidle_governor_curr->activate(cdev);
        if (retval)
            return retval;
    }

    cdev->enabled = true;
    return -ENOERR;
}
EXPORT_SYMBOL(cpuidle_device_enable);

void cpuidle_device_disable(struct cpuidle_device *cdev)
{
    if (!cdev->enabled)
        return;

    if (cpuidle_governor_curr->deactivate)
        cpuidle_governor_curr->deactivate(cdev);

    cdev->enabled = false;
}
EXPORT_SYMBOL(cpuidle_device_disable);

state cpuidle_device_register(struct cpuidle_device *cdev)
{
    state retval;

    if (!cdev->driver)
        return -EINVAL;

    mutex_lock(&cpuidle_lock);
    percpu(cdev->cpu, cpuidle_devices) = cdev;
    list_add(&cpuidle_detected_devices, &cdev->detected_list);
    retval = cpuidle_device_enable(cdev);
    mutex_unlock(&cpuidle_lock);

    return retval;
}
EXPORT_SYMBOL(cpuidle_device_register);

void cpuidle_device_unregister(struct cpuidle_device *cdev)
{
    mutex_lock(&cpuidle_lock);
    percpu(cdev->cpu, cpuidle_devices) = NULL;
    list_del(&cdev->detected_list);
    cpuidle_device_disable(cdev);
    mutex_unlock(&cpuidle_lock);
}
EXPORT_SYMBOL(cpuidle_device_unregister);
