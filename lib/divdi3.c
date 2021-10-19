/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel.h>
#include <export.h>
#include <lightcore/asm/byteorder.h>

typedef unsigned int UQItype __mode(QI);
typedef          int SItype  __mode(SI);
typedef unsigned int USItype __mode(SI);
typedef          int DItype  __mode(DI);
typedef unsigned int UDItype __mode(DI);

#define Wtype   SItype
#define HWtype  SItype
#define DWtype  DItype
#define UWtype  USItype
#define UHWtype USItype
#define UDWtype UDItype
#define W_TYPE_SIZE 32

#if defined(__BIG_ENDIAN__)
struct DWstruct { Wtype high, low;};
#elif  defined(__LITTLE_ENDIAN__)
struct DWstruct { Wtype low, high;};
#else
#error Unhandled endianity
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

#define __BITS4 (W_TYPE_SIZE / 4)
#define __ll_B ((UWtype) 1 << (W_TYPE_SIZE / 2))
#define __ll_lowpart(t) ((UWtype) (t) & (__ll_B - 1))
#define __ll_highpart(t) ((UWtype) (t) >> (W_TYPE_SIZE / 2))

#define __udiv_qrnnd_c(q, r, n1, n0, d)                                             \
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

#define UDIV_NEEDS_NORMALIZATION 1
#define udiv_qrnnd __udiv_qrnnd_c

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

static UDWtype __udivmoddi4(UDWtype n, UDWtype d, UDWtype *rp)
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

#if !UDIV_NEEDS_NORMALIZATION
    if (d1 == 0) {
        if (d0 > n1) {
            /* 0q = nn / 0D */

            udiv_qrnnd (q0, n0, n1, n0, d0);
            q1 = 0;
            /* Remainder in n0.  */
        } else {
            /* qq = NN / 0d */

        if (d0 == 0)
            d0 = 1 / d0;	/* Divide intentionally by zero.  */

        udiv_qrnnd (q1, n1, 0, n1, d0);
        udiv_qrnnd (q0, n0, n1, n0, d0);

        /* Remainder in n0.  */
        }

        if (rp != 0) {
            rr.s.low = n0;
            rr.s.high = 0;
            *rp = rr.ll;
        }
    }

#else /* UDIV_NEEDS_NORMALIZATION */

    if (d1 == 0) {
        if (d0 > n1) {
            /* 0q = nn / 0D */
            count_leading_zeros (bm, d0);

            if (bm != 0)
            {
            /* Normalize, i.e. make the most significant bit of the
                denominator set.  */

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
    }
#endif /* UDIV_NEEDS_NORMALIZATION */

    else {
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

DWtype __divdi3(DWtype u, DWtype v)
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

UDWtype __udivdi3(UDWtype u, UDWtype v)
{
    return __udivmoddi4(u, v, NULL);
}

DWtype __moddi3(DWtype u, DWtype v)
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

UDWtype __umoddi3(UDWtype u, UDWtype v)
{
    UDWtype w;
    __udivmoddi4(u, v, &w);
    return w;
}

EXPORT_SYMBOL(__divdi3);
EXPORT_SYMBOL(__udivdi3);
EXPORT_SYMBOL(__moddi3);
EXPORT_SYMBOL(__umoddi3);
