/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <export.h>

uint32_t bezier3(uint32_t time, uint32_t p0, uint32_t p1,
                 uint32_t p2, uint32_t p3)
{
    uint32_t rem1, rem2, rem3, tim2, tim3;
    uint32_t v1, v2, v3, v4;

    rem1 = 1024 - time;
    rem2 = (rem1 * rem1) >> 10;
    rem3 = (rem2 * rem1) >> 10;

    tim2 = (time * time) >> 10;
    tim3 = (tim2 * time) >> 10;

    v1 = (rem3 * p0) >> 10;
    v2 = (rem2 * p1 * time * 3) >> 20;
    v3 = (rem1 * p2 * tim2 * 3) >> 20;
    v4 = (tim3 * p3) >> 10;

    return v1 + v2 + v3 + v4;
}
EXPORT_SYMBOL(bezier3);
