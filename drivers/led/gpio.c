/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gpio-led"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/led.h>
#include <driver/gpio.h>

struct gpio_led_device {
    struct led_device ldev;
    struct gpio_channel *gpio;
};

struct gpio_led_array {
    unsigned int led_num;
    struct gpio_led_device leds[0];
};

#define led_to_gpio_led(ptr) \
    container_of(ptr, struct gpio_led_device, ldev)

static state gpio_led_brightness_get(struct led_device *ldev, enum led_brightness *bn)
{
    struct gpio_led_device *gdev = led_to_gpio_led(ldev);
    bool value;
    state retval;

    retval = gpioc_value_get(gdev->gpio, &value);
    if (retval)
        return retval;

    if (value)
        *bn = LED_BRIGHTNESS_ON;
    else
        *bn = LED_BRIGHTNESS_OFF;

    return -ENOERR;
}

static state gpio_led_brightness_set(struct led_device *ldev, enum led_brightness bn)
{
    struct gpio_led_device *gdev = led_to_gpio_led(ldev);
    bool value;

    if (bn == LED_BRIGHTNESS_OFF)
        value = false;
    else
        value = true;

    return gpioc_value_set(gdev->gpio, value);
}

static struct led_ops gpio_led_ops = {
    .brightness_get = gpio_led_brightness_get,
    .brightness_set = gpio_led_brightness_set,
};

static state gpio_led_probe(struct platform_device *pdev, const void *pdata)
{
    struct gpio_led_array *garray;
    struct fwnode *fwnode;
    unsigned int count;

    count = device_child_count(&pdev->dev);
    if (!count)
        return -ENODEV;

    garray = platform_kzalloc(pdev, sizeof(*garray) + sizeof(*garray->leds) * count, GFP_KERNEL);
    if (unlikely(!garray))
        return -ENOMEM;

    device_for_each_child(fwnode, &pdev->dev) {
        struct gpio_led_device *gdev;

        gdev = &garray->leds[garray->led_num];
        gdev->gpio = gpio_fwnode_channel(fwnode, NULL, 0);
        if (IS_INVAL(gdev->gpio))
            continue;

        gdev->ldev.dev = &pdev->dev;
        gdev->ldev.ops = &gpio_led_ops;
        gdev->ldev.fwnode = fwnode;
        gdev->ldev.max_brightness = LED_BRIGHTNESS_ON;
        led_register(&gdev->ldev);

        garray->led_num++;
    }

    return -ENOERR;
}

static const struct dt_device_id gpio_led_ids[] = {
    { .compatible = "gpio-leds" },
    { }, /* NULL */
};

static struct platform_driver gpio_led_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gpio_led_ids,
    .probe = gpio_led_probe,
};

static state gpio_led_init(void)
{
    return platform_driver_register(&gpio_led_driver);
}
driver_initcall_sync(gpio_led_init);
