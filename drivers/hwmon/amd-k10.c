/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "k10-thermal"

#include <initcall.h>
#include <driver/pci.h>
#include <driver/hwmon.h>
#include <driver/hwmon/amd-k10.h>

struct k10_hwmon_device {
    struct hwmon_device hwmon;
    const struct k10_hwmon_spec *spec;
};

struct k10_hwmon_spec {
    uint32_t (*temp)(struct pci_device *pdev);
    uint32_t (*crit)(struct pci_device *pdev);
    uint32_t ccd_offset;
};

enum k10_hwmon_sensor {
    K10_Tctl  = 0, K10_Tdie  = 1,
    K10_Tccd1 = 2, K10_Tccd2 = 3, K10_Tccd3 = 4, K10_Tccd4 = 5,
    K10_Tccd5 = 6, K10_Tccd6 = 7, K10_Tccd7 = 8, K10_Tccd8 = 9,
};

static const char *k10_hwmon_label[] = {
    [K10_Tctl]  = "Tctl",  [K10_Tdie]  = "Tdie",
    [K10_Tccd1] = "Tccd1", [K10_Tccd2] = "Tccd2", [K10_Tccd3] = "Tccd3", [K10_Tccd4] = "Tccd4",
    [K10_Tccd5] = "Tccd5", [K10_Tccd6] = "Tccd6", [K10_Tccd7] = "Tccd7", [K10_Tccd8] = "Tccd8",
};

static const struct hwmon_channel_info k10_hwmon_info[] = {
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_THERMAL,
        [K10_Tdie]  = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd1] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd2] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd3] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd4] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd5] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd6] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd7] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tccd8] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [K10_Tctl]  = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA |
                      HWMON_THERMAL_HAS_MAX   | HWMON_THERMAL_HAS_CRIT |
                      HWMON_THERMAL_HAS_CRIT_HYST,
    ),
    { }, /* NULL */
};

static uint32_t k10_pci_temp_raw(struct pci_device *pdev)
{
	return pci_config_readl(pdev, K10_HARDWARE_THERMAL_CONTROL);
}

static uint32_t k10_pci_crit_raw(struct pci_device *pdev)
{
	return pci_config_readl(pdev, K10_REPORTED_TEMPERATURE);
}

static state k10_hwmon_string(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                              unsigned int channel, uint32_t att, const char **str)
{
    if (att != HWMON_THERMAL_LABEL)
        return -EOPNOTSUPP;

    *str = k10_hwmon_label[channel];
    return -ENOERR;
}

static state k10_hwmon_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                            unsigned int channel, uint32_t att, ssize_t *val)
{

    switch (att) {
        case HWMON_THERMAL_DATA:

            break;
        case HWMON_THERMAL_MAX:
            *val = K10_TEMP_MAX;
            break;
        case HWMON_THERMAL_CRIT:

            break;
        case HWMON_THERMAL_CRIT_HYST:

            break;
        default:
            return -EOPNOTSUPP;
    }

    return -ENOERR;
}

static struct hwmon_ops k10_thermal_ops = {
    .string = k10_hwmon_string,
    .read = k10_hwmon_read,
};

static const struct k10_hwmon_spec k10_hwmon_default = {
    .temp = k10_pci_temp_raw,
    .crit = k10_pci_crit_raw,
};

static state k10_thermal_probe(struct pci_device *pdev, const void *pdata)
{
    struct k10_hwmon_device *kdev;

    kdev = dev_kmalloc(&pdev->dev, sizeof(*kdev), GFP_KERNEL);
    if (!kdev)
        return -ENOMEM;
    pci_set_devdata(pdev, kdev);

    kdev->spec = pdata ? pdata : &k10_hwmon_default;
    kdev->hwmon.dev = &pdev->dev;
    kdev->hwmon.ops = &k10_thermal_ops;
    kdev->hwmon.info = k10_hwmon_info;

    return hwmon_register(&kdev->hwmon);
}

static const struct pci_device_id k10_thermal_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_10H_NB_MISC) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_11H_NB_MISC) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_CNB17H_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_15H_NB_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_15H_M10H_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_15H_M30H_NB_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_15H_M60H_NB_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_15H_M70H_NB_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_16H_NB_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_16H_M30H_NB_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_17H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_17H_M10H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_17H_M30H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_17H_M60H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_17H_M70H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_19H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_19H_M40H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_19H_M50H_DF_F3) },
    { PCI_DEVICE(PCI_VENDOR_ID_HYGON, PCI_DEVICE_ID_AMD_17H_DF_F3) },
    { },
};

static struct pci_driver k10_thermal_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = k10_thermal_ids,
    .probe = k10_thermal_probe,
};

static state k10_thermal_init(void)
{
    return pci_driver_register(&k10_thermal_driver);
}
driver_initcall(k10_thermal_init);
