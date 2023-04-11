/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * lib/control/pidi.c: increase pid.
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <control/pidi.h>
#include <minmax.h>
#include <math.h>
#include <export.h>

int32_t pidi_update(struct pidi_state *pctx, int32_t expect, int32_t value)
{
    int64_t pout, iout, dout;
    int32_t retval, error;

    error = expect - value;
    pout = DIV_ROUND_CLOSEST((int64_t)pctx->kpm * error, pctx->kpd);
    iout = DIV_ROUND_CLOSEST((int64_t)pctx->kim * error, pctx->kid);

    retval = error - pctx->lasterr[0] * 2 + pctx->lasterr[1];
    dout = DIV_ROUND_CLOSEST((int64_t)pctx->kdm * value, pctx->kdd);

    pctx->lasterr[0] = error;
    pctx->lasterr[1] = pctx->lasterr[0];

    retval = pout + iout + dout;
    clamp_adj(retval, pctx->outmin, pctx->outmax);

    return retval;
}
EXPORT_SYMBOL(pidi_update);

void pidi_reset(struct pidi_state *pctx)
{
    pctx->iterm = 0;
    pctx->lasterr[0] = 0;
    pctx->lasterr[1] = 0;
}
EXPORT_SYMBOL(pidi_reset);

void pidi_init(struct pidi_state *pctx, uint32_t kpm, int32_t kpd, uint32_t kim,
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
    pidi_reset(pctx);
}
EXPORT_SYMBOL(pidi_init);
