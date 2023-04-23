/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_MEAN_H_
#define _FILTER_MEAN_H_

#include <types.h>
#include <stddef.h>
#include <mm/gfp.h>

struct mean_state {
    unsigned int count;
    unsigned int length;
    bool btrace;
    int64_t gross;
    int32_t buffer[];
};

extern int32_t mean_update(struct mean_state *state, int32_t value);
extern void mean_clear(struct mean_state *state);
extern struct mean_state *mean_alloc(unsigned int length, gfp_t flags);
extern void mean_free(struct mean_state *state);

#endif /* _FILTER_MEAN_H_ */
