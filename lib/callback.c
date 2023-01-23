/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <callback.h>
#include <export.h>

void cbfunc_noop(void)
{
    /* Nothing */
}
EXPORT_SYMBOL(cbfunc_noop);

long cbfunc_ret0(void)
{
    return 0;
}
EXPORT_SYMBOL(cbfunc_ret0);

long cbfunc_ret1(void)
{
    return 1;
}
EXPORT_SYMBOL(cbfunc_ret1);
