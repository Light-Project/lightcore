/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <delay.h>
#include <driver/gpio.h>

static state buzzer_testing(struct kshell_context *ctx, void *pdata)
{
    struct gpio_device *gdev;
    unsigned int index;
    bool value;
    state retval = -ENOERR;

    spin_lock(&gpio_lock);
    list_for_each_entry(gdev, &gpio_list, list) {
        for (index = 0; index < gdev->gpio_num; ++index) {
            retval = gpio_raw_value_get(gdev, index, &value);
            kshell_printf(ctx, "gpio %s raw read index %d (%d): %s\n",
                          gdev->dev->name, gdev->gpio_base + index,
                          index, value ? "high" : "low");
            if (retval)
                break;
        }
    }
    spin_unlock(&gpio_lock);

    return retval;
}

static struct selftest_command buzzer_command = {
    .group = "driver",
    .name = "gpio",
    .desc = "gpio unit test",
    .testing = buzzer_testing,
};

static state selftest_buzzer_init(void)
{
    return selftest_register(&buzzer_command);
}
kshell_initcall(selftest_buzzer_init);
