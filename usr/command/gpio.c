/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>
#include <driver/gpio.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: gpio [option] index=val index\n");
    kshell_printf(ctx, "\t-d  dump all gpio status\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static void gpio_dumpall(struct kshell_context *ctx)
{
    struct gpio_device *gdev;
    unsigned int index;
    bool value;

    spin_lock(&gpio_lock);
    list_for_each_entry(gdev, &gpio_list, list) {
        for (index = 0; index < gdev->gpio_num; ++index) {
            gpio_raw_value_get(gdev, index, &value);
            kshell_printf(ctx, "%s index %d (%d): %s\n", gdev->dev->name,
                          gdev->gpio_base + index, index, value ? "high" : "low");
        }
    }
    spin_unlock(&gpio_lock);
}

static state gpio_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, index;
    bool dumpall = false;
    state retval = -ENOERR;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'd':
                dumpall = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count || dumpall) {
        gpio_dumpall(ctx);
        return -ENOERR;
    }

    do {
        struct gpio_channel *channel;
        bool value;

        index = strtoul(argv[count], &argv[count], 0);
        if (*argv[count] != '=' && *argv[count] != '\0')
            goto usage;

        channel = gpio_channel_number(index);
        if (!channel)
            continue;

        if (*argv[count]++ == '=') {
            value = atoi(argv[count]);
            retval = gpioc_value_set(channel, value);
        } else {
            retval = gpioc_value_get(channel, &value);
            kshell_printf(ctx, "%d\n", value);
        }

        gpio_channel_put(channel);
    } while (!retval && ++count < argc);

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command gpio_cmd = {
    .name = "gpio",
    .desc = "gpio control tool",
    .exec = gpio_main,
};

static state gpio_init(void)
{
    return kshell_register(&gpio_cmd);
}
kshell_initcall(gpio_init);
