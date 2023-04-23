/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_FIR_H_
#define _FILTER_FIR_H_

#include <types.h>
#include <stddef.h>
#include <mm/gfp.h>

struct fir_state {
    unsigned int count;
    unsigned int length;
    const int32_t *coeffs;
    int32_t buffer[0];
};

extern int32_t fir_update(struct fir_state *state, int32_t value);
extern void fir_clear(struct fir_state *state);
extern struct fir_state *fir_alloc(const int32_t *coeffs, unsigned int length, gfp_t flags);
extern void fir_free(struct fir_state *state);

#endif /* _FILTER_FIR_H_ */
