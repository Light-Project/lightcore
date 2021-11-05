/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "irqchip: " fmt

#include <kmalloc.h>
#include <initcall.h>
#include <driver/irqchip.h>
#include <printk.h>

LIST_HEAD(irqchip_list);

state irqchip_pass(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->pass)
        return -ENXIO;

    return idev->ops->pass(idev, channel->index);
}

state irqchip_mask(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->mask)
        return -ENXIO;

    return idev->ops->mask(idev, channel->index);
}

state irqchip_ack(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->ack)
        return -ENXIO;

    return idev->ops->ack(idev, channel->index);
}

state irqchip_eoi(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->eoi)
        return -ENXIO;

    return idev->ops->eoi(idev, channel->index);
}

state irqchip_slave_add(struct irqchip_channel *channel,
                        struct irqchip_device *slave)
{
    struct irqchip_device *idev = channel->irqchip;
    state ret;

    if (!idev->ops->slave_add)
        return -ENXIO;

    if ((ret = idev->ops->slave_add(idev, channel->index)))
        return ret;

    list_add(&idev->child, &slave->sibling);
    return -ENOERR;
}

state irqchip_slave_del(struct irqchip_channel *channel,
                        struct irqchip_device *slave)
{
    struct irqchip_device *idev = channel->irqchip;
    state ret;

    if (!idev->ops->slave_del)
        return -ENXIO;

    if ((ret = idev->ops->slave_del(idev, channel->index)))
        return ret;

    list_del(&slave->sibling);
    return -ENOERR;
}

static struct irqchip_channel *
irqchip_alloc_channel(struct irqchip_device *idev, irqnr_t chnr)
{
    struct irqchip_channel *channel;

    channel = dev_kzalloc(idev->dev, sizeof(*channel), GFP_KERNEL);
    if (!channel)
        return NULL;

    channel->irqchip = idev;
    channel->index = chnr;
    list_add(&idev->channel, &channel->list);

    return channel;
}

struct irqchip_channel *irqchip_channel_get(struct irqchip_device *idev, irqnr_t chnr)
{
    struct irqchip_channel *channel;

    list_for_each_entry(channel, &idev->channel, list) {
        if (channel->index == chnr)
            return channel;
    }

    return irqchip_alloc_channel(idev, chnr);
}

void irqchip_channel_release(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (irqchip_mask(channel))
        dev_err(idev->dev, "channel%ld release", channel->index);

    list_del(&channel->list);
    dev_kfree(idev->dev, channel);
}

state irqchip_register(struct irqchip_device *idev)
{
    if (!idev->dev || !idev->ops)
        return -EINVAL;

    list_head_init(&idev->child);
    list_head_init(&idev->channel);
    list_add(&irqchip_list, &idev->list);
    return -ENOERR;
}

void irqchip_unregister(struct irqchip_device *idev)
{
    struct irqchip_device *child;

    if (!idev->dev || !idev->ops)
        return;

    list_for_each_entry(child, &idev->child, sibling) {
        bus_remove_device(child->dev);
    }

    list_del(&idev->list);
}

static void irqchip_slave_setup(void)
{
    struct irqchip_device *idev;
    struct irqchip_channel *channel;

    list_for_each_entry(idev, &irqchip_list, list) {
        if (!(channel = dt_get_irqchip_channel(idev->dt_node, 0)))
            continue;
        irqchip_slave_add(channel, idev);
    }
}

void __init irqchip_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, irqchip_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }

    irqchip_slave_setup();
}
