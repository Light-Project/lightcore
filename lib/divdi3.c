/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bits.h>
#include <panic.h>
#include <export.h>

typedef          int QItype  __mode(QI);
typedef unsigned int UQItype __mode(QI);
typedef          int HItype  __mode(HI);
typedef unsigned int UHItype __mode(HI);
typedef          int SItype  __mode(SI);
typedef unsigned int USItype __mode(SI);
typedef          int DItype  __mode(DI);
typedef unsigned int UDItype __mode(DI);
typedef        float DFtype  __mode(DF);
typedef          int WDtype  __mode(word);

#define Wtype       SItype
#define HWtype      SItype
#define DWtype      DItype
#define UWtype      USItype
#define UHWtype     USItype
#define UDWtype     UDItype

#define W_TYPE_SIZE BITS_PER_LONG

#if defined(CONFIG_ARCH_BIG_ENDIAN)
struct DWstruct { Wtype high, low;};
#elif defined(CONFIG_ARCH_LITTLE_ENDIAN)
struct DWstruct { Wtype low, high;};
#else
# error "unhandled endianity"
#endif

typedef union {
    struct DWstruct s;
    DWtype ll;
} DWunion;

const UQItype __clz_tab[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

#define __BITS4 (W_TYPE_SIZE / 4)
#define __ll_B ((UWtype) 1 << (W_TYPE_SIZE / 2))
#define __ll_lowpart(t) ((UWtype) (t) & (__ll_B - 1))
#define __ll_highpart(t) ((UWtype) (t) >> (W_TYPE_SIZE / 2))

#define count_leading_zeros(count, x)                                               \
    do {                                                                            \
        UWtype __xr = (x);                                                          \
        UWtype __a;                                                                 \
                                                                                    \
        if (W_TYPE_SIZE <= 32) {                                                    \
            __a = __xr < ((UWtype)1<<2*__BITS4)                                     \
            ? (__xr < ((UWtype)1<<__BITS4) ? 0 : __BITS4)                           \
            : (__xr < ((UWtype)1<<3*__BITS4) ?  2*__BITS4 : 3*__BITS4);             \
        } else {                                                                    \
            for (__a = W_TYPE_SIZE - 8; __a > 0; __a -= 8)                          \
                if (((__xr >> __a) & 0xff) != 0)                                    \
                    break;                                                          \
        }                                                                           \
                                                                                    \
        (count) = W_TYPE_SIZE - (__clz_tab[__xr >> __a] + __a);                     \
    } while (0)

#define udiv_qrnnd(q, r, n1, n0, d)                                                 \
    do {                                                                            \
        UWtype __d1, __d0, __q1, __q0;                                              \
        UWtype __r1, __r0, __m;                                                     \
        __d1 = __ll_highpart (d);                                                   \
        __d0 = __ll_lowpart (d);                                                    \
                                                                                    \
        __r1 = (n1) % __d1;                                                         \
        __q1 = (n1) / __d1;                                                         \
        __m = (UWtype) __q1 * __d0;                                                 \
        __r1 = __r1 * __ll_B | __ll_highpart (n0);                                  \
        if (__r1 < __m) {                                                           \
            __q1--, __r1 += (d);                                                    \
            if (__r1 >= (d)) /* i.e. we didn't get carry when adding to __r1 */     \
                if (__r1 < __m)                                                     \
                    __q1--, __r1 += (d);                                            \
        }                                                                           \
        __r1 -= __m;                                                                \
                                                                                    \
        __r0 = __r1 % __d1;                                                         \
        __q0 = __r1 / __d1;                                                         \
        __m = (UWtype) __q0 * __d0;                                                 \
        __r0 = __r0 * __ll_B | __ll_lowpart (n0);                                   \
        if (__r0 < __m) {                                                           \
            __q0--, __r0 += (d);                                                    \
            if (__r0 >= (d))                                                        \
                if (__r0 < __m)                                                     \
                    __q0--, __r0 += (d);                                            \
        }                                                                           \
        __r0 -= __m;                                                                \
                                                                                    \
        (q) = (UWtype) __q1 * __ll_B | __q0;                                        \
        (r) = __r0;                                                                 \
    } while (0)

#define umul_ppmm(w1, w0, u, v)                                                     \
    do {                                                                            \
        UWtype __x0, __x1, __x2, __x3;                                              \
        UHWtype __ul, __vl, __uh, __vh;                                             \
                                                                                    \
        __ul = __ll_lowpart (u);                                                    \
        __uh = __ll_highpart (u);                                                   \
        __vl = __ll_lowpart (v);                                                    \
        __vh = __ll_highpart (v);                                                   \
                                                                                    \
        __x0 = (UWtype) __ul * __vl;                                                \
        __x1 = (UWtype) __ul * __vh;                                                \
        __x2 = (UWtype) __uh * __vl;                                                \
        __x3 = (UWtype) __uh * __vh;                                                \
                                                                                    \
        __x1 += __ll_highpart (__x0);   /* this can't give carry */                 \
        __x1 += __x2;                   /* but this indeed can */                   \
        if (__x1 < __x2)                /* did we get it? */                        \
        __x3 += __ll_B;                 /* yes, add it in the proper pos.  */       \
                                                                                    \
        (w1) = __x3 + __ll_highpart (__x1);                                         \
        (w0) = __ll_lowpart (__x1) * __ll_B + __ll_lowpart (__x0);                  \
    } while (0)

#define sub_ddmmss(sh, sl, ah, al, bh, bl)                                          \
    do {                                                                            \
        UWtype __x;                                                                 \
        __x = (al) - (bl);                                                          \
        (sh) = (ah) - (bh) - (__x > (al));                                          \
        (sl) = __x;                                                                 \
    } while (0)

#define __umulsidi3(u, v) ({                                                        \
    DWunion __w;                                                                    \
    umul_ppmm (__w.s.high, __w.s.low, u, v);                                        \
    __w.ll;                                                                         \
})

