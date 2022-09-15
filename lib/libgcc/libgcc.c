/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <libgcc.h>
#include <bitops.h>
#include <popcount.h>
#include <lightcore/swab.h>
#include <panic.h>
#include <export.h>

DWtype __weak __negdi2(DWtype u)
{
    const DWunion uu = {.ll = u};
    const DWunion w = {{
        .low = -uu.s.low,
        .high = -uu.s.high - ((UWtype) -uu.s.low > 0)
    }};
    return w.ll;
}
EXPORT_SYMBOL(__negdi2);

Wtype __weak __addvSI3(Wtype a, Wtype b)
{
    const Wtype w = (UWtype)a + (UWtype)b;
    BUG_ON(!(b >= 0 ? w < a : w > a));
    return w;
}
EXPORT_SYMBOL(__addvSI3);

SItype __weak __addvsi3(SItype a, SItype b)
{
    const SItype w = (USItype)a + (USItype)b;
    BUG_ON(!(b >= 0 ? w < a : w > a));
    return w;
}
EXPORT_SYMBOL(__addvsi3);

DWtype __weak __addvDI3(DWtype a, DWtype b)
{
    const DWtype w = (UDWtype)a + (UDWtype) b;
    BUG_ON(!(b >= 0 ? w < a : w > a));
    return w;
}
EXPORT_SYMBOL(__addvDI3);

Wtype __weak __subvSI3(Wtype a, Wtype b)
{
    const Wtype w = (UWtype)a - (UWtype)b;
    BUG_ON(!(b >= 0 ? w > a : w < a));
    return w;
}
EXPORT_SYMBOL(__subvSI3);

SItype __weak __subvsi3(SItype a, SItype b)
{
    const SItype w = (USItype)a - (USItype)b;
    BUG_ON(!(b >= 0 ? w > a : w < a));
    return w;
}
EXPORT_SYMBOL(__subvsi3);

DWtype __weak __subvDI3(DWtype a, DWtype b)
{
    const DWtype w = (UDWtype)a - (UDWtype)b;
    BUG_ON(!(b >= 0 ? w > a : w < a));
    return w;
}
EXPORT_SYMBOL(__subvDI3);

Wtype __weak __mulvSI3(Wtype a, Wtype b)
{
    const DWtype w = (DWtype)a * (DWtype)b;
    BUG_ON(!((Wtype) (w >> BITS_PER_LONG) != (Wtype) w >> (BITS_PER_LONG - 1)));
    return w;
}
EXPORT_SYMBOL(__mulvSI3);

SItype __weak __mulvsi3(SItype a, SItype b)
{
    const DItype w = (DItype) a * (DItype) b;
    BUG_ON(!((SItype) (w >> WORD_SIZE) != (SItype) w >> (WORD_SIZE-1)));
    return w;
}
EXPORT_SYMBOL(__mulvsi3);

Wtype __weak __negvSI2(Wtype a)
{
    const Wtype w = -(UWtype) a;
    BUG_ON(!(a >= 0 ? w > 0 : w < 0));
    return w;
}
EXPORT_SYMBOL(__negvSI2);

SItype __weak __negvsi2(SItype a)
{
    const SItype w = -(USItype) a;
    BUG_ON(!(a >= 0 ? w > 0 : w < 0));
    return w;
}
EXPORT_SYMBOL(__negvsi2);

DWtype __weak __negvDI2(DWtype a)
{
    const DWtype w = -(UDWtype) a;
    BUG_ON(!(a >= 0 ? w > 0 : w < 0));
    return w;
}
EXPORT_SYMBOL(__negvDI2);

Wtype __weak __absvSI2(Wtype a)
{
    Wtype w = a;

    if (a < 0)
        w = __negvSI2(a);

    return w;
}
EXPORT_SYMBOL(__absvSI2);

SItype __weak __absvsi2(SItype a)
{
    SItype w = a;

    if (a < 0)
        w = __negvsi2(a);

    return w;
}
EXPORT_SYMBOL(__absvsi2);

DWtype __absvDI2(DWtype a)
{
    DWtype w = a;

    if (a < 0)
        w = __negvDI2(a);

    return w;
}
EXPORT_SYMBOL(__absvDI2);

DItype __weak __lshrdi3(DItype u, WDtype b)
{
    DWunion w, uu;
    WDtype bm;

    if (b == 0)
        return u;

    uu.ll = u;
    bm = (sizeof (SItype) * BITS_PER_BYTE) - b;

    if (bm <= 0) {
        w.s.high = 0;
        w.s.low = (USItype)uu.s.high >> -bm;
    } else {
        USItype carries = (USItype)uu.s.high << bm;
        w.s.high = (USItype)uu.s.high >> b;
        w.s.low = ((USItype)uu.s.low >> b) | carries;
    }

    return w.ll;
}
EXPORT_SYMBOL(__lshrdi3);

DItype __weak __ashldi3(DItype u, WDtype b)
{
    DWunion w, uu;
    WDtype bm;

    if (b == 0)
        return u;

    uu.ll = u;
    bm = (sizeof (SItype) * BITS_PER_BYTE) - b;

    if (bm <= 0) {
        w.s.low = 0;
        w.s.high = (USItype)uu.s.low << -bm;
    } else {
        USItype carries = (USItype)uu.s.low >> bm;
        w.s.low = (USItype)uu.s.low << b;
        w.s.high = ((USItype)uu.s.high << b) | carries;
    }

    return w.ll;
}
EXPORT_SYMBOL(__ashldi3);

