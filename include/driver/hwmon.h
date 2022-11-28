/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_H_
#define _DRIVER_HWMON_H_

#include <device.h>

enum hwmon_sensor {
    HWMON_SENSOR_THERMAL        = 1,
    HWMON_SENSOR_HUMIDITY       = 2,
    HWMON_SENSOR_FAN            = 3,
    HWMON_SENSOR_PWM            = 4,
};

enum hwmon_thermal_attributes {
    HWMON_THERMAL_LABEL         = 0,
    HWMON_THERMAL_ENABLE        = 1,
    HWMON_THERMAL_DATA          = 2,
    HWMON_THERMAL_MIN           = 3,
    HWMON_THERMAL_MIN_HYST      = 4,
    HWMON_THERMAL_MAX           = 5,
    HWMON_THERMAL_MAX_HYST      = 6,
    HWMON_THERMAL_CRIT          = 7,
    HWMON_THERMAL_CRIT_HYST     = 8,
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
    HWMON_HUMIDITY_LABEL        = 0,
    HWMON_HUMIDITY_ENABLE       = 1,
    HWMON_HUMIDITY_DATA         = 2,
    HWMON_HUMIDITY_MIN          = 3,
    HWMON_HUMIDITY_MIN_HYST     = 4,
    HWMON_HUMIDITY_MAX          = 5,
    HWMON_HUMIDITY_MAX_HYST     = 6,
};

#define HWMON_HUMIDITY_HAS_ENABLE   BIT(HWMON_HUMIDITY_ENABLE)
#define HWMON_HUMIDITY_HAS_LABEL    BIT(HWMON_HUMIDITY_LABEL)
#define HWMON_HUMIDITY_HAS_DATA     BIT(HWMON_HUMIDITY_DATA)
#define HWMON_HUMIDITY_HAS_MIN      BIT(HWMON_HUMIDITY_MIN)
#define HWMON_HUMIDITY_HAS_MIN_HYST BIT(HWMON_HUMIDITY_MIN_HYST)
#define HWMON_HUMIDITY_HAS_MAX      BIT(HWMON_HUMIDITY_MAX)
#define HWMON_HUMIDITY_HAS_MAX_HYST BIT(HWMON_HUMIDITY_MAX_HYST)

enum hwmon_fan_attributes {
    HWMON_FAN_LABEL             = 0,
    HWMON_FAN_ENABLE            = 1,
    HWMON_FAN_DATA              = 2,
    HWMON_FAN_MIN               = 3,
    HWMON_FAN_MAX               = 4,
    HWMON_FAN_TARGET            = 5,
};

#define HWMON_FAN_HAS_ENABLE        BIT(HWMON_FAN_ENABLE)
#define HWMON_FAN_HAS_LABEL         BIT(HWMON_FAN_LABEL)
#define HWMON_FAN_HAS_DATA          BIT(HWMON_FAN_DATA)
#define HWMON_FAN_HAS_MIN           BIT(HWMON_FAN_MIN)
#define HWMON_FAN_HAS_MAX           BIT(HWMON_FAN_MAX)
#define HWMON_FAN_HAS_TARGET        BIT(HWMON_FAN_TARGET)

enum hwmon_pwm_attributes {
    HWMON_PWM_LABEL             = 0,
    HWMON_PWM_ENABLE            = 1,
    HWMON_PWM_DATA              = 2,
    HWMON_PWM_MODE              = 3,
    HWMON_PWM_FREQ              = 4,
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

#endif  /* _DRIVER_HWMON_H_ */
