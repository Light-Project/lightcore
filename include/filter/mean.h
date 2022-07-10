/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_MEAN_H_
#define _FILTER_MEAN_H_

#include <types.h>
#include <stddef.h>
#include <mm/gfp.h>

struct mean_state {
    int count;
    int mask;
    int gross;
    int buffer[0];
};

extern int mean_update(struct mean_state *state, int value);
extern void mean_clear(struct mean_state *state);
extern struct mean_state *mean_alloc(int length, gfp_t flags);
extern void mean_free(struct mean_state *state);

#endif  /* _FILTER_MEAN_H_ */
