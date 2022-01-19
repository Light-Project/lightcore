/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_H_
#define _DRIVER_HWMON_H_

#include <device.h>

enum hwmon_sensor {
    HWMON_SENSOR_THERMAL,
    HWMON_SENSOR_FAN,
    HWMON_SENSOR_PWM,
};

enum hwmon_thermal_attributes {
    HWMON_THERMAL_ENABLE,
    HWMON_THERMAL_LABEL,
    HWMON_THERMAL_DATA,
    HWMON_THERMAL_MIN,
    HWMON_THERMAL_MIN_HYST,
    HWMON_THERMAL_MAX,
    HWMON_THERMAL_MAX_HYST,
    HWMON_THERMAL_CRIT,
    HWMON_THERMAL_CRIT_HYST,
};

#define HWMON_THERMAL_HAS_ENABLE    BIT(HWMON_THERMAL_ENABLE)
#define HWMON_THERMAL_HAS_LABEL     BIT(HWMON_THERMAL_LABEL)
#define HWMON_THERMAL_HAS_DATA      BIT(HWMON_THERMAL_DATA)
#define HWMON_THERMAL_HAS_MIN       BIT(HWMON_THERMAL_MIN)
#define HWMON_THERMAL_HAS_MIN_HYST  BIT(HWMON_THERMAL_MIN_HYST)
#define HWMON_THERMAL_HAS_MAX       BIT(HWMON_THERMAL_MAX)
#define HWMON_THERMAL_HAS_MAX_HYST  BIT(HWMON_THERMAL_MAX_HYST)
#define HWMON_THERMAL_HAS_CRIT      BIT(HWMON_THERMAL_CRIT)
#define HWMON_THERMAL_HAS_CRIT_HYST BIT(HWMON_THERMAL_CRIT_HYST)

enum hwmon_fan_attributes {
    HWMON_FAN_ENABLE,
    HWMON_FAN_LABEL,
    HWMON_FAN_DATA,
    HWMON_FAN_MIN,
    HWMON_FAN_MAX,
    HWMON_FAN_TARGET,
};

#define HWMON_FAN_HAS_ENABLE        BIT(HWMON_FAN_ENABLE)
#define HWMON_FAN_HAS_LABEL         BIT(HWMON_FAN_LABEL)
#define HWMON_FAN_HAS_DATA          BIT(HWMON_FAN_DATA)
#define HWMON_FAN_HAS_MIN           BIT(HWMON_FAN_MIN)
#define HWMON_FAN_HAS_MAX           BIT(HWMON_FAN_MAX)
#define HWMON_FAN_HAS_TARGET        BIT(HWMON_FAN_TARGET)

enum hwmon_pwm_attributes {
    HWMON_PWM_ENABLE,
    HWMON_PWM_LABEL,
    HWMON_PWM_DATA,
    HWMON_PWM_MODE,
    HWMON_PWM_FREQ,
};

#define HWMON_PWM_HAS_ENABLE        BIT(HWMON_PWM_ENABLE)
#define HWMON_PWM_HAS_LABEL         BIT(HWMON_PWM_LABEL)
#define HWMON_PWM_HAS_DATA          BIT(HWMON_PWM_DATA)
#define HWMON_PWM_HAS_MODE          BIT(HWMON_PWM_MODE)
#define HWMON_PWM_HAS_FREQ          BIT(HWMON_PWM_FREQ)

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
    state (*string)(struct hwmon_device *, enum hwmon_sensor, unsigned int channel, uint32_t att, const char **str);
    state (*read)(struct hwmon_device *, enum hwmon_sensor, unsigned int channel, uint32_t att, ssize_t *val);
    state (*write)(struct hwmon_device *, enum hwmon_sensor, unsigned int channel, uint32_t att, ssize_t val);
};

extern state hwmon_register(struct hwmon_device *);
extern void hwmon_unregister(struct hwmon_device *);

#endif  /* _DRIVER_HWMON_H_ */
