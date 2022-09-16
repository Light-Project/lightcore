/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <libgcc.h>
#include <bitops.h>
#include <export.h>

#define __ll_B ((UWtype)1 << (BITS_PER_LONG / 2))
#define __ll_lowpart(t) ((UWtype)(t) & (__ll_B - 1))
#define __ll_highpart(t) ((UWtype)(t) >> (BITS_PER_LONG / 2))

#define sub_ddmmss(sh, sl, ah, al, bh, bl) do {                     \
    UWtype __x;                                                     \
    __x = (al) - (bl);                                              \
    (sh) = (ah) - (bh) - (__x > (al));                              \
    (sl) = __x;                                                     \
} while (0)

#define udiv_qrnnd(q, r, n1, n0, d) do {                            \
    UWtype __d1, __d0, __q1, __q0;                                  \
    UWtype __r1, __r0, __m;                                         \
    __d1 = __ll_highpart(d);                                        \
    __d0 = __ll_lowpart(d);                                         \
    __r1 = (n1) % __d1;                                             \
    __q1 = (n1) / __d1;                                             \
    __m = (UWtype) __q1 * __d0;                                     \
    __r1 = __r1 * __ll_B | __ll_highpart(n0);                       \
    if (__r1 < __m) {                                               \
        __q1--, __r1 += (d);                                        \
        if (__r1 >= (d))                                            \
            if (__r1 < __m)                                         \
                __q1--, __r1 += (d);                                \
    }                                                               \
    __r1 -= __m;                                                    \
    __r0 = __r1 % __d1;                                             \
    __q0 = __r1 / __d1;                                             \
    __m = (UWtype) __q0 * __d0;                                     \
    __r0 = __r0 * __ll_B | __ll_lowpart(n0);                        \
    if (__r0 < __m) {                                               \
        __q0--, __r0 += (d);                                        \
        if (__r0 >= (d))                                            \
            if (__r0 < __m)                                         \
                __q0--, __r0 += (d);                                \
    }                                                               \
    __r0 -= __m;                                                    \
    (q) = (UWtype)__q1 * __ll_B | __q0;                             \
    (r) = __r0;                                                     \
} while (0)

#define umul_ppmm(w1, w0, u, v) do {                                \
    UWtype __x0, __x1, __x2, __x3;                                  \
    UHWtype __ul, __vl, __uh, __vh;                                 \
    __ul = __ll_lowpart(u);                                         \
    __uh = __ll_highpart(u);                                        \
    __vl = __ll_lowpart(v);                                         \
    __vh = __ll_highpart(v);                                        \
    __x0 = (UWtype) __ul * __vl;                                    \
    __x1 = (UWtype) __ul * __vh;                                    \
    __x2 = (UWtype) __uh * __vl;                                    \
    __x3 = (UWtype) __uh * __vh;                                    \
    __x1 += __ll_highpart(__x0);                                    \
    __x1 += __x2;                                                   \
    if (__x1 < __x2)                                                \
        __x3 += __ll_B;                                             \
    (w1) = __x3 + __ll_highpart(__x1);                              \
    (w0) = __ll_lowpart(__x1) * __ll_B + __ll_lowpart(__x0);        \
} while (0)

#define umulsidi3(u, v) ({                                          \
    DWunion __w;                                                    \
    umul_ppmm (__w.s.high, __w.s.low, u, v);                        \
    __w.ll;                                                         \
})

