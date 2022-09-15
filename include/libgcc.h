/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOCAL_LIBGCC_H_
#define _LOCAL_LIBGCC_H_

#include <types.h>
#include <stddef.h>
#include <bits.h>

typedef          int QItype  __mode(QI);
typedef unsigned int UQItype __mode(QI);
typedef          int SItype  __mode(SI);
typedef unsigned int USItype __mode(SI);
typedef          int DItype  __mode(DI);
typedef unsigned int UDItype __mode(DI);
typedef          int WDtype  __mode(word);

#define Wtype   SItype
#define HWtype  SItype
#define UWtype  USItype
#define UHWtype USItype
#define DWtype  DItype
#define UDWtype UDItype

#define __ll_B ((UWtype)1 << (BITS_PER_LONG / 2))
#define __ll_lowpart(t) ((UWtype)(t) & (__ll_B - 1))
#define __ll_highpart(t) ((UWtype)(t) >> (BITS_PER_LONG / 2))
#define WORD_SIZE (sizeof(SItype) * BITS_PER_BYTE)

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

#if defined(CONFIG_ARCH_BIG_ENDIAN)
struct DWstruct { Wtype high, low;};
#else
struct DWstruct { Wtype low, high;};
#endif

typedef union {
    struct DWstruct s;
    DWtype ll;
} DWunion;

extern UDWtype __udivmoddi4(UDWtype n, UDWtype d, UDWtype *rp);
extern DWtype __divmoddi4(DWtype u, DWtype v, DWtype *rp);
extern UDWtype __udivdi3(UDWtype u, UDWtype v);
extern DWtype __divdi3(DWtype u, DWtype v);
extern UDWtype __umoddi3(UDWtype u, UDWtype v);
extern DWtype __moddi3(DWtype u, DWtype v);

extern DWtype __negdi2(DWtype u);
extern Wtype __addvSI3(Wtype a, Wtype b);
extern SItype __addvsi3(SItype a, SItype b);
extern DWtype __addvDI3(DWtype a, DWtype b);
extern Wtype __subvSI3(Wtype a, Wtype b);
extern SItype __subvsi3(SItype a, SItype b);
extern DWtype __subvDI3(DWtype a, DWtype b);
extern Wtype __mulvSI3(Wtype a, Wtype b);
extern SItype __mulvsi3(SItype a, SItype b);
extern Wtype __negvSI2(Wtype a);
extern SItype __negvsi2(SItype a);
extern DWtype __negvDI2(DWtype a);
extern Wtype __absvSI2(Wtype a);
extern SItype __absvsi2(SItype a);
extern DItype __lshrdi3(DItype u, WDtype b);
extern DItype __ashldi3(DItype u, WDtype b);
extern DWtype __ashrdi3(DWtype u, WDtype b);
extern SItype __bswapsi2(SItype u);
extern DItype __bswapdi2(DItype u);
extern int __ffsSI2(UWtype u);
extern int __ffsDI2(DWtype u);
extern DWtype __muldi3(DWtype u, DWtype v);
extern int __clzSI2(UWtype x);
extern int __clzDI2(UDWtype x);
extern int __ctzSI2(UWtype x);
extern int __ctzDI2(UDWtype x);
extern int __clrsbSI2(Wtype x);
extern int __clrsbDI2(DWtype x);
extern int __popcountSI2(UWtype x);
extern int __popcountDI2(UDWtype x);
extern int __paritySI2(UWtype x);
extern int __parityDI2(UDWtype x);

#endif  /* _LOCAL_LIBGCC_H_ */
