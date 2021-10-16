/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_TYPES_H_
#define _LIGHTCORE_TYPES_H_

#include <lightcore/asm/bitsperlong.h>

#ifndef __ASSEMBLY__

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#define __bitwise __bitwise__

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;
typedef signed long         ssize_t;
typedef unsigned long       size_t;

typedef signed int          state;

typedef uint16_t __bitwise le16;
typedef uint32_t __bitwise le32;
typedef uint64_t __bitwise le64;
typedef uint16_t __bitwise be16;
typedef uint32_t __bitwise be32;
typedef uint64_t __bitwise be64;

#endif /* __ASSEMBLY__ */
#endif /* _LIGHTCORE_TYPES_H_ */
