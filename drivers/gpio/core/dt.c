/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "gpio-dt"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/gpio.h>
#include <dt-bindings/gpio.h>
#include <export.h>

static LIST_HEAD(gpio_dt_quirk_list);
static SPIN_LOCK(gpio_dt_quirk_lock);

static bool dt_match_device_by_xlate(struct gpio_device *gdev, void *pdata)
{
    struct dt_phandle_args *gpargs = pdata;
    return gdev->dt_node == gpargs->node &&
           !gpio_raw_dt_xlate(gdev, gpargs, NULL, NULL);
}

static inline struct gpio_device *
dt_get_device_by_xlate(struct dt_phandle_args *args)
{
    return gpio_device_find(dt_match_device_by_xlate, args);
}

static inline struct gpio_channel *
dt_get_channel_by_xlate(struct gpio_device *gdev, struct dt_phandle_args *args, uint32_t *flags)
{
    uint32_t index;
    state retval;

    if (gdev->ops->dt_gpio_cells != args->args_count)
        return ERR_PTR(-EINVAL);

    retval = gpio_raw_dt_xlate(gdev, args, &index, flags);
    if (retval)
        return ERR_PTR(retval);

    return gpio_channel_get(gdev, index);
}

struct gpio_channel *gpio_dt_channel_prop(struct dt_node *node, const char *prop,
                                          int index, uint32_t *flags)
{
	struct dt_phandle_args gpargs;
    struct gpio_device *gdev;
    struct gpio_channel *channel;
    state retval;

    retval = dt_parse_phandle_with_args_map(node, prop, "gpio", index, &gpargs);
    if (retval) {
        pr_debug("can't parse '%s' property of node (%p)\n", prop, node);
        return ERR_PTR(retval);
    }

    gdev = dt_get_device_by_xlate(&gpargs);
    if (!gdev) {
        pr_debug("can't get device by '%s' property of node (%p)\n", prop, node);
        return ERR_PTR(-ENOENT);
    }

    channel = dt_get_channel_by_xlate(gdev, &gpargs, flags);
    if (IS_INVAL(channel))
        pr_debug("can't get channel by '%s' property of node (%p)\n", prop, node);

    return channel;
}
EXPORT_SYMBOL(gpio_dt_channel_prop);

struct gpio_channel *gpio_dt_channel(struct dt_node *node, const char *label,
                                     int index, enum gpio_lookup_flags *flags)
{
    struct gpio_dt_quirk *quirk;
    struct gpio_channel *channel;
    uint32_t bind_flags;
    unsigned int count;
    const char *prop;
    char buff[32];

    for (count = 0; count < ARRAY_SIZE(gpio_suffixes); ++count) {
        if (!label)
            prop = gpio_suffixes[count];
        else {
            snprintf(buff, sizeof(buff), "%s-%s", label, gpio_suffixes[count]);
            prop = buff;
        }

        channel = gpio_dt_channel_prop(node, prop, index, &bind_flags);
        if (!IS_INVAL(channel))
            goto finish;
    }

    spin_lock(&gpio_dt_quirk_lock);
    list_for_each_entry(quirk, &gpio_dt_quirk_list, list) {
        channel = quirk->lookup(node, prop, index, &bind_flags);
        if (!IS_INVAL(channel))
            break;
    }
    spin_unlock(&gpio_dt_quirk_lock);

finish:
    if (bind_flags & GPIO_ACTIVE_LOW)
        *flags |= GPIOL_ACTIVE_LOW;

    if (bind_flags & GPIO_SINGLE_ENDED) {
        if (bind_flags & GPIO_LINE_OPEN_DRAIN)
            *flags |= GPIOL_OPEN_DRAIN;
        else
            *flags |= GPIOL_OPEN_SOURCE;
    }

    if (bind_flags & GPIO_TRANSITORY)
        *flags |= GPIOL_TRANSITORY;

    if (bind_flags & GPIO_PULL_UP)
        *flags |= GPIOL_PULL_UP;

    if (bind_flags & GPIO_PULL_DOWN)
        *flags |= GPIOL_PULL_DOWN;

    return channel;
}
EXPORT_SYMBOL(gpio_dt_channel);

state gpio_dt_quirk_register(struct gpio_dt_quirk *quirk)
{
    if (!quirk->lookup)
        return -EINVAL;

    spin_lock(&gpio_dt_quirk_lock);
    list_add(&gpio_dt_quirk_list, &quirk->list);
    spin_unlock(&gpio_dt_quirk_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(gpio_dt_quirk_register);

void gpio_dt_quirk_unregister(struct gpio_dt_quirk *quirk)
{
    spin_lock(&gpio_dt_quirk_lock);
    list_del(&quirk->list);
    spin_unlock(&gpio_dt_quirk_lock);
}
EXPORT_SYMBOL(gpio_dt_quirk_unregister);
