/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CONTROL_PIDR_H_
#define _CONTROL_PIDR_H_

#include <types.h>
#include <stddef.h>
#include <limits.h>

struct pidr_state {
    uint32_t kpm;
    uint32_t kim;
    uint32_t kdm;

    int32_t kpd;
    int32_t kid;
    int32_t kdd;

    int32_t outmin;
    int32_t outmax;

    int64_t iterm;
    int32_t lasterr;
};

#define PIDR_STATIC(kpm, kpd, kim, kid, kdm, kdd, min, max) \
    {(kpm), (kim), (kdm), (kpd), (kid), (kdd), (min), (max)}

#define PIDR_INIT(kpm, kpd, kim, kid, kdm, kdd, min, max) \
    (struct pidr_state)PIDR_STATIC(kpm, kpd, kim, kid, kdm, kdd, min, max)

#define PIDR_STATE(name, kpm, kpd, kim, kid, kdm, kdd, min, max) \
    struct pidr_state name = PIDR_INIT(kpm, kpd, kim, kid, kdm, kdd, min, max)

extern int32_t pidr_update(struct pidr_state *pctx, int32_t expect, int32_t value);
extern void pidr_reset(struct pidr_state *pctx);
extern void pidr_init(struct pidr_state *pctx, uint32_t kpm, int32_t kpd, uint32_t kim, int32_t kid, uint32_t kdm, int32_t kdd, int32_t min, int32_t max);

#endif /* _CONTROL_PIDR_H_ */
