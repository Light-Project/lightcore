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
typedef        float SFtype  __mode(SF);
typedef        float DFtype  __mode(DF);
typedef int CMPtype __mode(__libgcc_cmp_return__);
typedef int SHCtype __mode(__libgcc_shift_count__);

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

/* libgcc divsi3 */
extern UDItype __udivmodsi4(UDItype u, UDItype d, int modwanted);
extern UDItype __udivsi3(UDItype u, UDItype v);
extern UDItype __umodsi3(UDItype u, UDItype v);
extern DItype __divmodsi4(DItype u, DItype v, int modwanted);
extern DItype __divsi3(DItype u, DItype v);
extern DItype __modsi3(DItype u, DItype v);

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

/* libgcc soft-fp */
extern DFtype __adddf3(DFtype a, DFtype b);
extern SFtype __addsf3(SFtype a, SFtype b);
extern DFtype __divdf3(DFtype a, DFtype b);
extern SFtype __divsf3(SFtype a, SFtype b);
extern CMPtype __eqdf2(DFtype a, DFtype b);
extern CMPtype __eqsf2(SFtype a, SFtype b);
extern DFtype __extendsfdf2(SFtype a);
extern SItype __fixdfsi(DFtype a);
extern SItype __fixsfsi(SFtype a);
extern DFtype __floatsidf(SItype i);
extern SFtype __floatsisf(SItype i);
extern DFtype __floatunsidf(USItype i);
extern SFtype __floatunsisf(USItype i);
extern CMPtype __gedf2(DFtype a, DFtype b);
extern CMPtype __gesf2(SFtype a, SFtype b);
extern CMPtype __ledf2(DFtype a, DFtype b);
extern CMPtype __lesf2(SFtype a, SFtype b);
extern DFtype __muldf3(DFtype a, DFtype b);
extern SFtype __mulsf3(SFtype a, SFtype b);
extern DFtype __negdf2(DFtype a);
extern SFtype __negsf2(SFtype a);
extern DFtype __subdf3(DFtype a, DFtype b);
extern SFtype __subsf3(SFtype a, SFtype b);
extern SFtype __truncdfsf2(DFtype a);
extern CMPtype __unorddf2(DFtype a, DFtype b);
extern CMPtype __unordsf2(SFtype a, SFtype b);

/* LIBGCC2_HAS_SF_MODE */
extern DItype __fixsfdi(SFtype a);
extern SFtype __floatdisf(DItype i);
extern SFtype __floatundisf(UDItype i);
extern USItype __fixunssfsi(SFtype a);
extern UDItype __fixunssfdi(SFtype a);

/* LIBGCC2_HAS_HF_MODE */
extern DItype __fixdfdi(DFtype a);
extern DFtype __floatdidf(DItype i);
extern DFtype __floatundidf(UDItype i);
extern USItype __fixunsdfsi(DFtype a);
extern UDItype __fixunsdfdi(DFtype a);

#endif  /* _LOCAL_LIBGCC_H_ */
