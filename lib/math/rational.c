/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <minmax.h>
#include <limits.h>
#include <export.h>

state rational(unsigned int numer, unsigned int denomin,
               unsigned int maxnumer, unsigned int maxdenomin,
               unsigned int *bestnumer, unsigned int *bestdenomin)
{
    unsigned int numer0, denomin0, numer1, denomin1;
    unsigned int bnumer, bdenomin;

    if (!bestnumer || !bestdenomin)
        return -EINVAL;

    numer0 = bdenomin = 0;
    denomin0 = bnumer = 1;

    while (denomin) {
        unsigned int save, result;

        save = denomin;
        result = numer / denomin;
        denomin = numer % denomin;
        numer = save;

        numer1 = numer0 + result * bnumer;
        denomin1 = denomin0 + result * bdenomin;

        if ((numer1 > maxnumer) || (denomin1 > maxdenomin)) {
            unsigned int nvalue = UINT_MAX, dvalue = UINT_MAX;
            unsigned int value;

            if (bnumer)
                nvalue = (maxnumer - numer0) / bnumer;
            if (bdenomin)
                dvalue = (maxdenomin - denomin0) / bdenomin;
            value = min(nvalue, dvalue);

            if (!bdenomin || value * 2 > result || (value * 2 == result &&
                 denomin0 * save > denomin * bdenomin)) {
                bnumer = numer0 + value * bnumer;
                bdenomin = denomin0 + value * bdenomin;
            }

            break;
        }

        numer0 = bnumer;
        bnumer = numer1;

        denomin0 = bdenomin;
        bdenomin = denomin1;
    }

    *bestnumer = bnumer;
    *bestdenomin = bdenomin;

    return -ENOERR;
}
EXPORT_SYMBOL(rational);
