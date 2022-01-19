/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "dell-smm"

#include <initcall.h>
#include <driver/firmware/dmi.h>
#include <driver/platform.h>
#include <driver/pci.h>
#include <driver/hwmon.h>
#include <driver/hwmon/dell-smm.h>

struct dell_smm_device {
    struct hwmon_device hwmon;
    const struct dell_smm_spec *spec;
};

#define hwmon_to_dell(hdev) \
    container_of(hdev, struct dell_smm_device, hwmon)

struct dell_smm_spec {
    unsigned int fan_mult;
    unsigned int fan_max;
};

static const char *dell_temp_labels[] = {
    "CPU", "GPU", "SODIMM", "Other",
    "Ambient", "Other",
};

static const char *dell_fan_labels[] = {
    "Processor Fan", "Motherboard Fan",
    "Video Fan", "Power Supply Fan",
    "Chipset Fan", "Other Fan",
};

static const char *dell_docking_labels[] = {
    "Docking Processor Fan", "Docking Motherboard Fan",
    "Docking Video Fan", "Docking Power Supply Fan",
    "Docking Chipset Fan", "Docking Other Fan",
};

static const struct hwmon_channel_info dell_hwmon_info[] = {
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
        HWMON_FAN_HAS_LABEL | HWMON_FAN_HAS_DATA |
        HWMON_FAN_HAS_MAX   | HWMON_FAN_HAS_MIN  | HWMON_FAN_HAS_TARGET,
        HWMON_FAN_HAS_LABEL | HWMON_FAN_HAS_DATA |
        HWMON_FAN_HAS_MAX   | HWMON_FAN_HAS_MIN  | HWMON_FAN_HAS_TARGET,
        HWMON_FAN_HAS_LABEL | HWMON_FAN_HAS_DATA |
        HWMON_FAN_HAS_MAX   | HWMON_FAN_HAS_MIN  | HWMON_FAN_HAS_TARGET,
    ),
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_PWM,
        HWMON_PWM_HAS_DATA | HWMON_PWM_HAS_DATA,
        HWMON_PWM_HAS_DATA,
        HWMON_PWM_HAS_DATA,
    ),
    { }, /* NULL */
};

struct dell_smm_regs {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
} __packed;

static state dell_smm_trigger(struct dell_smm_regs *regs)
{
    uint32_t eax = regs->eax;
    int retval;

#ifdef CONFIG_ARCH_X86_64
    asm volatile (
        "pushq  %%rax               \n"
        "movl   0(%%rax), %%edx     \n"
        "pushq  %%rdx               \n"
        "movl   4(%%rax), %%ebx     \n"
        "movl   8(%%rax), %%ecx     \n"
        "movl   12(%%rax), %%edx    \n"
        "movl   16(%%rax), %%esi    \n"
        "movl   20(%%rax), %%edi    \n"
        "popq   %%rax               \n"
        "out    %%al, $0xb2         \n"
        "out    %%al, $0x84         \n"
        "xchgq  %%rax, (%%rsp)      \n"
        "movl   %%ebx, 4(%%rax)     \n"
        "movl   %%ecx, 8(%%rax)     \n"
        "movl   %%edx, 12(%%rax)    \n"
        "movl   %%esi, 16(%%rax)    \n"
        "movl   %%edi, 20(%%rax)    \n"
        "popq   %%rdx               \n"
        "movl   %%edx, 0(%%rax)     \n"
        "pushfq                     \n"
        "popq   %%rax               \n"
        "andl   $1, %%eax           \n"
        : "=a"(retval)
        : "a"(regs)
        : "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory"
    );
#else
    asm volatile(
        "pushl  %%eax               \n"
        "movl   0(%%eax), %%edx     \n"
        "push   %%edx               \n"
        "movl   4(%%eax), %%ebx     \n"
        "movl   8(%%eax), %%ecx     \n"
        "movl   12(%%eax), %%edx    \n"
        "movl   16(%%eax), %%esi    \n"
        "movl   20(%%eax), %%edi    \n"
        "popl   %%eax               \n"
        "out    %%al, $0xb2         \n"
        "out    %%al, $0x84         \n"
        "xchgl  %%eax, (%%esp)      \n"
        "movl   %%ebx, 4(%%eax)     \n"
        "movl   %%ecx, 8(%%eax)     \n"
        "movl   %%edx, 12(%%eax)    \n"
        "movl   %%esi, 16(%%eax)    \n"
        "movl   %%edi, 20(%%eax)    \n"
        "popl   %%edx               \n"
        "movl   %%edx, 0(%%eax)     \n"
        "lahf                       \n"
        "shrl   $8, %%eax           \n"
        "andl   $1, %%eax           \n"
        : "=a"(retval)
        : "a"(regs)
        : "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory"
    );
#endif

    if (!retval || regs->eax == eax || (regs->eax & 0xffff) == 0xffff)
        retval = -EINVAL;

    return retval;
}

