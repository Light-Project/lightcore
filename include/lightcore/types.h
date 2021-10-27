/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_TYPES_H_
#define _LIGHTCORE_TYPES_H_

#ifndef __ASSEMBLY__

#include <lightcore/asm/types.h>
#include <lightcore/posix.h>

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#define __bitwise __bitwise__

typedef __u16 __bitwise __le16;
typedef __u32 __bitwise __le32;
typedef __u64 __bitwise __le64;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __be64;

typedef __signed__ int  __state;

#endif /* __ASSEMBLY__ */
#endif /* _LIGHTCORE_TYPES_H_ */
