/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <filter/pidc.h>
#include <minmax.h>
#include <export.h>

int32_t pidc_update(struct pidc_context *pctx, int32_t expect, int32_t value)
{
    int64_t pout, iout, dout;
    int32_t retval;

    pout = ((int64_t)pctx->kp * (expect - value)) / INT16_MAX;
    iout = ((int64_t)pctx->ki * (expect - value)) / INT16_MAX;
    dout = ((int64_t)pctx->kd * (value - pctx->lastval)) / INT16_MAX;
    pctx->lastval = value;

    pctx->iterm += iout;
    clamp_adj(pctx->iterm, pctx->outmin, pctx->outmax);

    retval = pout + pctx->iterm + dout;
    clamp_adj(retval, pctx->outmin, pctx->outmax);

    return retval;
}
EXPORT_SYMBOL(pidc_update);

void pidc_reset(struct pidc_context *pctx)
{
    pctx->iterm = 0;
    pctx->lastval = 0;
}
EXPORT_SYMBOL(pidc_reset);

void pidc_init(struct pidc_context *pctx, int32_t kp, int32_t ki,
               int32_t kd, int32_t min, int32_t max)
{
    pctx->kp = kp;
    pctx->ki = ki;
    pctx->kd = kd;
    pctx->outmin = min;
    pctx->outmax = max;
    pidc_reset(pctx);
}
EXPORT_SYMBOL(pidc_init);
