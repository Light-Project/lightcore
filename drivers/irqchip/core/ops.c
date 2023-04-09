/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/irqchip.h>
#include <export.h>

/**
 * irqchip_mask - pass the interrupt gate of a channel.
 * @channel: channel to be pass.
 */
state irqchip_pass(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->pass)
        return -ENXIO;

    return idev->ops->pass(idev, channel->index);
}
EXPORT_SYMBOL(irqchip_pass);

/**
 * irqchip_mask - mask the interrupt gate of a channel.
 * @channel: channel to be mask.
 */
state irqchip_mask(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->mask)
        return -ENXIO;

    return idev->ops->mask(idev, channel->index);
}
EXPORT_SYMBOL(irqchip_mask);

/**
 * irqchip_ack - manually trigger interrupt of a channel.
 * @channel: channel to be triggered manually.
 */
state irqchip_ack(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->ack)
        return -ENXIO;

    return idev->ops->ack(idev, channel->index);
}
EXPORT_SYMBOL(irqchip_ack);

/**
 * irqchip_eoi - end the interrupt of a channel.
 * @channel: channel to terminate interrupt.
 */
state irqchip_eoi(struct irqchip_channel *channel)
{
    struct irqchip_device *idev = channel->irqchip;

    if (!idev->ops->eoi)
        return -ENXIO;

    return idev->ops->eoi(idev, channel->index);
}
EXPORT_SYMBOL(irqchip_eoi);

/**
 * irqchip_slave_add - add a child interrupt controller to the interrupt controller.
 * @channel: The channel to which the slave controller is to be added.
 * @slave: slave interrupt controller to add.
 */
state irqchip_slave_add(struct irqchip_channel *channel, struct irqchip_device *slave)
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
EXPORT_SYMBOL(irqchip_slave_add);

/**
 * irqchip_slave_del - remove a child interrupt controller from an interrupt controller.
 * @channel: The channel to which the slave controller is to be removed.
 * @slave: slave interrupt controller to remove.
 */
state irqchip_slave_del(struct irqchip_channel *channel, struct irqchip_device *slave)
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
EXPORT_SYMBOL(irqchip_slave_del);
