/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <minmax.h>
#include <log2.h>
#include <string.h>
#include <kmalloc.h>
#include <filter/fir.h>
#include <export.h>

int32_t fir_update(struct fir_state *state, int32_t value)
{
    unsigned int index, coidx;
    int64_t retval = 0;

    index = state->count++;
    if (state->count == state->length)
        state->count = 0;

    state->buffer[index] = value;
    coidx = state->length;

    do {
        retval += (int64_t)state->buffer[index] *
                  state->coeffs[--coidx];
    } while (index--);

    for (index = state->length - 1; coidx; --index) {
        retval += (int64_t)state->buffer[index] *
                  state->coeffs[--coidx];
    }

    return (int32_t)upper_32_bits(retval);
}
EXPORT_SYMBOL(fir_update);

void fir_clear(struct fir_state *state)
{
    state->count = 0;
    memset(state->buffer, 0, sizeof(*state->buffer) * state->length);
}
EXPORT_SYMBOL(fir_clear);

struct fir_state *fir_alloc(const int32_t *coeffs, unsigned int length, gfp_t flags)
{
    struct fir_state *state;

    state = kmalloc(sizeof(*state) + sizeof(*state->buffer) * length, flags);
    if (unlikely(!state))
        return NULL;

    state->coeffs = coeffs;
    state->length = length;
    fir_clear(state);

    return state;
}
EXPORT_SYMBOL(fir_alloc);

void fir_free(struct fir_state *state)
{
    kfree(state);
}
EXPORT_SYMBOL(fir_free);
