/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <libgcc.h>
#include <bitops.h>
#include <export.h>

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

UDWtype __weak __udivdi3(UDWtype u, UDWtype v)
{
    return __udivmoddi4(u, v, NULL);
}
EXPORT_SYMBOL(__udivdi3);

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

UDWtype __weak __umoddi3(UDWtype u, UDWtype v)
{
    UDWtype w;
    __udivmoddi4(u, v, &w);
    return w;
}
EXPORT_SYMBOL(__umoddi3);

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

    __udivmoddi4(u, v, (UDWtype *) &w);
    return c ? -w : w;
}
EXPORT_SYMBOL(__moddi3);
