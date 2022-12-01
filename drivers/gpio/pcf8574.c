/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pcf8574"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/i2c.h>
#include <driver/gpio.h>

struct pcf8574_device {
    struct gpio_device gpio;
    struct i2c_device *idev;
    uint8_t ports;
};

#define gpio_to_pcf8574(ptr) \
    container_of(ptr, struct pcf8574_device, gpio)

static state pcf8574_value_get(struct gpio_device *gdev, unsigned int port, bool *value)
{
    struct pcf8574_device *pcf8574 = gpio_to_pcf8574(gdev);
    uint8_t buffer;
    state retval;

    /**
     * If it has been pulled down,
     * we don't need to read it out.
     */
    if (!(pcf8574->ports & BIT(port))) {
        *value = false;
        return -ENOERR;
    }

    retval = i2c_transfer_read_byte(pcf8574->idev, &buffer);
    if (retval)
        return retval;

    *value = (buffer >> port) & 0x01;
    return -ENOERR;
}

static state pcf8574_value_set(struct gpio_device *gdev, unsigned int port, bool value)
{
    struct pcf8574_device *pcf8574 = gpio_to_pcf8574(gdev);

    /**
     * If the pin is the same as before,
     * we don't need to change it.
     */
    if (!!(pcf8574->ports & BIT(port)) == value)
        return -ENOERR;

    pcf8574->ports &= ~BIT(port);
    pcf8574->ports |= BIT_SHIFT(port, value);
    return i2c_transfer_write_byte(pcf8574->idev, pcf8574->ports);
}

static state pcf8574_multiple_get(struct gpio_device *gdev, unsigned long *mask,
                                  unsigned long *value)
{
    struct pcf8574_device *pcf8574 = gpio_to_pcf8574(gdev);
    uint8_t buffer;
    state retval;

    /**
     * If it has been all pulled down,
     * we don't need to read it out.
     */
    if (!(*mask & pcf8574->ports)) {
        *value = 0;
        return -ENOERR;
    }

    retval = i2c_transfer_read_byte(pcf8574->idev, &buffer);
    if (retval)
        return retval;

    *value = *mask & buffer;
    return -ENOERR;
}

static state pcf8574_multiple_set(struct gpio_device *gdev, unsigned long *mask,
                                  unsigned long *value)
{
    struct pcf8574_device *pcf8574 = gpio_to_pcf8574(gdev);

    /**
     * If the pin is the same as before,
     * we don't need to change it.
     */
    if ((pcf8574->ports & *mask) == *value)
        return -ENOERR;

    pcf8574->ports &= ~*mask;
    pcf8574->ports |= *mask & *value;
    return i2c_transfer_write_byte(pcf8574->idev, pcf8574->ports);
}

static state pcf8574_dt_xlate(struct gpio_device *gdev, const struct dt_phandle_args *args,
                              uint32_t *index, uint32_t *flags)
{
    uint32_t pinnum;

    pinnum = args->args[0];
    if (pinnum >gdev->gpio_num)
        return -EINVAL;

    if (index)
        *index = pinnum;

    if (flags)
        *flags = args->args[1];

    return -ENOERR;
}

static struct gpio_ops pcf8574_ops = {
    .value_get = pcf8574_value_get,
    .value_set = pcf8574_value_set,
    .multiple_get = pcf8574_multiple_get,
    .multiple_set = pcf8574_multiple_set,
    .dt_xlate = pcf8574_dt_xlate,
    .dt_gpio_cells = 2,
};

static state pcf8574_probe(struct i2c_device *idev, const void *pdata)
{
    struct pcf8574_device *pcf8574;
    state retval;

    retval = i2c_transfer_write_byte(idev, 0xff);
    if (retval)
        return -ENODEV;

    pcf8574 = i2c_kzalloc(idev, sizeof(*pcf8574), GFP_KERNEL);
    if (unlikely(!pcf8574))
        return -ENOMEM;

    pcf8574->idev = idev;
    pcf8574->ports = 0xff;

    pcf8574->gpio.dev = &idev->dev;
    pcf8574->gpio.dt_node = idev->dt_node;
    pcf8574->gpio.ops = &pcf8574_ops;
    pcf8574->gpio.gpio_base = UINT_MAX;
    pcf8574->gpio.gpio_num = BITS_PER_U8;
    i2c_set_devdata(idev, pcf8574);

    return gpio_register(&pcf8574->gpio);
}

static struct dt_device_id pcf8574_ids[] = {
    { .compatible = "nxp,pcf8574" },
    { }, /* NULL */
};

static struct i2c_driver pcf8574_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = pcf8574_ids,
    .probe = pcf8574_probe,
};

static state pcf8574_init(void)
{
    return i2c_driver_register(&pcf8574_driver);
}
driver_initcall(pcf8574_init);
