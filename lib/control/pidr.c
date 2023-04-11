/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * lib/control/pidr.c: realize pid.
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <control/pidr.h>
#include <minmax.h>
#include <math.h>
#include <export.h>

int32_t pidr_update(struct pidr_state *pctx, int32_t expect, int32_t value)
{
    int64_t pout, iout, dout;
    int32_t retval, error;

    error = expect - value;
    pout = DIV_ROUND_CLOSEST((int64_t)pctx->kpm * error, pctx->kpd);
    iout = DIV_ROUND_CLOSEST((int64_t)pctx->kim * error, pctx->kid);

    retval = error - pctx->lasterr;
    dout = DIV_ROUND_CLOSEST((int64_t)pctx->kdm * retval, pctx->kdd);
    pctx->lasterr = error;

    pctx->iterm += iout;
    clamp_adj(pctx->iterm, pctx->outmin, pctx->outmax);

    retval = pout + pctx->iterm + dout;
    clamp_adj(retval, pctx->outmin, pctx->outmax);

    return retval;
}
EXPORT_SYMBOL(pidr_update);

void pidr_reset(struct pidr_state *pctx)
{
    pctx->iterm = 0;
    pctx->lasterr = 0;
}
EXPORT_SYMBOL(pidr_reset);

void pidr_init(struct pidr_state *pctx, uint32_t kpm, int32_t kpd, uint32_t kim,
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
    pidr_reset(pctx);
}
EXPORT_SYMBOL(pidr_init);