static int dell_temp_get(int sensor)
{
    struct dell_smm_regs regs = {
        .eax = DELL_SMM_GET_TEMP_TYPE,
        .ebx = sensor & 0xff,
    };

    return dell_smm_trigger(&regs) ? 0 : regs.eax & 0xff;
}

static int dell_fan_speed_get(int sensor)
{
    struct dell_smm_regs regs = {
        .eax = DELL_SMM_GET_SPEED,
        .ebx = sensor & 0xff,
    };

    return dell_smm_trigger(&regs) ? 0 : (regs.eax & 0xffff) * DELL_FAN_MULT;
}

static int dell_fan_speed_get(int sensor, int speed)
{
    struct dell_smm_regs regs = {
        .eax = DELL_SMM_GET_SPEED,
        .ebx = sensor & 0xff,
    };

    regs.ebx = ;
    return dell_smm_trigger(&regs) ? 0 : (regs.eax & 0xffff) * DELL_FAN_MULT;
}

static int dell_fan_auto_mode(int sensor, bool enable)
{
	struct smm_regs regs = { };

	if (data->disallow_fan_support)
		return -EINVAL;

	regs.eax = enable ? data->auto_fan : data->manual_fan;
	return i8k_smm(&regs);
}

static state dell_smm_string(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                             unsigned int channel, uint32_t att, const char **str)
{

    return -ENOERR;
}

static state dell_smm_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                           unsigned int channel, uint32_t att, ssize_t *val)
{
    return -ENOERR;
}

static state dell_smm_write(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                            unsigned int channel, uint32_t att, ssize_t val)
{

    if (sensor != HWMON_SENSOR_PWM)
        return -EOPNOTSUPP;

    switch (att) {
        case HWMON_PWM_ENABLE:

            break;
        case HWMON_PWM_DATA:

            break;
        default:
            return -EOPNOTSUPP;
    }

    return -ENOERR;
}

static const struct hwmon_ops dell_smm_ops = {
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

static const struct dmi_device_id dell_smm_ids[] = {
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
        .data = &dell_latitude_d520;
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
        .data = &dell_precision_490;
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
        .data = &dell_studio;
    },
    {   .name = "Dell XPS M140",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "MXC051"),
        },
        .data = &dell_xps;
    },
    {   .name = "Dell XPS",
        .matches = {
            DMI_MATCH(DMI_REG_SYS_VENDOR, "Dell Inc."),
            DMI_MATCH(DMI_REG_PRODUCT_NAME, "XPS"),
        },
    },
    { }, /* NULL */
};

static state dell_smm_probe(struct platform_device *pdev, const void *pdata)
{
    struct dell_smm_device *ddev;

    ddev = dev_kmalloc(&pdev->dev, sizeof(*ddev), GFP_KERNEL);
    if (!ddev)
        return -ENOMEM;
    pci_set_devdata(pdev, ddev);

    ddev->hwmon.dev = &pdev->dev;
    ddev->hwmon.ops = &dell_smm_ops;
    ddev->hwmon.info = dell_hwmon_info;

    return hwmon_register(&ddev->hwmon);
}

static struct platform_driver dell_smm_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = dell_smm_probe,
};

static state dell_smm_init(void)
{
    void *data;

    if (!(data = dmi_system_check(dell_smm_ids)))
        return -ENODEV;

    return -ENOERR;
}
driver_initcall(dell_smm_init);
