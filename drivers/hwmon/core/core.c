/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/hwmon.h>
#include <export.h>

SPIN_LOCK(hwmon_lock);
LIST_HEAD(hwmon_list);

state hwmon_register(struct hwmon_device *hdev)
{
    if (!hdev->dev || !hdev->ops)
        return -EINVAL;

    spin_lock(&hwmon_lock);
    list_add(&hwmon_list, &hdev->list);
    spin_unlock(&hwmon_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(hwmon_register);

void hwmon_unregister(struct hwmon_device *hdev)
{
    spin_lock(&hwmon_lock);
    list_del(&hdev->list);
    spin_unlock(&hwmon_lock);
}
EXPORT_SYMBOL(hwmon_unregister);
