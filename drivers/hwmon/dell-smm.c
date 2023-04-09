/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "dell-smm"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <delay.h>
#include <asm/smi.h>
#include <driver/isa.h>
#include <driver/firmware/dmi.h>
#include <driver/hwmon.h>
#include <driver/hwmon/dell-smm.h>

struct dell_smm_spec {
    unsigned int fan_mult;
    unsigned int fan_max;
};

struct dell_smm_device {
    struct hwmon_device hwmon;
    struct mutex lock;

    unsigned int fan_mult;
    unsigned int fan_max;
    unsigned int pwm_mult;

    bool forbid_fan_support;
    bool forbid_fan_type;

    uint32_t fan_auto_value;
    uint32_t fan_manual_value;
};

#define hwmon_to_dell(hdev) \
    container_of(hdev, struct dell_smm_device, hwmon)

enum dell_temp_type {
    DELL_TEMP_CPU           = 0,
    DELL_TEMP_GPU           = 1,
    DELL_TEMP_SODIMM        = 2,
    DELL_TEMP_OTHER1        = 3,
    DELL_TEMP_AMBIENT       = 4,
    DELL_TEMP_OTHER2        = 5,
};

enum dell_fan_type {
    DELL_FAN_PROCESSOR      = 0,
    DELL_FAN_MOTHERBOARD    = 1,
    DELL_FAN_VIDEO          = 2,
    DELL_FAN_POWER          = 3,
    DELL_FAN_CHIPSET        = 4,
    DELL_FAN_OTHER          = 5,
};

static const char *dell_temp_labels[] = {
    [DELL_TEMP_CPU]         = "CPU",
    [DELL_TEMP_GPU]         = "GPU",
    [DELL_TEMP_SODIMM]      = "SODIMM",
    [DELL_TEMP_OTHER1]      = "Other1",
    [DELL_TEMP_AMBIENT]     = "Ambient",
    [DELL_TEMP_OTHER2]      = "Other2",
};

static const char *dell_fan_labels[] = {
    [DELL_FAN_PROCESSOR]    = "Processor Fan",
    [DELL_FAN_MOTHERBOARD]  = "Motherboard Fan",
    [DELL_FAN_VIDEO]        = "Video Fan",
    [DELL_FAN_POWER]        = "Power Supply Fan",
    [DELL_FAN_CHIPSET]      = "Chipset Fan",
    [DELL_FAN_OTHER]        = "Other Fan",
};

static const char *dell_docking_labels[] = {
    [DELL_FAN_PROCESSOR]    = "Docking Processor Fan",
    [DELL_FAN_MOTHERBOARD]  = "Docking Motherboard Fan",
    [DELL_FAN_VIDEO]        = "Docking Video Fan",
    [DELL_FAN_POWER]        = "Docking Power Supply Fan",
    [DELL_FAN_CHIPSET]      = "Docking Chipset Fan",
    [DELL_FAN_OTHER]        = "Docking Other Fan",
};

static struct hwmon_channel_info dell_hwmon_info[] = {
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_THERMAL,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
    ),
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_FAN,
        HWMON_FAN_HAS_LABEL | HWMON_FAN_HAS_ENABLE | HWMON_FAN_HAS_DATA |
        HWMON_FAN_HAS_MAX   | HWMON_FAN_HAS_MIN    | HWMON_FAN_HAS_TARGET,
        HWMON_FAN_HAS_LABEL | HWMON_FAN_HAS_ENABLE | HWMON_FAN_HAS_DATA |
        HWMON_FAN_HAS_MAX   | HWMON_FAN_HAS_MIN    | HWMON_FAN_HAS_TARGET,
        HWMON_FAN_HAS_LABEL | HWMON_FAN_HAS_ENABLE | HWMON_FAN_HAS_DATA |
        HWMON_FAN_HAS_MAX   | HWMON_FAN_HAS_MIN    | HWMON_FAN_HAS_TARGET,
    ),
    { }, /* NULL */
};

static state dell_smm_trigger(struct smi_regs *regs)
{
    uint32_t eax = regs->eax;

    smi_trigger(regs);
    if (regs->eax == eax)
        return -EINVAL;

    if ((regs->eax & 0xffff) == 0xffff)
        return -ENODEV;

    return -ENOERR;
}

static int dell_fan_auto_mode(struct dell_smm_device *dell, uint8_t sensor, bool enable)
{
    struct smi_regs regs = {};

    if (dell->forbid_fan_support)
        return -EINVAL;

    if (enable)
        regs.eax = dell->fan_auto_value;
    else
        regs.eax = dell->fan_manual_value;

    return dell_smm_trigger(&regs);
}

static int dell_fan_status_get(struct dell_smm_device *dell, uint8_t sensor)
{
    struct smi_regs regs = {
        .eax = DELL_SMM_FAN_STATUS_GET,
        .ebx = sensor,
    };

    if (dell->forbid_fan_support)
        return -EINVAL;

    return dell_smm_trigger(&regs) ?: (regs.eax & 0xff);
}

