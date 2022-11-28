/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i2c-gpio"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <ticktime.h>
#include <driver/platform.h>
#include <driver/i2c-bitbang.h>
#include <driver/gpio.h>

struct i2c_gpio_device {
    struct i2c_host host;
    struct i2c_bitbang bitbang;
    struct gpio_channel *sda;
    struct gpio_channel *scl;
};

static bool i2c_gpio_getsda(void *data)
{
    struct i2c_gpio_device *gdev = data;
    state retval;
    bool value;

    retval = gpioc_value_get(gdev->sda, &value);
    if (unlikely(retval)) {
        dev_err(gdev->host.dev, "getsda error %d\n", retval);
        value = false;
    }

    return value;
}

static bool i2c_gpio_getscl(void *data)
{
    struct i2c_gpio_device *gdev = data;
    state retval;
    bool value;

    retval = gpioc_value_get(gdev->scl, &value);
    if (unlikely(retval)) {
        dev_err(gdev->host.dev, "getscl error %d\n", retval);
        value = false;
    }

    return value;
}

static void i2c_gpio_setsda(void *data, bool value)
{
    struct i2c_gpio_device *gdev = data;
    state retval;

    retval = gpioc_value_set(gdev->sda, value);
    if (unlikely(retval))
        dev_err(gdev->host.dev, "setsda error %d\n", retval);
}

static void i2c_gpio_setscl(void *data, bool value)
{
    struct i2c_gpio_device *gdev = data;
    state retval;

    retval = gpioc_value_set(gdev->scl, value);
    if (unlikely(retval))
        dev_err(gdev->host.dev, "setscl error %d\n", retval);
}

static struct gpio_channel *
i2c_gpio_get_desc(struct device *dev, const char *label,
                  unsigned int index, enum gpio_request_flags flags)
{
    struct gpio_channel *channel;

    channel = gpio_fwnode_channel(dev->fwnode, label, flags);
    if (!IS_INVAL(channel))
        return channel;

    channel = gpio_fwnode_channel_index(dev->fwnode, NULL, index, flags);
    if (!IS_INVAL(channel))
        return channel;

    dev_err(dev, "error trying to get descriptor: %d\n", PTR_INVAL(channel));
    return channel;
}

static state i2c_gpio_probe(struct platform_device *pdev, const void *pdata)
{
    struct i2c_gpio_device *gdev;
    uint32_t delay = 0, timeout = 0;
    bool sda_drain, scl_drain, scl_noget;
    enum gpio_request_flags flags;

    gdev = platform_kzalloc(pdev, sizeof(*gdev), GFP_KERNEL);
    if (unlikely(!gdev))
        return -ENOMEM;

    device_attribute_read_u32(&pdev->dev, "i2c-gpio,delay-us", &delay);
    device_attribute_read_u32(&pdev->dev, "i2c-gpio,timeout-ms", &timeout);
    sda_drain = device_attribute_present(&pdev->dev, "i2c-gpio,sda-open-drain");
    scl_drain = device_attribute_present(&pdev->dev, "i2c-gpio,scl-open-drain");
    scl_noget = device_attribute_present(&pdev->dev, "i2c-gpio,scl-output-only");

    flags = sda_drain ? GPIOR_OUT_OPEN_DRAIN_HIGH : GPIOR_OUT_HIGH;
    gdev->sda = i2c_gpio_get_desc(&pdev->dev, "sda", 0, flags);
    if (IS_INVAL(gdev->sda))
        return PTR_INVAL(gdev->sda);

    flags = scl_drain ? GPIOR_OUT_OPEN_DRAIN_HIGH : GPIOR_OUT_HIGH;
    gdev->scl = i2c_gpio_get_desc(&pdev->dev, "scl", 1, flags);
    if (IS_INVAL(gdev->scl))
        return PTR_INVAL(gdev->scl);

    if (!scl_noget)
        gdev->bitbang.getscl = i2c_gpio_getscl;

    if (delay)
        gdev->bitbang.delay = delay;
    else if (!scl_noget)
        gdev->bitbang.delay = 50;
    else
        gdev->bitbang.delay = 5;

    if (timeout)
        gdev->bitbang.timeout = ms_to_ttime(timeout);
    else
        gdev->bitbang.timeout = SYSTICK_FREQ / 10;

    gdev->bitbang.getsda = i2c_gpio_getsda;
    gdev->bitbang.setsda = i2c_gpio_setsda;
    gdev->bitbang.setscl = i2c_gpio_setscl;

    gdev->bitbang.data = gdev;
    gdev->host.dev = &pdev->dev;
    gdev->host.host_data = &gdev->bitbang;

    return i2c_bitbang_register(&gdev->host);
}

static const struct dt_device_id i2c_gpio_ids[] = {
    { .compatible = "i2c-gpio" },
    { }, /* NULL */
};

static struct platform_driver i2c_gpio_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i2c_gpio_ids,
    .probe = i2c_gpio_probe,
};

static state i2c_gpio_init(void)
{
    return platform_driver_register(&i2c_gpio_driver);
}
driver_initcall_sync(i2c_gpio_init);