#define count_trailing_zeros(count, x)                                              \
    do {                                                                            \
        UWtype __ctz_x = (x);                                                       \
        UWtype __ctz_c;                                                             \
        count_leading_zeros (__ctz_c, __ctz_x & -__ctz_x);                          \
        (count) = W_TYPE_SIZE - 1 - __ctz_c;                                        \
    } while (0)

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
    const Wtype w = (UWtype) a + (UWtype) b;
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
    const DWtype w = (UDWtype) a + (UDWtype) b;
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
    BUG_ON(!((Wtype) (w >> W_TYPE_SIZE) != (Wtype) w >> (W_TYPE_SIZE - 1)));
    return w;
}
EXPORT_SYMBOL(__mulvSI3);

#define WORD_SIZE (sizeof (SItype) * __CHAR_BIT__)
SItype __weak __mulvsi3 (SItype a, SItype b)
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
    if (b == 0)
        return u;

    const DWunion uu = {.ll = u};
    const WDtype bm = W_TYPE_SIZE - b;
    DWunion w;

    if (bm <= 0) {
        /* w.s.high = 1..1 or 0..0 */
        w.s.high = uu.s.high >> (W_TYPE_SIZE - 1);
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
    return ((((u) & 0xff000000) >> 24)
        | (((u) & 0x00ff0000) >>  8)
        | (((u) & 0x0000ff00) <<  8)
        | (((u) & 0x000000ff) << 24));
}
EXPORT_SYMBOL(__bswapsi2);

DItype __weak __bswapdi2(DItype u)
{
    return ((((u) & 0xff00000000000000ull) >> 56)
        | (((u) & 0x00ff000000000000ull) >> 40)
        | (((u) & 0x0000ff0000000000ull) >> 24)
        | (((u) & 0x000000ff00000000ull) >>  8)
        | (((u) & 0x00000000ff000000ull) <<  8)
        | (((u) & 0x0000000000ff0000ull) << 24)
        | (((u) & 0x000000000000ff00ull) << 40)
        | (((u) & 0x00000000000000ffull) << 56));
}
EXPORT_SYMBOL(__bswapdi2);

int __weak __ffsSI2(UWtype u)
{
    UWtype count;

    if (u == 0)
        return 0;

    count_trailing_zeros (count, u);
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
        word = uu.s.high, add = W_TYPE_SIZE;
    else
        return 0;

    count_trailing_zeros (count, word);
    return count + add + 1;
}
EXPORT_SYMBOL(__ffsDI2);

DWtype __weak __muldi3(DWtype u, DWtype v)
{
    const DWunion uu = {.ll = u};
    const DWunion vv = {.ll = v};
    DWunion w = {.ll = __umulsidi3 (uu.s.low, vv.s.low)};

    w.s.high += ((UWtype) uu.s.low * (UWtype) vv.s.high
            + (UWtype) uu.s.high * (UWtype) vv.s.low);

    return w.ll;
}
EXPORT_SYMBOL(__muldi3);

