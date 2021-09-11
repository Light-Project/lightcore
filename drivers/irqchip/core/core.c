/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "irqchip: " fmt

#include <mm.h>
#include <initcall.h>
#include <driver/irqchip.h>
#include <printk.h>

LIST_HEAD(irqchip_list);

state irqchip_pass(struct irqchip_channel *channel)
{
    struct irqchip_device *irqchip;

    if (!channel)
        return -EINVAL;

    irqchip = channel->irqchip;
    if (!irqchip->ops->pass)
        return -ENXIO;

    return irqchip->ops->pass(irqchip, channel->index);
}

state irqchip_mask(struct irqchip_channel *channel)
{
    struct irqchip_device *irqchip;

    if (!channel)
        return -EINVAL;

    irqchip = channel->irqchip;
    if (!irqchip->ops->mask)
        return -ENXIO;

    return irqchip->ops->mask(irqchip, channel->index);
}

state irqchip_ack(struct irqchip_channel *channel)
{
    struct irqchip_device *irqchip;

    if (!channel)
        return -EINVAL;

    irqchip = channel->irqchip;
    if (!irqchip->ops->ack)
        return -ENXIO;

    return irqchip->ops->ack(irqchip, channel->index);
}

state irqchip_eoi(struct irqchip_channel *channel)
{
    struct irqchip_device *irqchip;

    if (!channel)
        return -EINVAL;

    irqchip = channel->irqchip;
    if (!irqchip->ops->eoi)
        return -ENXIO;

    return irqchip->ops->eoi(irqchip, channel->index);
}

static struct irqchip_channel *
irqchip_alloc_channel(struct irqchip_device *idev, irqnr_t chnr)
{
    struct irqchip_channel *channel;

    channel = kzalloc(sizeof(struct irqchip_channel), GFP_KERNEL);
    if (!channel)
        return NULL;

    channel->irqchip = idev;
    channel->index = chnr;
    list_add(&idev->channel_list, &channel->list);

    return channel;
}

struct irqchip_channel *irqchip_get_channel(struct irqchip_device *idev, irqnr_t chnr)
{
    struct irqchip_channel *channel;

    list_for_each_entry(channel, &idev->channel_list, list)
        if (channel->index == chnr)
            return channel;

    return irqchip_alloc_channel(idev, chnr);
}

state irqchip_register(struct irqchip_device *idev)
{
    if(!idev || !idev->ops)
        return -EINVAL;

    list_head_init(&idev->channel_list);
    list_add(&irqchip_list, &idev->list);
    return -ENOERR;
}

void irqchip_unregister(struct irqchip_device *idev)
{
    if(!idev || !idev->ops)
        return;

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
}
