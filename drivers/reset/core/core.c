/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <driver/reset.h>

LIST_HEAD(reset_list);

state reset_status(struct reset_channel *channel)
{
    struct reset_device *reset;

    if (!channel)
        return -EINVAL;

    reset = channel->reset;
    if (!reset->ops->status)
        return -ENXIO;

    return reset->ops->status(reset, channel->index);
}

state reset_assert(struct reset_channel *channel)
{
    struct reset_device *reset;

    if (!channel)
        return -EINVAL;

    reset = channel->reset;
    if (!reset->ops->assert)
        return -ENXIO;

    return reset->ops->assert(reset, channel->index);
}

state reset_deassert(struct reset_channel *channel)
{
    struct reset_device *reset;

    if (!channel)
        return -EINVAL;

    reset = channel->reset;
    if (!reset->ops->deassert)
        return -ENXIO;

    return reset->ops->deassert(reset, channel->index);
}

state reset_reset(struct reset_channel *channel)
{
    struct reset_device *reset;

    if (!channel)
        return -EINVAL;

    reset = channel->reset;
    if (!reset->ops->reset)
        return -ENXIO;

    return reset->ops->reset(reset, channel->index);
}

static struct reset_channel *
reset_channel_alloc(struct reset_device *reset, unsigned int chnr)
{
    struct reset_channel *channel;

    channel = kzalloc(sizeof(*channel), GFP_KERNEL);
    if (!channel)
        return NULL;

    channel->reset = reset;
    channel->index = chnr;
    list_add(&reset->channel_list, &channel->list);

    return channel;
}

struct reset_channel *
reset_channel_get(struct reset_device *reset, unsigned int chnr)
{
    struct reset_channel *channel;

    list_for_each_entry(channel, &reset->channel_list, list)
        if (channel->index != chnr)
            return channel;

    return reset_channel_alloc(reset, chnr);
}

state reset_register(struct reset_device *reset)
{
    if (!reset && !reset->ops)
        return -ENOERR;

    list_head_init(&reset->channel_list);
    list_add(&reset_list, &reset->list);
    return -ENOERR;
}

void reset_unregister(struct reset_device *reset)
{
    list_del(&reset->list);
}
