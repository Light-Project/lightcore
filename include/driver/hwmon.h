/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_H_
#define _DRIVER_HWMON_H_

#include <device.h>

enum hwmon_sensor {
    HWMON_SENSOR_THERMAL,
    HWMON_SENSOR_FAN,
};

enum hwmon_thermal_attributes {
    HWMON_THERMAL_ENABLE,
    HWMON_THERMAL_LABEL,
    HWMON_THERMAL_DATA,
    HWMON_THERMAL_MIX,
    HWMON_THERMAL_MIX_HYST,
    HWMON_THERMAL_MAX,
    HWMON_THERMAL_MAX_HYST,
    HWMON_THERMAL_CRIT,
    HWMON_THERMAL_CRIT_HYST,
};

#define HWMON_THERMAL_HAS_ENABLE    BIT(HWMON_THERMAL_ENABLE)
#define HWMON_THERMAL_HAS_LABEL     BIT(HWMON_THERMAL_LABEL)
#define HWMON_THERMAL_HAS_DATA      BIT(HWMON_THERMAL_DATA)
#define HWMON_THERMAL_HAS_MIX       BIT(HWMON_THERMAL_MIX)
#define HWMON_THERMAL_HAS_MIX_HYST  BIT(HWMON_THERMAL_MIX_HYST)
#define HWMON_THERMAL_HAS_MAX       BIT(HWMON_THERMAL_MAX)
#define HWMON_THERMAL_HAS_MAX_HYST  BIT(HWMON_THERMAL_MAX_HYST)
#define HWMON_THERMAL_HAS_CRIT      BIT(HWMON_THERMAL_CRIT)
#define HWMON_THERMAL_HAS_CRIT_HYST BIT(HWMON_THERMAL_CRIT_HYST)

struct hwmon_channel_info {
    enum hwmon_sensor sensor;
    const uint32_t *channels;
};

#define HWMON_CHANNEL_INFO(stype, ...) {    \
    .sensor = (stype),                      \
    .channels = (uint32_t []) {             \
        __VA_ARGS__ 0                       \
    },                                      \
}

struct hwmon_device {
    struct device *dev;
    struct hwmon_ops *ops;
    const struct hwmon_channel_info *info;
};

struct hwmon_ops {
    state (*string)(struct hwmon_device *, unsigned int channel, uint32_t att, const char **str);
    state (*read)(struct hwmon_device *, unsigned int channel, uint32_t att, ssize_t *val);
    state (*write)(struct hwmon_device *, unsigned int channel, uint32_t att, ssize_t val);
};

extern state hwmon_register(struct hwmon_device *);
extern void hwmon_unregister(struct hwmon_device *);

#endif  /* _DRIVER_HWMON_H_ */
