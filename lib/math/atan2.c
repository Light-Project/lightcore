/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <export.h>

#define NEGATX 0x01
#define NEGATY 0x02
#define OCTANT 0x04

unsigned int atan2i(int x, int y)
{
    unsigned char temp, negflag = 0;
    unsigned int degree, uintx, uinty;

    if (x < 0) {
        negflag |= NEGATX;
        x = -x;
    }

    if (y < 0) {
        negflag |= NEGATY;
        y = -y;
    }

    uintx = (unsigned int)x;
    uinty = (unsigned int)y;

    if (uintx <= uinty)
        degree = (uintx * 45) / uinty;
    else {
        degree = (uinty * 45) / uintx;
        negflag |= OCTANT;
    }

    if ((temp = (unsigned char)degree) > 22) {
        if (temp <= 32) degree += 4; else
        if (temp <= 37) degree += 3; else
        if (temp <= 41) degree += 2; else
        if (temp <= 44) degree += 1;
    } else {
        if (temp >= 15) degree += 4; else
        if (temp >= 10) degree += 3; else
        if (temp >=  6) degree += 2; else
        if (temp >=  2) degree += 1;
    }

    if (negflag & OCTANT)
        degree = 90 - degree;

    if (negflag & NEGATY) {
        if (negflag & NEGATX)
            degree = 180 + degree;
        else
            degree = 180 - degree;
    } else {
        if (negflag & NEGATX)
            degree = 360 - degree;
    }

    return degree;
}
EXPORT_SYMBOL(atan2i);