static int dell_fan_status_set(struct dell_smm_device *dell, uint8_t sensor, int speed)
{
    struct smi_regs regs = {
        .eax = DELL_SMM_FAN_STATUS_SET,
        .ebx = sensor,
    };

    if (dell->forbid_fan_support)
        return -EINVAL;

    clamp_adj(speed, 0, dell->fan_max);
    regs.ebx |= speed << 8;

    return dell_smm_trigger(&regs) ?: regs.eax & 0xff;
}

static int dell_fan_speed(struct dell_smm_device *dell, uint8_t sensor)
{
    struct smi_regs regs = {
        .eax = DELL_SMM_FAN_SPEED,
        .ebx = sensor,
    };

    if (dell->forbid_fan_support)
        return -EINVAL;

    return dell_smm_trigger(&regs) ?: (regs.eax & 0xffff) * dell->fan_mult;
}

static int dell_fan_type(struct dell_smm_device *dell, uint8_t sensor)
{
    struct smi_regs regs = {
        .eax = DELL_SMM_FAN_TYPE,
        .ebx = sensor,
    };

    if (dell->forbid_fan_support || dell->forbid_fan_type)
        return -EINVAL;

    return dell_smm_trigger(&regs) ?: regs.eax & 0xff;
}

static int dell_nominal_speed(struct dell_smm_device *dell, uint8_t sensor, int speed)
{
    struct smi_regs regs = {
        .eax = DELL_SMM_NOM_SPEED,
        .ebx = sensor | (speed << 8),
    };

    if (dell->forbid_fan_support)
        return -EINVAL;

    return dell_smm_trigger(&regs) ?: regs.eax & 0xffff;
}

static int dell_temp_raw(struct dell_smm_device *dell, uint8_t sensor)
{
    struct smi_regs regs = {
        .eax = DELL_SMM_TEMP_GET,
        .ebx = sensor,
    };

    return dell_smm_trigger(&regs) ?: regs.eax & 0xff;
}

static int dell_temp_get(struct dell_smm_device *dell, uint8_t sensor)
{
    int temp;

    temp = dell_temp_raw(dell, sensor);
    if (temp == 0x99) {
        msleep(100);
        temp = dell_temp_raw(dell, sensor);
    }

    if (temp > DELL_TEMP_MAX)
        return -ENODATA;

    return temp;
}

static int dell_temp_type(struct dell_smm_device *dell, uint8_t sensor)
{
    struct smi_regs regs = {
        .eax = DELL_SMM_TEMP_TYPE,
        .ebx = sensor,
    };

    return dell_smm_trigger(&regs) ?: regs.eax & 0xff;
}