DWtype __weak __ashrdi3(DWtype u, WDtype b)
{
    const DWunion uu = {.ll = u};
    const WDtype bm = BITS_PER_LONG - b;
    DWunion w;

    if (b == 0)
        return u;

    if (bm <= 0) {
        w.s.high = uu.s.high >> (BITS_PER_LONG - 1);
        w.s.low = uu.s.high >> -bm;
    } else {
        const UWtype carries = (UWtype) uu.s.high << bm;
        w.s.high = uu.s.high >> b;
        w.s.low = ((UWtype) uu.s.low >> b) | carries;
    }

    return w.ll;
}
EXPORT_SYMBOL(__ashrdi3);

SItype __weak __bswapsi2(SItype u)
{
    return swab32(u);
}
EXPORT_SYMBOL(__bswapsi2);

DItype __weak __bswapdi2(DItype u)
{
    return swab64(u);
}
EXPORT_SYMBOL(__bswapdi2);

int __weak __ffsSI2(UWtype u)
{
    UWtype count;

    if (u == 0)
        return 0;

    count = ctz(u);
    return count + 1;
}
EXPORT_SYMBOL(__ffsSI2);

int __weak __ffsDI2(DWtype u)
{
    const DWunion uu = {.ll = u};
    UWtype word, count, add;

    if (uu.s.low != 0)
        word = uu.s.low, add = 0;
    else if (uu.s.high != 0)
        word = uu.s.high, add = BITS_PER_LONG;
    else
        return 0;

    count = ctz(word);
    return count + add + 1;
}
EXPORT_SYMBOL(__ffsDI2);

DWtype __weak __muldi3(DWtype u, DWtype v)
{
    const DWunion uu = {.ll = u};
    const DWunion vv = {.ll = v};
    DWunion w = {.ll = umulsidi3(uu.s.low, vv.s.low)};

    w.s.high += ((UWtype) uu.s.low * (UWtype) vv.s.high
               + (UWtype) uu.s.high * (UWtype) vv.s.low);

    return w.ll;
}
EXPORT_SYMBOL(__muldi3);

int __weak __clzSI2(UWtype x)
{
    return clz(x);
}
EXPORT_SYMBOL(__clzSI2);

int __weak __clzDI2(UDWtype x)
{
    const DWunion uu = {.ll = x};
    UWtype word;
    Wtype add;

    if (uu.s.high) {
        word = uu.s.high;
        add = 0;
    } else {
        word = uu.s.low;
        add = BITS_PER_LONG;
    }

    return clz(word) + add;
}
EXPORT_SYMBOL(__clzDI2);

int __weak __ctzSI2(UWtype x)
{
    return ctz(x);
}
EXPORT_SYMBOL(__ctzSI2);

int __weak __ctzDI2(UDWtype x)
{
    const DWunion uu = {.ll = x};
    UWtype word;
    Wtype add;

    if (uu.s.low) {
        word = uu.s.low;
        add = 0;
    } else {
        word = uu.s.high;
        add = BITS_PER_LONG;
    }

    return ctz(word) + add;
}
EXPORT_SYMBOL(__ctzDI2);

int __weak __clrsbSI2(Wtype x)
{
    if (x < 0)
        x = ~x;

    if (x == 0)
        return BITS_PER_LONG - 1;

    return clz(x) - 1;
}
EXPORT_SYMBOL(__clrsbSI2);

int __weak __clrsbDI2(DWtype x)
{
    const DWunion uu = {.ll = x};
    UWtype word;
    Wtype ret, add;

    if (uu.s.high == 0) {
        word = uu.s.low;
        add = BITS_PER_LONG;
    } else if (uu.s.high == -1) {
        word = ~uu.s.low;
        add = BITS_PER_LONG;
    } else if (uu.s.high >= 0) {
        word = uu.s.high;
        add = 0;
    } else {
        word = ~uu.s.high;
        add = 0;
    }

    if (word == 0)
        ret = BITS_PER_LONG;
    else
        ret = clz(word);

    return ret + add - 1;
}
EXPORT_SYMBOL(__clrsbDI2);

int __weak __popcountSI2(UWtype x)
{
#if BITS_PER_LONG == 32
    return popcount32(x);
#else
    return popcount64(x);
#endif
}
EXPORT_SYMBOL(__popcountSI2);

int __weak __popcountDI2(UDWtype x)
{
    const DWunion uu = {.ll = x};
    int x1 = __popcountSI2(uu.s.low);
    int x2 = __popcountSI2(uu.s.high);
    return x1 + x2;
}
EXPORT_SYMBOL(__popcountDI2);

int __weak __paritySI2(UWtype x)
{
#if BITS_PER_LONG == 64
    return popparity64(x);
#else
    return popparity32(x);
#endif
}
EXPORT_SYMBOL(__paritySI2);

int __weak __parityDI2(UDWtype x)
{
    const DWunion uu = {.ll = x};
    int x1 = __paritySI2(uu.s.low);
    int x2 = __paritySI2(uu.s.high);
    return (x1 + x2) & 1;
}
EXPORT_SYMBOL(__parityDI2);
