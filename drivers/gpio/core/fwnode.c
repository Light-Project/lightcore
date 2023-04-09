/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "gpio-fwnode"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/gpio.h>
#include <printk.h>
#include <export.h>

const char *const gpio_suffixes[2] = {
    "gpio", "gpios",
};

struct gpio_channel *gpio_fwnode_channel_index(struct fwnode *node, const char *label,
                                               int index, enum gpio_request_flags flags)
{
    const char *path = fwnode_node_path(node);
    enum gpio_lookup_flags lflags = 0;
    struct gpio_channel *channel;
    state retval;

#ifdef CONFIG_GPIO_DT
    if (fwnode_test_dtree(node)) {
        struct dt_node *dt_node = fwnode_node_container(node);
        pr_debug("%s: lookup channel using device tree\n", path);
        channel = gpio_dt_channel(dt_node, label, index, &lflags);
    } else
#endif /* CONFIG_GPIO_DT */
#ifdef CONFIG_GPIO_ACPI
    if (fwnode_test_acpi(node)) {
        struct acpi_node *dt = fwnode_node_container(node);
        pr_debug("%s: lookup channel using acpi\n", path);
        channel = acpi_dt_channel(dt_node, label, index, &lflags);
    } else
#endif /* CONFIG_GPIO_ACPI */
    {
        pr_err("%s: fwnode type not supported\n", path);
        return ERR_PTR(-EBADF);
    }

    if (unlikely(IS_INVAL(channel))) {
        pr_warn("%s: no gpio consumer find\n", path);
        return channel;
    }

    retval = gpioc_configure(channel, lflags, flags);
    if (retval)
        return ERR_PTR(retval);

    return channel;
}
EXPORT_SYMBOL(gpio_fwnode_channel_index);

struct gpio_channel *gpio_fwnode_channel(struct fwnode *node, const char *label,
                                         enum gpio_request_flags flags)
{
    return gpio_fwnode_channel_index(node, label, 0, flags);
}
EXPORT_SYMBOL(gpio_fwnode_channel);