static state dell_smm_string(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                             unsigned int index, uint32_t att, const char **str)
{
    struct dell_smm_device *dell = hwmon_to_dell(hdev);
    bool docking = false;
    state retval;

    switch (sensor) {
        case HWMON_SENSOR_THERMAL:
            switch (att) {
                case HWMON_THERMAL_LABEL:
                    retval = dell_temp_type(dell, index);
                    if (retval < 0)
                        break;

                    min_adj(retval, ARRAY_SIZE(dell_temp_labels) - 1);
                    *str = dell_temp_labels[retval];
                    retval = -ENOERR;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        case HWMON_SENSOR_FAN:
            switch (att) {
                case HWMON_FAN_LABEL:
                    retval = dell_fan_type(dell, index);
                    if (retval < 0)
                        break;

                    if (retval & 0x10) {
                        docking = true;
                        retval &= 0x0f;
                    }

                    min_adj(retval, ARRAY_SIZE(dell_temp_labels) - 1);
                    *str = (docking ? dell_docking_labels : dell_fan_labels)[retval];
                    retval = -ENOERR;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        default:
            return -EOPNOTSUPP;
    }

    return retval;
}

static state dell_smm_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                           unsigned int index, uint32_t att, ssize_t *val)
{
    struct dell_smm_device *dell = hwmon_to_dell(hdev);
    state retval;

    switch (sensor) {
        case HWMON_SENSOR_THERMAL:
            switch (att) {
                case HWMON_THERMAL_DATA:
                    retval = dell_temp_get(dell, index);
                    if (retval < 0)
                        break;

                    *val = retval * 1000;
                    retval = -ENOERR;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        case HWMON_SENSOR_FAN:
            switch (att) {
                case HWMON_FAN_ENABLE:
                    retval = dell_fan_status_get(dell, index);
                    if (retval < 0)
                        break;

                    *val = retval;
                    retval = -ENOERR;
                    break;

                case HWMON_FAN_DATA:
                    retval = dell_fan_speed(dell, index);
                    if (retval < 0)
                        break;
                    *val = retval;
                    retval = -ENOERR;
                    break;

                case HWMON_FAN_MIN:
                    retval = dell_nominal_speed(dell, index, 0);
                    if (retval < 0)
                        break;

                    *val = retval;
                    retval = -ENOERR;
                    break;

                case HWMON_FAN_MAX:
                    retval = dell_nominal_speed(dell, index, dell->fan_max);
                    if (retval < 0)
                        break;

                    *val = retval;
                    retval = -ENOERR;
                    break;

                case HWMON_FAN_TARGET:
                    retval = dell_fan_status_get(dell, index);
                    if (retval < 0)
                        break;

                    min_adj(retval, dell->fan_max);
                    retval = dell_nominal_speed(dell, index, retval);
                    if (retval < 0)
                        break;

                    *val = retval;
                    retval = -ENOERR;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        default:
            return -EOPNOTSUPP;
    }

    return retval;
}

static state dell_smm_write(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                            unsigned int index, uint32_t att, ssize_t val)
{
    struct dell_smm_device *dell = hwmon_to_dell(hdev);
    state retval;

    switch (sensor) {
        case HWMON_SENSOR_FAN:
            switch (att) {
                case HWMON_FAN_ENABLE:
                    mutex_lock(&dell->lock);
                    retval = dell_fan_status_set(dell, index, val);
                    if (retval < 0)
                        break;
                    retval = -ENOERR;
                    mutex_unlock(&dell->lock);
                    break;

                case HWMON_FAN_DATA:
                    mutex_lock(&dell->lock);
                    retval = dell_fan_auto_mode(dell, index, val);
                    if (retval < 0)
                        break;
                    retval = -ENOERR;
                    mutex_unlock(&dell->lock);
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        default:
            return -EOPNOTSUPP;
    }

    return retval;
}

static state dell_check_signature(uint32_t request)
{
    state retval;
    struct smi_regs regs = {
        .eax = request,
    };

    retval = dell_smm_trigger(&regs);
    if (retval)
        return retval;

    if (regs.eax != 0x44494147 || regs.edx != 0x44454c4c)
        return -ENODEV;

    return -ENOERR;
}

static struct hwmon_ops dell_smm_ops = {
    .string = dell_smm_string,
    .read = dell_smm_read,
    .write = dell_smm_write,
};

static const struct dell_smm_spec dell_latitude_d520 = {
    .fan_mult = 1,
    .fan_max = DELL_FAN_TURBO,
};

static const struct dell_smm_spec dell_precision_490 = {
    .fan_mult = 1,
    .fan_max = DELL_FAN_TURBO,
};

static const struct dell_smm_spec dell_studio = {
    .fan_mult = 1,
    .fan_max = DELL_FAN_HIGH,
};

static const struct dell_smm_spec dell_xps = {
    .fan_mult = 1,
    .fan_max = DELL_FAN_HIGH,
};

static struct dmi_device_id dell_smm_ids[] = {
    {   .name = "Dell Inspiron",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Computer"),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Inspiron"),
        },
    },
    {   .name = "Dell Latitude",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Computer"),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Latitude"),
        },
    },
    {   .name = "Dell Inspiron 2",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Inspiron"),
        },
    },
    {   .name = "Dell Latitude D520",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Latitude D520"),
        },
        .data = &dell_latitude_d520,
    },
    {   .name = "Dell Latitude 2",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Latitude"),
        },
    },
    {   .name = "Dell Inspiron 3",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "MM061"),
        },
    },
    {   .name = "Dell Inspiron 3",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "MP061"),
        },
    },
    {   .name = "Dell Precision 490",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME,
                  "Precision WorkStation 490"),
        },
        .data = &dell_precision_490,
    },
    {   .name = "Dell Precision",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Precision"),
        },
    },
    {   .name = "Dell Vostro",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Vostro"),
        },
    },
    {   .name = "Dell Studio",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "Studio"),
        },
        .data = &dell_studio,
    },
    {   .name = "Dell XPS M140",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "MXC051"),
        },
        .data = &dell_xps,
    },
    {   .name = "Dell XPS",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "XPS"),
        },
    },
    { }, /* NULL */
};

static state dell_smm_probe(struct isa_device *idev, unsigned int index, const void *pdata)
{
    struct dell_smm_device *dell;
    const struct dell_smm_spec *spec;

    dell = isa_kzalloc(idev, sizeof(*dell), GFP_KERNEL);
    if (unlikely(!dell))
        return -ENOMEM;

    if ((spec = dmi_system_check(dell_smm_ids))) {
        dell->fan_mult = spec->fan_mult;
        dell->fan_max = spec->fan_max;
    }

    if (!dell->fan_mult)
        dell->fan_mult = DELL_FAN_MULT;
    if (!dell->fan_max)
        dell->fan_max = DELL_FAN_HIGH;

    dell->hwmon.dev = &idev->dev;
    dell->hwmon.ops = &dell_smm_ops;
    dell->hwmon.info = dell_hwmon_info;
    isa_set_devdata(idev, dell);

    return hwmon_register(&dell->hwmon);
}

static state dell_smm_match(struct isa_device *idev, unsigned int index)
{
    state retval;

    if ((retval = dell_check_signature(DELL_SMM_SIG1_GET)) &&
        (retval = dell_check_signature(DELL_SMM_SIG2_GET))) {
		isa_err(idev, "unable to get smm signature\n");
        return retval;
    }

    return -ENOERR;
}

static struct isa_driver dell_smm_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .match = dell_smm_match,
    .probe = dell_smm_probe,
};

static state dell_smm_init(void)
{
    return isa_driver_register(&dell_smm_driver, 1, NULL);
}
driver_initcall(dell_smm_init);
