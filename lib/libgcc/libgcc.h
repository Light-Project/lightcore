/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOCAL_LIBGCC_H_
#define _LOCAL_LIBGCC_H_

typedef          int QItype  __mode(QI);
typedef unsigned int UQItype __mode(QI);
typedef          int SItype  __mode(SI);
typedef unsigned int USItype __mode(SI);
typedef          int DItype  __mode(DI);
typedef unsigned int UDItype __mode(DI);
typedef          int WDtype  __mode(word);

#define Wtype   SItype
#define HWtype  SItype
#define DWtype  DItype
#define UWtype  USItype
#define UHWtype USItype
#define UDWtype UDItype

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

#endif  /* _LOCAL_LIBGCC_H_ */
