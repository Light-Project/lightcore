/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/gpio.h>
#include <export.h>

LIST_HEAD(gpio_list);
SPIN_LOCK(gpio_lock);

struct gpio_device *gpio_device_find(bool (*match)(struct gpio_device *gdev, void *pdata), void *pdata)
{
    struct gpio_device *walk, *find = NULL;
    irqflags_t irqflags;

    spin_lock_irqsave(&gpio_lock, &irqflags);
    list_for_each_entry(walk, &gpio_list, list) {
        if (match(walk, pdata)) {
            find = walk;
            break;
        }
    }
    spin_unlock_irqrestore(&gpio_lock, &irqflags);

    return find;
}

static struct gpio_channel *gpio_alloc_channel(struct gpio_device *gdev, irqnr_t chnr)
{
    struct gpio_channel *channel;

    channel = dev_kzalloc(gdev->dev, sizeof(*channel), GFP_KERNEL);
    if (!channel)
        return NULL;

    channel->gdev = gdev;
    channel->index = chnr;
    list_add(&gdev->channel, &channel->list);

    return channel;
}

struct gpio_channel *gpio_channel_get(struct gpio_device *gdev, unsigned int index)
{
    struct gpio_channel *channel;
    irqflags_t irqflags;

    if (index >= gdev->gpio_num)
        return NULL;

    spin_lock_irqsave(&gdev->lock, &irqflags);
    list_for_each_entry(channel, &gdev->channel, list) {
        if (channel->index == index) {
            channel->using++;
            spin_unlock_irqrestore(&gdev->lock, &irqflags);
            return channel;
        }
    }

    channel = gpio_alloc_channel(gdev, index);
    spin_unlock_irqrestore(&gdev->lock, &irqflags);

    return channel;
}
EXPORT_SYMBOL(gpio_channel_get);

void gpio_channel_put(struct gpio_channel *channel)
{
    struct gpio_device *gdev = channel->gdev;
    irqflags_t irqflags;

    spin_lock_irqsave(&gdev->lock, &irqflags);
    if (channel->using--)
        return;

    list_del(&channel->list);
    spin_unlock_irqrestore(&gdev->lock, &irqflags);

    dev_kfree(gdev->dev, channel);
}

static state gpio_channel_base(unsigned int ngpio, unsigned int *base)
{
    struct gpio_device *gdev;
    unsigned int find = 0;

	list_for_each_entry(gdev, &gpio_list, list) {
        if (find + ngpio < gdev->gpio_base)
            break;
        find = gdev->gpio_base + gdev->gpio_num;
    }

    if (gpio_valid(find + ngpio)) {
        pr_debug("found new base at %u\n", find);
        *base = find;
        return -ENOERR;
    } else {
        pr_err("cannot find free range for %u\n", ngpio);
        return -ENOSPC;
    }
}

struct gpio_channel *gpio_channel_number(unsigned int index)
{
    struct gpio_channel *channel;
    struct gpio_device *gdev;
    irqflags_t irqflags;

	spin_lock_irqsave(&gpio_lock, &irqflags);
    list_for_each_entry(gdev, &gpio_list, list) {
        if (index < gdev->gpio_base ||
            index >= gdev->gpio_base + gdev->gpio_num)
            continue;

        channel = gpio_channel_get(gdev, index - gdev->gpio_base);
	    spin_unlock_irqrestore(&gpio_lock, &irqflags);
        return channel;
    }

	spin_unlock_irqrestore(&gpio_lock, &irqflags);
    pr_warn("invalid gpio %u\n", index);

    return NULL;
}
EXPORT_SYMBOL(gpio_channel_number);

state gpio_register(struct gpio_device *gdev)
{
    unsigned int base;
    irqflags_t irqflags;
    state retval;

    if (!gdev->ops || !gdev->dev)
        return -EINVAL;

    list_head_init(&gdev->channel);
    spin_lock_irqsave(&gpio_lock, &irqflags);

    if (gdev->gpio_base == UINT_MAX) {
        retval = gpio_channel_base(gdev->gpio_num, &base);
        if (unlikely(retval)) {
            spin_unlock_irqrestore(&gpio_lock, &irqflags);
            return retval;
        }
        gdev->gpio_base = base;
    }

    list_add_prev(&gpio_list, &gdev->list);
    spin_unlock_irqrestore(&gpio_lock, &irqflags);

    return -ENOERR;
}

void gpio_unregister(struct gpio_device *gdev)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&gdev->lock, &irqflags);
    list_del(&gpio_list);
    spin_unlock_irqrestore(&gpio_lock, &irqflags);
}
