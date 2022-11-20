/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <stddef.h>
#include <filter/ewma.h>
#include <export.h>

int32_t ewma_update(struct ewma_state *state, int32_t value)
{
    if (state->ready) {
        state->last = (int64_t)value * state->weight / INT32_MAX +
            (int64_t)state->last * (INT32_MAX - state->weight) / INT32_MAX;
    } else {
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

void ewma_init(struct ewma_state *state, int32_t weight)
{
    state->weight = weight;
    ewma_clear(state);
}
EXPORT_SYMBOL(ewma_init);
