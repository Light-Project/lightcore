/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "irqchip"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kmalloc.h>
#include <initcall.h>
#include <driver/irqchip.h>
#include <printk.h>

LIST_HEAD(irqchip_list);

static struct irqchip_channel *irqchip_alloc_channel(struct irqchip_device *idev, irqnr_t chnr)
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

/**
 * irqchip_register - Register an irqchip controller.
 * @idev: controller to register.
 */
state irqchip_register(struct irqchip_device *idev)
{
    if (!idev->dev || !idev->ops)
        return -EINVAL;

    list_head_init(&idev->child);
    list_head_init(&idev->channel);
    list_add(&irqchip_list, &idev->list);

    return -ENOERR;
}

/**
 * irqchip_register - Unregister an irqchip controller.
 * @idev: controller to unregister.
 */
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

void __init irqchip_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, irqchip_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }

#ifdef CONFIG_IRQCHIP_DT
    dt_irqchip_slave_setup();
#endif
}
