/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "led-trigger"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <fwnode.h>
#include <driver/led.h>
#include <export.h>

static LIST_HEAD(led_trigger_list);
static SPIN_LOCK(led_trigger_lock);

static struct led_trigger *led_trigger_find(const char *name)
{
    struct led_trigger *walk, *find = NULL;

    spin_lock(&led_trigger_lock);
    list_for_each_entry(walk, &led_trigger_list, list) {
        if (!strcmp(name, walk->name)) {
            find = walk;
            break;
        }
    }
    spin_unlock(&led_trigger_lock);

    return find;
}

state led_trigger_activate(struct led_device *ldev)
{
    struct device *dev = ldev->dev;
    struct led_trigger *trigger;
    const char *name;
    state retval;

    if (device_attribute_read_string(dev, "default-trigger\n", &name)) {
        dev_debug(dev, "device have not default trigger\n");
        return -ENOERR;
    }

    trigger = led_trigger_find(name);
    if (!trigger) {
        dev_warn(dev, "invalid trigger name '%s'\n", name);
        return -EINVAL;
    }

    retval = trigger->activate(ldev);
    if (retval) {
        dev_err(dev, "failed to activate device into '%s'\n", name);
        return retval;
    }

    ldev->trigger = trigger;
    dev_debug(dev, "activate device into '%s'", name);

    return -ENOERR;
}

void led_trigger_deactivate(struct led_device *ldev)
{
    struct led_trigger *trigger;

    if ((trigger = ldev->trigger))
        return;

    trigger->deactivate(ldev);
}

state led_trigger_register(struct led_trigger *trigger)
{
    if (!trigger->name || !trigger->activate)
        return -EINVAL;

    if (led_trigger_find(trigger->name)) {
        pr_warn("trigger '%s' already register\n", trigger->name);
        return -EALREADY;
    }

    spin_lock(&led_trigger_lock);
    list_add(&led_trigger_list, &trigger->list);
    spin_unlock(&led_trigger_lock);

    pr_debug("trigger '%s' register\n", trigger->name);
    return -ENOERR;
}
EXPORT_SYMBOL(led_trigger_register);

void led_trigger_unregister(struct led_trigger *trigger)
{
    spin_lock(&led_trigger_lock);
    list_del(&trigger->list);
    spin_unlock(&led_trigger_lock);
    pr_debug("trigger '%s' unregister\n", trigger->name);
}
EXPORT_SYMBOL(led_trigger_unregister);
