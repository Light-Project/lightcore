/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOCAL_LIBGCC_H_
#define _LOCAL_LIBGCC_H_

#include <types.h>
#include <stddef.h>
#include <bits.h>

typedef          int QItype  __mode(QI);
typedef          int SItype  __mode(SI);
typedef          int DItype  __mode(DI);
typedef unsigned int UQItype __mode(QI);
typedef unsigned int USItype __mode(SI);
typedef unsigned int UDItype __mode(DI);
typedef          int WDtype  __mode(word);

#if BITS_PER_LONG == 8
# define Wtype      DItype
# define UWtype     UDItype
# define HWtype     DItype
# define UHWtype    UDItype
# define DWtype     TItype
# define UDWtype    UTItype
#else
# define Wtype      SItype
# define UWtype     USItype
# define HWtype     SItype
# define UHWtype    USItype
# define DWtype     DItype
# define UDWtype    UDItype
#endif

#if defined(CONFIG_ARCH_BIG_ENDIAN)
struct DWstruct { Wtype high, low;};
#else
struct DWstruct { Wtype low, high;};
#endif

typedef union {
    struct DWstruct s;
    DWtype ll;
} DWunion;

/* libgcc divhi3 */
extern unsigned short __udivmodhi4(unsigned short u, unsigned short d, int modwanted);
extern unsigned short __udivhi3(unsigned short u, unsigned short v);
extern unsigned short __umodhi3(unsigned short u, unsigned short v);
extern short __divmodhi4(short u, short v, int modwanted);
extern short __divhi3(short u, short v);
extern short __modhi3(short u, short v);

/* libgcc divsi3 */
extern unsigned long __udivmodsi4(unsigned long u, unsigned long d, int modwanted);
extern unsigned long __udivsi3(unsigned long u, unsigned long v);
extern unsigned long __umodsi3(unsigned long u, unsigned long v);
extern long __divmodsi4(long u, long v, int modwanted);
extern long __divsi3(long u, long v);
extern long __modsi3(long u, long v);

/* libgcc divdi3 */
extern UDWtype __udivmoddi4(UDWtype n, UDWtype d, UDWtype *rp);
extern UDWtype __udivdi3(UDWtype u, UDWtype v);
extern UDWtype __umoddi3(UDWtype u, UDWtype v);
extern DWtype __divmoddi4(DWtype u, DWtype v, DWtype *rp);
extern DWtype __divdi3(DWtype u, DWtype v);
extern DWtype __moddi3(DWtype u, DWtype v);
extern DWtype __muldi3(DWtype u, DWtype v);

/* libgcc base */
extern Wtype __negvSI2(Wtype a);
extern Wtype __absvSI2(Wtype a);
extern Wtype __addvSI3(Wtype a, Wtype b);
extern Wtype __subvSI3(Wtype a, Wtype b);
extern Wtype __mulvSI3(Wtype a, Wtype b);
extern DWtype __negvDI2(DWtype a);
extern DWtype __absvDI2(DWtype a);
extern DWtype __addvDI3(DWtype a, DWtype b);
extern DWtype __subvDI3(DWtype a, DWtype b);
extern DWtype __mulvDI3(DWtype a, DWtype b);

/* COMPAT_SIMODE_TRAPPING_ARITHMETIC */
extern SItype __negvsi2(SItype a);
extern SItype __absvsi2(SItype a);
extern SItype __addvsi3(SItype a, SItype b);
extern SItype __subvsi3(SItype a, SItype b);
extern SItype __mulvsi3(SItype a, SItype b);

extern DWtype __negdi2(DWtype u);
extern DItype __lshrdi3(DItype u, WDtype b);
extern DItype __ashldi3(DItype u, WDtype b);
extern DWtype __ashrdi3(DWtype u, WDtype b);
extern SItype __bswapsi2(SItype u);
extern DItype __bswapdi2(DItype u);

extern int __clzSI2(UWtype x);
extern int __clzDI2(UDWtype x);
extern int __ctzSI2(UWtype x);
extern int __ctzDI2(UDWtype x);
extern int __clrsbSI2(Wtype x);
extern int __clrsbDI2(DWtype x);
extern int __ffsSI2(UWtype u);
extern int __ffsDI2(DWtype u);
extern int __popcountSI2(UWtype x);
extern int __popcountDI2(UDWtype x);
extern int __paritySI2(UWtype x);
extern int __parityDI2(UDWtype x);

#endif  /* _LOCAL_LIBGCC_H_ */
