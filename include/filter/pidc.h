/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_PIDC_H_
#define _FILTER_PIDC_H_

#include <types.h>
#include <stddef.h>
#include <limits.h>

struct pidc_state {
    uint32_t kpm;
    uint32_t kim;
    uint32_t kdm;

    int32_t kpd;
    int32_t kid;
    int32_t kdd;

    int32_t outmin;
    int32_t outmax;

    int64_t iterm;
    int32_t lastval;
};

#define PIDC_STATIC(kpm, kpd, kim, kid, kdm, kdd, min, max) \
    {(kpm), (kim), (kdm), (kpd), (kid), (kdd), (min), (max)}

#define PIDC_INIT(kpm, kpd, kim, kid, kdm, kdd, min, max) \
    (struct pidc_state)PIDC_STATIC(kpm, kpd, kim, kid, kdm, kdd, min, max)

#define PIDC_STATE(name, kpm, kpd, kim, kid, kdm, kdd, min, max) \
    struct pidc_state name = PIDC_INIT(kpm, kpd, kim, kid, kdm, kdd, min, max)

extern int32_t pidc_update(struct pidc_state *pctx, int32_t expect, int32_t value);
extern void pidc_reset(struct pidc_state *pctx);
extern void pidc_init(struct pidc_state *pctx, uint32_t kpm, int32_t kpd, uint32_t kim, int32_t kid, uint32_t kdm, int32_t kdd, int32_t min, int32_t max);

#endif  /* _FILTER_PIDC_H_ */
