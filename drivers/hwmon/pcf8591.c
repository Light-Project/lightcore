/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pcf8591"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <ioops.h>
#include <kmalloc.h>
#include <timekeeping.h>
#include <driver/i2c.h>
#include <driver/hwmon.h>
#include <driver/hwmon/pcf8591.h>

struct pcf8591_device {
    struct hwmon_device hwmon;
    struct i2c_device *i2c;
    uint8_t mode;
    uint8_t ctrl;
};

#define hwmon_to_pcf8591(ptr) \
    container_of(ptr, struct pcf8591_device, hwmon)

static struct hwmon_channel_info pcf8591_info[] = {
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_IN,
        HWMON_IN_HAS_DATA, /* adc channel 0 / dac */
        HWMON_IN_HAS_DATA, /* adc channel 1 / dac */
        HWMON_IN_HAS_DATA, /* adc channel 2 / dac */
        HWMON_IN_HAS_DATA, /* adc channel 3 / dac */
    ),
    { }, /* NULL */
};

static state pcf8591_getdata(struct pcf8591_device *pcf8591,
                             unsigned int index, uint8_t *data)
{
    uint8_t value;
    state retval;

    value = PCF8591_MODE(pcf8591->mode) | PCF8591_CHANNEL(index);
    if (pcf8591->ctrl != value) {
        pcf8591->ctrl = value;

        retval = i2c_transfer_write_byte(pcf8591->i2c, value);
        if (retval)
            return retval;

        /*
         * The first byte transmitted contains the conversion code of
         * the previous read cycle. FLUSH IT!
         */
        retval = i2c_transfer_read_byte(pcf8591->i2c, &value);
        if (retval)
            return retval;
    }

    return i2c_transfer_read_byte(pcf8591->i2c, data);
}

static state pcf8591_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                          unsigned int index, uint32_t att, ssize_t *val)
{
    struct pcf8591_device *pcf8591 = hwmon_to_pcf8591(hdev);
    uint8_t value;
    state retval;

    switch (sensor) {
        case HWMON_SENSOR_IN:
            switch (att) {
                case HWMON_IN_DATA:
                    retval = pcf8591_getdata(pcf8591, index, &value);
                    if (retval)
                        return retval;
                    *val = value;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        default:
            return -EOPNOTSUPP;
    }

    return -ENOERR;
}

static state pcf8591_write(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                           unsigned int index, uint32_t att, ssize_t val)
{
    struct pcf8591_device *pcf8591 = hwmon_to_pcf8591(hdev);
    uint8_t buff[2];
    state retval;

    switch (sensor) {
        case HWMON_SENSOR_IN:
            switch (att) {
                case HWMON_IN_ENABLE:
                    if (val != PCF8591_MODE_TWDIFF && val != PCF8591_MODE_SIDIFF &&
                        val != PCF8591_MODE_TRDIFF && val != PCF8591_MODE_SINGLE)
                        return -EINVAL;
                    pcf8591->mode = val;
                    break;

                case HWMON_IN_DATA:
                    buff[0] = pcf8591->ctrl = PCF8591_OUTPUT_EN;
                    buff[1] = (uint8_t)val;
                    retval = i2c_transfer_buffer(pcf8591->i2c, buff, sizeof(buff), 0);
                    if (retval)
                        return retval;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        default:
            return -EOPNOTSUPP;
    }

    return -ENOERR;
}

static struct hwmon_ops pcf8591_ops = {
    .read = pcf8591_read,
    .write = pcf8591_write,
};

static state pcf8591_hwinit(struct pcf8591_device *pcf8591)
{
    uint8_t dummy;
    state retval;

    /* Initialize control register */
    retval = i2c_transfer_write_byte(pcf8591->i2c, pcf8591->ctrl);
    if (retval)
        return retval;

    return i2c_transfer_read_byte(pcf8591->i2c, &dummy);
}

static state pcf8591_probe(struct i2c_device *idev, const void *pdata)
{
    struct pcf8591_device *pcf8591;
    state retval;

    pcf8591 = i2c_kzalloc(idev, sizeof(*pcf8591), GFP_KERNEL);
    if (unlikely(!pcf8591))
        return -ENOMEM;

    pcf8591->i2c = idev;
    pcf8591->hwmon.dev = &idev->dev;
    pcf8591->hwmon.info = pcf8591_info;
    pcf8591->hwmon.ops = &pcf8591_ops;
    i2c_set_devdata(idev, pcf8591);

    retval = pcf8591_hwinit(pcf8591);
    if (retval) {
        i2c_err(idev, "device initialization failed\n");
        return retval;
    }

    return hwmon_register(&pcf8591->hwmon);
}

static void pcf8591_remove(struct i2c_device *idev)
{
    struct pcf8591_device *pcf8591 = i2c_get_devdata(idev);
    hwmon_unregister(&pcf8591->hwmon);
}

static const struct dt_device_id pcf8591_ids[] = {
    { .compatible = "nxp,pcf8591" },
    { }, /* NULL */
};

static struct i2c_driver pcf8591_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = pcf8591_ids,
    .probe = pcf8591_probe,
    .remove = pcf8591_remove,
};

static state pcf8591_init(void)
{
    return i2c_driver_register(&pcf8591_driver);
}
driver_initcall(pcf8591_init);
