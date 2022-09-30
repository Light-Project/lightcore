/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <export.h>

unsigned int atan2i(int x, int y)
{
    unsigned char negflag, temdeg, comp;
    unsigned int degree, ux, uy;

    negflag = 0;
    if (x < 0) {
        negflag += 0x01;
        x = (0 - x);
    }

    ux = x;
    if (y < 0) {
        negflag += 0x02;
        y = (0 - y);
    }

    uy = y;
    if (ux <= uy)
        degree = (ux * 45) / uy;
    else {
        degree = (uy * 45) / ux;
        negflag += 0x10;
    }

    comp = 0;
    temdeg = degree;

    if (temdeg > 22) {
        if (temdeg <= 44)
            comp++;
        if (temdeg <= 41)
            comp++;
        if (temdeg <= 37)
            comp++;
        if (temdeg <= 32)
            comp++;
    } else {
        if (temdeg >= 2)
            comp++;
        if (temdeg >= 6)
            comp++;
        if (temdeg >= 10)
            comp++;
        if (temdeg >= 15)
            comp++;
    }

    degree += comp;
    if (negflag & 0x10)
        degree = (90 - degree);

    if (negflag & 0x02) {
        if (negflag & 0x01)
            degree = (180 + degree);
        else
            degree = (180 - degree);
    } else {
        if (negflag & 0x01)
            degree = (360 - degree);
    }

    return degree;
}
EXPORT_SYMBOL(atan2i);
