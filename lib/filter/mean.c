/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <minmax.h>
#include <log2.h>
#include <string.h>
#include <kmalloc.h>
#include <filter/mean.h>
#include <export.h>

int32_t mean_update(struct mean_state *state, int32_t value)
{
    unsigned int count;
    int32_t retval;

    count = state->count++;
    if (state->count == state->length) {
        state->count = 0;
        state->btrace = true;
    }

    state->gross -= state->buffer[count];
    state->gross += value;
    state->buffer[count] = value;

    if (state->btrace)
        retval = DIV_ROUND_CLOSEST(state->gross, state->length);
    else
        retval = DIV_ROUND_CLOSEST(state->gross, state->count);

    return retval;
}
EXPORT_SYMBOL(mean_update);

void mean_clear(struct mean_state *state)
{
    state->gross = 0;
    state->count = 0;
    state->btrace = false;
    memset(state->buffer, 0, sizeof(*state->buffer) * state->length);
}
EXPORT_SYMBOL(mean_clear);

struct mean_state *mean_alloc(unsigned int length, gfp_t flags)
{
    struct mean_state *state;

    state = kmalloc(sizeof(*state) + sizeof(*state->buffer) * length, flags);
    if (unlikely(!state))
        return NULL;

    state->length = length;
    mean_clear(state);

    return state;
}
EXPORT_SYMBOL(mean_alloc);

void mean_free(struct mean_state *state)
{
    kfree(state);
}
EXPORT_SYMBOL(mean_free);
