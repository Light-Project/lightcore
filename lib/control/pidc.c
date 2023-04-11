/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <control/pidc.h>
#include <minmax.h>
#include <math.h>
#include <export.h>

int32_t pidc_update(struct pidc_state *pctx, int32_t expect, int32_t value)
{
    int64_t pout, iout, dout;
    int32_t retval;

    pout = DIV_ROUND_CLOSEST((int64_t)pctx->kpm * (expect - value), pctx->kpd);
    iout = DIV_ROUND_CLOSEST((int64_t)pctx->kim * (expect - value), pctx->kid);
    dout = DIV_ROUND_CLOSEST((int64_t)pctx->kdm * (pctx->lastval - value), pctx->kdd);
    pctx->lastval = value;

    pctx->iterm += iout;
    clamp_adj(pctx->iterm, pctx->outmin, pctx->outmax);

    retval = pout + pctx->iterm + dout;
    clamp_adj(retval, pctx->outmin, pctx->outmax);

    return retval;
}
EXPORT_SYMBOL(pidc_update);

void pidc_reset(struct pidc_state *pctx)
{
    pctx->iterm = 0;
    pctx->lastval = 0;
}
EXPORT_SYMBOL(pidc_reset);

void pidc_init(struct pidc_state *pctx, uint32_t kpm, int32_t kpd, uint32_t kim,
               int32_t kid, uint32_t kdm, int32_t kdd, int32_t min, int32_t max)
{
    pctx->kpm = kpm;
    pctx->kim = kim;
    pctx->kdm = kdm;

    pctx->kpd = kpd;
    pctx->kid = kid;
    pctx->kdd = kdd;

    pctx->outmin = min;
    pctx->outmax = max;
    pidc_reset(pctx);
}
EXPORT_SYMBOL(pidc_init);