UDWtype __weak __udivmoddi4(UDWtype n, UDWtype d, UDWtype *rp)
{
    DWunion ww;
    DWunion nn, dd;
    DWunion rr;
    UWtype d0, d1, n0, n1, n2;
    UWtype q0, q1;
    UWtype b, bm;

    nn.ll = n;
    dd.ll = d;

    d0 = dd.s.low;
    d1 = dd.s.high;
    n0 = nn.s.low;
    n1 = nn.s.high;

    if (d1 == 0) {
        if (d0 > n1) {
            /* 0q = nn / 0D */
            count_leading_zeros (bm, d0);

            if (bm != 0) {
                /* Normalize, i.e. make the most significant bit of the denominator set.  */
                d0 = d0 << bm;
                n1 = (n1 << bm) | (n0 >> (W_TYPE_SIZE - bm));
                n0 = n0 << bm;
            }

            udiv_qrnnd (q0, n0, n1, n0, d0);
            q1 = 0;

        /* Remainder in n0 >> bm.  */
        } else {
            /* qq = NN / 0d */
            if (d0 == 0)
                d0 = 1 / d0;	/* Divide intentionally by zero.  */

            count_leading_zeros (bm, d0);

            if (bm == 0) {
                /* From (n1 >= d0) /\ (the most significant bit of d0 is set),
                conclude (the most significant bit of n1 is set) /\ (the
                leading quotient digit q1 = 1).
                This special case is necessary, not an optimization.
                (Shifts counts of W_TYPE_SIZE are undefined.)  */
                n1 -= d0;
                q1 = 1;
            } else {
                /* Normalize.  */
                b = W_TYPE_SIZE - bm;

                d0 = d0 << bm;
                n2 = n1 >> b;
                n1 = (n1 << bm) | (n0 >> b);
                n0 = n0 << bm;
                udiv_qrnnd (q1, n1, n2, n1, d0);
            }

            /* n1 != d0...  */
            udiv_qrnnd (q0, n0, n1, n0, d0);

            /* Remainder in n0 >> bm.  */
        }

        if (rp != 0) {
            rr.s.low = n0 >> bm;
            rr.s.high = 0;
            *rp = rr.ll;
        }
    } else {
        if (d1 > n1) {
            /* 00 = nn / DD */
            q0 = 0;
            q1 = 0;

            /* Remainder in n1n0.  */
            if (rp != 0) {
                rr.s.low = n0;
                rr.s.high = n1;
                *rp = rr.ll;
            }
        } else {
            /* 0q = NN / dd */

            count_leading_zeros (bm, d1);
            if (bm == 0) {
                /* From (n1 >= d1) /\ (the most significant bit of d1 is set),
                conclude (the most significant bit of n1 is set) /\ (the
                quotient digit q0 = 0 or 1).

                This special case is necessary, not an optimization.  */

                /* The condition on the next line takes advantage of that
                n1 >= d1 (true due to program flow).  */
                if (n1 > d1 || n0 >= d0) {
                    q0 = 1;
                    sub_ddmmss (n1, n0, n1, n0, d1, d0);
                } else
                    q0 = 0;

                q1 = 0;

                if (rp != 0) {
                    rr.s.low = n0;
                    rr.s.high = n1;
                    *rp = rr.ll;
                }
            } else {
                UWtype m1, m0;
                /* Normalize.  */

                b = W_TYPE_SIZE - bm;

                d1 = (d1 << bm) | (d0 >> b);
                d0 = d0 << bm;
                n2 = n1 >> b;
                n1 = (n1 << bm) | (n0 >> b);
                n0 = n0 << bm;

                udiv_qrnnd (q0, n1, n2, n1, d1);
                umul_ppmm (m1, m0, q0, d0);

                if (m1 > n1 || (m1 == n1 && m0 > n0)) {
                    q0--;
                    sub_ddmmss(m1, m0, m1, m0, d1, d0);
                }

                q1 = 0;

                /* Remainder in (n1n0 - m1m0) >> bm.  */
                if (rp != 0) {
                    sub_ddmmss (n1, n0, n1, n0, m1, m0);
                    rr.s.low = (n1 << b) | (n0 >> bm);
                    rr.s.high = n1 >> bm;
                    *rp = rr.ll;
                }
            }
        }
    }

    ww.s.low = q0;
    ww.s.high = q1;
    return ww.ll;
}

DWtype __weak __divdi3(DWtype u, DWtype v)
{
    Wtype c = 0;
    DWtype w;

    if (u < 0) {
        c = ~c;
        u = -u;
    }

    if (v < 0) {
        c = ~c;
        v = -v;
    }

    w = __udivmoddi4 (u, v, NULL);
    return c ? -w : w;
}
EXPORT_SYMBOL(__divdi3);

UDWtype __weak __udivdi3(UDWtype u, UDWtype v)
{
    return __udivmoddi4(u, v, NULL);
}
EXPORT_SYMBOL(__udivdi3);

DWtype __weak __moddi3(DWtype u, DWtype v)
{
    Wtype c = 0;
    DWtype w;

    if (u < 0) {
        c = ~c;
        u = -u;
    }

    if (v < 0)
        v = -v;

    __udivmoddi4 (u, v, (UDWtype *) &w);
    return c ? -w : w;
}
EXPORT_SYMBOL(__moddi3);

UDWtype __weak __umoddi3(UDWtype u, UDWtype v)
{
    UDWtype w;
    __udivmoddi4(u, v, &w);
    return w;
}
EXPORT_SYMBOL(__umoddi3);
