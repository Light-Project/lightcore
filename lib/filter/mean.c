/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <range.h>
#include <log2.h>
#include <string.h>
#include <kmalloc.h>
#include <filter/mean.h>
#include <export.h>

long mean_update(struct mean_state *state, long value)
{
    unsigned int index = state->count & state->mask;
    unsigned int length = state->mask + 1;

    state->gross -= state->buffer[index];
    state->gross += value;
    state->buffer[index] = value;
    state->count++;

    return state->gross / min(state->count, length);
}
EXPORT_SYMBOL(mean_update);

void mean_clear(struct mean_state *state)
{
    state->gross = state->count = 0;
    memset(state->buffer, 0, state->mask + 1);
}
EXPORT_SYMBOL(mean_clear);

struct mean_state *mean_alloc(unsigned int length, gfp_t flags)
{
    struct mean_state *state;

    length = roundup_power_of_2(length);
    if (length < 2)
        return NULL;

    state = kzalloc(sizeof(*state) + sizeof(*state->buffer) * length, flags);
    if (unlikely(!state))
        return NULL;

    state->mask = length - 1;
    return state;
}
EXPORT_SYMBOL(mean_alloc);

void mean_free(struct mean_state *state)
{
    kfree(state);
}
EXPORT_SYMBOL(mean_free);
