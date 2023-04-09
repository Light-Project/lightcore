/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/hwmon.h>
#include <export.h>

state hwmon_device_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                        unsigned int index, uint32_t att, ssize_t *val)
{
    struct hwmon_ops *ops = hdev->ops;

    if (!ops->read)
        return -EOPNOTSUPP;

    return ops->read(hdev, sensor, index, att, val);
}
EXPORT_SYMBOL(hwmon_device_read);

state hwmon_device_write(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                         unsigned int index, uint32_t att, ssize_t val)
{
    struct hwmon_ops *ops = hdev->ops;

    if (!ops->write)
        return -EOPNOTSUPP;

    return ops->write(hdev, sensor, index, att, val);
}
EXPORT_SYMBOL(hwmon_device_write);

state hwmon_device_string(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                         unsigned int index, uint32_t att, const char **str)
{
    struct hwmon_ops *ops = hdev->ops;

    if (!ops->string)
        return -EOPNOTSUPP;

    return ops->string(hdev, sensor, index, att, str);
}
EXPORT_SYMBOL(hwmon_device_string);
