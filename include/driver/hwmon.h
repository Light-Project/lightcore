/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_H_
#define _DRIVER_HWMON_H_

#include <device.h>

enum hwmon_sensor {
    HWMON_SENSOR_IN = 1,
    HWMON_SENSOR_THERMAL,
    HWMON_SENSOR_HUMIDITY,
    HWMON_SENSOR_FAN,
    HWMON_SENSOR_PWM,
};

enum hwmon_in_attributes {
    HWMON_IN_LABEL,
    HWMON_IN_ENABLE,
    HWMON_IN_DATA,
    HWMON_IN_MIN,
    HWMON_IN_MAX,
    HWMON_IN_CRIT,
    HWMON_IN_AVERAGE,
    HWMON_IN_LOWEST,
    HWMON_IN_HIGHEST,
    HWMON_IN_ALARM,
    HWMON_IN_MIN_ALARM,
    HWMON_IN_MAX_ALARM,
    HWMON_IN_CRIT_ALARM,
    HWMON_IN_RATED_MIN,
    HWMON_IN_RATED_MAX,
};

#define HWMON_IN_HAS_LABEL          BIT(HWMON_IN_LABEL)
#define HWMON_IN_HAS_ENABLE         BIT(HWMON_IN_ENABLE)
#define HWMON_IN_HAS_DATA           BIT(HWMON_IN_DATA)
#define HWMON_IN_HAS_MIN            BIT(HWMON_IN_MIN)
#define HWMON_IN_HAS_MAX            BIT(HWMON_IN_MAX)
#define HWMON_IN_HAS_CRIT           BIT(HWMON_IN_CRIT)
#define HWMON_IN_HAS_AVERAGE        BIT(HWMON_IN_AVERAGE)
#define HWMON_IN_HAS_LOWEST         BIT(HWMON_IN_LOWEST)
#define HWMON_IN_HAS_HIGHEST        BIT(HWMON_IN_HIGHEST)
#define HWMON_IN_HAS_ALARM          BIT(HWMON_IN_ALARM)
#define HWMON_IN_HAS_MIN_ALARM      BIT(HWMON_IN_MIN_ALARM)
#define HWMON_IN_HAS_MAX_ALARM      BIT(HWMON_IN_MAX_ALARM)
#define HWMON_IN_HAS_CRIT_ALARM     BIT(HWMON_IN_CRIT_ALARM)
#define HWMON_IN_HAS_RATED_MIN      BIT(HWMON_IN_RATED_MIN)
#define HWMON_IN_HAS_RATED_MAX      BIT(HWMON_IN_RATED_MAX)

enum hwmon_thermal_attributes {
    HWMON_THERMAL_LABEL,
    HWMON_THERMAL_ENABLE,
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

enum hwmon_humidity_attributes {
    HWMON_HUMIDITY_LABEL,
    HWMON_HUMIDITY_ENABLE,
    HWMON_HUMIDITY_DATA,
    HWMON_HUMIDITY_MIN,
    HWMON_HUMIDITY_MIN_HYST,
    HWMON_HUMIDITY_MAX,
    HWMON_HUMIDITY_MAX_HYST,
};

#define HWMON_HUMIDITY_HAS_ENABLE   BIT(HWMON_HUMIDITY_ENABLE)
#define HWMON_HUMIDITY_HAS_LABEL    BIT(HWMON_HUMIDITY_LABEL)
#define HWMON_HUMIDITY_HAS_DATA     BIT(HWMON_HUMIDITY_DATA)
#define HWMON_HUMIDITY_HAS_MIN      BIT(HWMON_HUMIDITY_MIN)
#define HWMON_HUMIDITY_HAS_MIN_HYST BIT(HWMON_HUMIDITY_MIN_HYST)
#define HWMON_HUMIDITY_HAS_MAX      BIT(HWMON_HUMIDITY_MAX)
#define HWMON_HUMIDITY_HAS_MAX_HYST BIT(HWMON_HUMIDITY_MAX_HYST)

enum hwmon_fan_attributes {
    HWMON_FAN_LABEL,
    HWMON_FAN_ENABLE,
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
    HWMON_PWM_LABEL,
    HWMON_PWM_ENABLE,
    HWMON_PWM_DATA,
    HWMON_PWM_MIN,
    HWMON_PWM_MAX,
    HWMON_PWM_RAMP,
    HWMON_PWM_MODE,
    HWMON_PWM_FREQ,
};

#define HWMON_PWM_HAS_ENABLE        BIT(HWMON_PWM_ENABLE)
#define HWMON_PWM_HAS_LABEL         BIT(HWMON_PWM_LABEL)
#define HWMON_PWM_HAS_DATA          BIT(HWMON_PWM_DATA)
#define HWMON_PWM_HAS_MIN           BIT(HWMON_PWM_MIN)
#define HWMON_PWM_HAS_MAX           BIT(HWMON_PWM_MAX)
#define HWMON_PWM_HAS_RAMP          BIT(HWMON_PWM_RAMP)
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

/**
 * struct hwmon_device - describe a hwmon device.
 * @dev: points to the parent device of the hwmon device.
 * @ops: operations method of hwmon device.
 * @list: linked list for centralized management of hwmon device.
 * @info: channels information of hwmon device.
 * @pdata: private data of hwmon device.
 */
struct hwmon_device {
    struct device *dev;
    struct hwmon_ops *ops;
    struct list_head list;
    struct hwmon_channel_info *info;
    void *pdata;
};

/**
 * struct hwmon_ops - describe the operations of a hwmon.
 * @read: read the attribute value of a channel of the hwmon device.
 * @write: write the attribute value of a channel of the hwmon device.
 * @string: read the attribute string of a channel of the hwmon device.
 */
struct hwmon_ops {
    state (*read)(struct hwmon_device *hdev, enum hwmon_sensor sensor, unsigned int index, uint32_t att, ssize_t *val);
    state (*write)(struct hwmon_device *hdev, enum hwmon_sensor sensor, unsigned int index, uint32_t att, ssize_t val);
    state (*string)(struct hwmon_device *hdev, enum hwmon_sensor sensor, unsigned int index, uint32_t att, const char **str);
};

extern spinlock_t hwmon_lock;
extern struct list_head hwmon_list;

extern state hwmon_device_read(struct hwmon_device *hdev, enum hwmon_sensor sensor, unsigned int index, uint32_t att, ssize_t *val);
extern state hwmon_device_write(struct hwmon_device *hdev, enum hwmon_sensor sensor, unsigned int index, uint32_t att, ssize_t val);
extern state hwmon_device_string(struct hwmon_device *hdev, enum hwmon_sensor sensor, unsigned int index, uint32_t att, const char **str);
extern state hwmon_register(struct hwmon_device *hdev);
extern void hwmon_unregister(struct hwmon_device *hdev);

#endif /* _DRIVER_HWMON_H_ */
