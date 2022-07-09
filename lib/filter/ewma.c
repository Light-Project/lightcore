/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <stddef.h>
#include <filter/ewma.h>
#include <export.h>

uint16_t ewma_update(struct ewma_state *state, uint16_t value)
{
    if (state->ready)
        state->last = (uint32_t)value * state->weight / UINT16_MAX +
            (uint32_t)state->last * (UINT16_MAX - state->weight) / UINT16_MAX;
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

void ewma_init(struct ewma_state *state, uint16_t weight)
{
    state->weight = weight;
    ewma_clear(state);
}
EXPORT_SYMBOL(ewma_init);