UDWtype __weak __udivmoddi4(UDWtype n, UDWtype d, UDWtype *rp)
{
    DWunion ww, nn, dd, rr;
    UWtype d0, d1, n0, n1, n2;
    UWtype q0, q1, b, bm;

    nn.ll = n;
    dd.ll = d;

    d0 = dd.s.low;
    d1 = dd.s.high;
    n0 = nn.s.low;
    n1 = nn.s.high;

    if (d1 == 0) {
        if (d0 > n1) {
            bm = clz(d0);
            if (bm != 0) {
                d0 = d0 << bm;
                n1 = (n1 << bm) | (n0 >> (BITS_PER_LONG - bm));
                n0 = n0 << bm;
            }
            udiv_qrnnd(q0, n0, n1, n0, d0);
            q1 = 0;
        } else {
            if (d0 == 0)
                d0 = 1 / d0;
            bm = clz(d0);
            if (bm == 0) {
                n1 -= d0;
                q1 = 1;
            } else {
                b = BITS_PER_LONG - bm;
                d0 = d0 << bm;
                n2 = n1 >> b;
                n1 = (n1 << bm) | (n0 >> b);
                n0 = n0 << bm;
                udiv_qrnnd(q1, n1, n2, n1, d0);
            }
            udiv_qrnnd(q0, n0, n1, n0, d0);
        }

        if (rp != 0) {
            rr.s.low = n0 >> bm;
            rr.s.high = 0;
            *rp = rr.ll;
        }
    } else {
        if (d1 > n1) {
            q0 = 0;
            q1 = 0;

            if (rp != 0) {
                rr.s.low = n0;
                rr.s.high = n1;
                *rp = rr.ll;
            }
        } else {
            bm = clz(d1);
            if (bm == 0) {
                if (n1 > d1 || n0 >= d0) {
                    q0 = 1;
                    sub_ddmmss(n1, n0, n1, n0, d1, d0);
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

                b = BITS_PER_LONG - bm;
                d1 = (d1 << bm) | (d0 >> b);
                d0 = d0 << bm;
                n2 = n1 >> b;
                n1 = (n1 << bm) | (n0 >> b);
                n0 = n0 << bm;

                udiv_qrnnd(q0, n1, n2, n1, d1);
                umul_ppmm(m1, m0, q0, d0);

                if (m1 > n1 || (m1 == n1 && m0 > n0)) {
                    q0--;
                    sub_ddmmss(m1, m0, m1, m0, d1, d0);
                }

                q1 = 0;
                if (rp != 0) {
                    sub_ddmmss(n1, n0, n1, n0, m1, m0);
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
EXPORT_SYMBOL(__udivmoddi4);

UDWtype __weak __udivdi3(UDWtype u, UDWtype v)
{
    return __udivmoddi4(u, v, NULL);
}
EXPORT_SYMBOL(__udivdi3);

UDWtype __weak __umoddi3(UDWtype u, UDWtype v)
{
    UDWtype w;
    __udivmoddi4(u, v, &w);
    return w;
}
EXPORT_SYMBOL(__umoddi3);

DWtype __weak __divmoddi4(DWtype u, DWtype v, DWtype *rp)
{
    Wtype c1 = 0, c2 = 0;
    DWtype w, r;

    if (u < 0) {
        c1 = ~c1;
        c2 = ~c2;
        u = -u;
    }

    if (v < 0) {
        c1 = ~c1;
        v = -v;
    }

    w = __udivmoddi4(u, v, (UDWtype*)&r);
    if (c1)
        w = -w;

    if (c2)
        r = -r;

    *rp = r;
    return w;
}
EXPORT_SYMBOL(__divmoddi4);

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

    w = __udivmoddi4(u, v, NULL);
    return c ? -w : w;
}
EXPORT_SYMBOL(__divdi3);

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

    __udivmoddi4(u, v, (UDWtype *)&w);
    return c ? -w : w;
}
EXPORT_SYMBOL(__moddi3);

DWtype __weak __muldi3(DWtype u, DWtype v)
{
    const DWunion uu = {.ll = u};
    const DWunion vv = {.ll = v};
    DWunion w = {.ll = umulsidi3(uu.s.low, vv.s.low)};

    w.s.high += ((UWtype)vv.s.high * (UWtype)uu.s.low +
                 (UWtype)uu.s.high * (UWtype)vv.s.low);

    return w.ll;
}
EXPORT_SYMBOL(__muldi3);
