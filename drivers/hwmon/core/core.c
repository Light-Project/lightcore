/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/hwmon.h>
#include <export.h>



state hwmon_register(struct hwmon_device *hdev)
{
    if (!hdev->dev || !hdev->ops)
        return -EINVAL;

    return -ENOERR;
}

void hwmon_unregister(struct hwmon_device *hdev)
{

}

EXPORT_SYMBOL(hwmon_register);
EXPORT_SYMBOL(hwmon_unregister);
