/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <bitops.h>
#include <driver/hwmon.h>

#define ATTRIBUTE_TEST(attribute) \
    [attribute] = __stringify(attribute)

static const char *thermal_attributes_names[] = {
    ATTRIBUTE_TEST(HWMON_THERMAL_LABEL),
    ATTRIBUTE_TEST(HWMON_THERMAL_ENABLE),
    ATTRIBUTE_TEST(HWMON_THERMAL_DATA),
    ATTRIBUTE_TEST(HWMON_THERMAL_MIN),
    ATTRIBUTE_TEST(HWMON_THERMAL_MIN_HYST),
    ATTRIBUTE_TEST(HWMON_THERMAL_MAX),
    ATTRIBUTE_TEST(HWMON_THERMAL_MAX_HYST),
    ATTRIBUTE_TEST(HWMON_THERMAL_CRIT),
    ATTRIBUTE_TEST(HWMON_THERMAL_CRIT_HYST),
};

static const char *humidity_attributes_names[] = {
    ATTRIBUTE_TEST(HWMON_HUMIDITY_LABEL),
    ATTRIBUTE_TEST(HWMON_HUMIDITY_ENABLE),
    ATTRIBUTE_TEST(HWMON_HUMIDITY_DATA),
    ATTRIBUTE_TEST(HWMON_HUMIDITY_MIN),
    ATTRIBUTE_TEST(HWMON_HUMIDITY_MIN_HYST),
    ATTRIBUTE_TEST(HWMON_HUMIDITY_MAX),
    ATTRIBUTE_TEST(HWMON_HUMIDITY_MAX_HYST),
};

static const char *fan_attributes_names[] = {
    ATTRIBUTE_TEST(HWMON_FAN_LABEL),
    ATTRIBUTE_TEST(HWMON_FAN_ENABLE),
    ATTRIBUTE_TEST(HWMON_FAN_DATA),
    ATTRIBUTE_TEST(HWMON_FAN_MIN),
    ATTRIBUTE_TEST(HWMON_FAN_MAX),
    ATTRIBUTE_TEST(HWMON_FAN_TARGET),
};

static const char *pwm_attributes_names[] = {
    ATTRIBUTE_TEST(HWMON_PWM_LABEL),
    ATTRIBUTE_TEST(HWMON_PWM_ENABLE),
    ATTRIBUTE_TEST(HWMON_PWM_DATA),
    ATTRIBUTE_TEST(HWMON_PWM_MODE),
    ATTRIBUTE_TEST(HWMON_PWM_FREQ),
};

static state hwmon_test_channels(struct kshell_context *ctx, struct hwmon_device *hdev, struct hwmon_channel_info *info)
{
    unsigned int channel, attmax;
    const char **attname;
    const uint32_t *atts;

    switch (info->sensor) {
        case HWMON_SENSOR_THERMAL:
            attmax = ARRAY_SIZE(thermal_attributes_names);
            attname = thermal_attributes_names;
            break;

        case HWMON_SENSOR_HUMIDITY:
            attmax = ARRAY_SIZE(humidity_attributes_names);
            attname = humidity_attributes_names;
            break;

        case HWMON_SENSOR_FAN:
            attmax = ARRAY_SIZE(fan_attributes_names);
            attname = fan_attributes_names;
            break;

        case HWMON_SENSOR_PWM:
            attmax = ARRAY_SIZE(pwm_attributes_names);
            attname = pwm_attributes_names;
            break;

        default:
            return -EINVAL;
    }

    for (channel = 0, atts = info->channels; *atts; ++channel, ++atts) {
        unsigned long attmask = *atts;
        unsigned int index;

        for (attmask = *atts; (index = ffs(attmask)); bit_clr(&attmask, index - 1)) {
            state retval;

            if (index > attmax)
                return -EOVERFLOW;

            if (index == 1) {
                const char *value;
                retval = hwmon_device_string(hdev, info->sensor, channel, index - 1, &value);
                if (retval)
                    kshell_printf(ctx, "hwmon %s '%s' failed: %d\n",
                                  hdev->dev->name, attname[index - 1], retval);
                else
                    kshell_printf(ctx, "hwmon %s '%s' passed: %s\n",
                                  hdev->dev->name, attname[index - 1], value);
            } else {
                ssize_t value;
                retval = hwmon_device_read(hdev, info->sensor, channel, index - 1, &value);
                if (retval)
                    kshell_printf(ctx, "hwmon %s '%s' failed: %d\n",
                                  hdev->dev->name, attname[index - 1], retval);
                else
                    kshell_printf(ctx, "hwmon %s '%s' passed: %ld\n",
                                  hdev->dev->name, attname[index - 1], value);
            }
        }
    }

    return -ENOERR;
}

static state hwmon_testing(struct kshell_context *ctx, void *pdata)
{
    struct hwmon_device *hdev;
    state retval = -ENOERR;

    spin_lock(&hwmon_lock);
    list_for_each_entry(hdev, &hwmon_list, list) {
        struct hwmon_channel_info *info;
        for (info = hdev->info; info->sensor; ++info) {
            retval = hwmon_test_channels(ctx, hdev, info);
            if (retval)
                goto finish;
        }
    }

finish:
    spin_unlock(&hwmon_lock);
    return retval;
}

static struct selftest_command hwmon_command = {
    .group = "driver",
    .name = "hwmon",
    .desc = "hwmon unit test",
    .testing = hwmon_testing,
};

static state selftest_hwmon_init(void)
{
    return selftest_register(&hwmon_command);
}
kshell_initcall(selftest_hwmon_init);
