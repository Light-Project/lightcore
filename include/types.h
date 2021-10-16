/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TYPES_H_
#define _TYPES_H_

#include <lightcore/types.h>

#ifndef __ASSEMBLY__

typedef unsigned char   uchar_t;
typedef unsigned short  ushort_t;
typedef unsigned int    uint_t;
typedef unsigned long   ulong_t;
typedef unsigned char   bool;

/* linux */
typedef int8_t          s8;
typedef uint8_t         u8;
typedef int16_t	        s16;
typedef uint16_t        u16;
typedef int32_t         s32;
typedef uint32_t        u32;
typedef int64_t         s64;
typedef uint64_t        u64;

/* bsd */
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

/* sysv */
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

/* Limits of integral types. */
typedef int64_t         intmax_t;
typedef uint64_t        uintmax_t;

typedef ssize_t         ptrdiff_t;
typedef ssize_t         intptr_t;
typedef size_t          uintptr_t;

typedef size_t          phys_addr_t;
typedef size_t          dma_addr_t;
typedef size_t          resource_size_t;
typedef uint32_t        gfp_t;


typedef short           mode_t;
typedef unsigned short  umode_t;
typedef unsigned int    pid_t;
typedef uint32_t        uid_t;
typedef uint32_t        gid_t;
typedef uint64_t        time_t;

typedef long long       loff_t;
typedef uint64_t        sector_t;
typedef uint64_t        blkcnt_t;

#endif /* __ASSEMBLY__ */
#endif /* _TYPES_H_ */
