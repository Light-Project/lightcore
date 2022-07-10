/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <stddef.h>
#include <filter/ewma.h>
#include <export.h>

short ewma_update(struct ewma_state *state, short value)
{
    if (state->ready)
        state->last = (int)value * state->weight / SHRT_MAX +
            (int)state->last * (SHRT_MAX - state->weight) / SHRT_MAX;
    else {
        state->last = value;
        state->ready = true;
    }

    return state->last;
}
EXPORT_SYMBOL(ewma_update);

void ewma_clear(struct ewma_state *state)
{
    state->ready = false;
    state->last = 0;
}
EXPORT_SYMBOL(ewma_clear);

void ewma_init(struct ewma_state *state, short weight)
{
    state->weight = weight;
    ewma_clear(state);
}
EXPORT_SYMBOL(ewma_init);
