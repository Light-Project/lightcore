/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PIDC_H_
#define _PIDC_H_

#include <types.h>
#include <stddef.h>
#include <limits.h>

struct pidc_context {
    int32_t kp;
    int32_t ki;
    int32_t kd;

    int32_t outmin;
    int32_t outmax;

    int64_t iterm;
    int32_t lastval;
};

#define PIDC_STATIC(kp, ki, kd, min, max) \
    {(kp), (ki), (kd), (min), (max)}

#define PIDC_INIT(kp, ki, kd, min, max) \
    (struct pidc_context)PIDC_STATIC(kp, ki, kd, min, max)

#define PIDC_CONTEXT(name, kp, ki, kd, min, max) \
    struct pidc_context name = PIDC_INIT(kp, ki, kd, min, max)

extern int32_t pidc_update(struct pidc_context *pctx, int32_t expect, int32_t value);
extern void pidc_reset(struct pidc_context *pctx);
extern void pidc_init(struct pidc_context *pctx, int32_t kp, int32_t ki, int32_t kd, int32_t min, int32_t max);

#endif  /* _PIDC_H_ */
