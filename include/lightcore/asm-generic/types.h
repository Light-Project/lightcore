/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_ASM_GENERIC_TYPES_H_
#define _LIGHTCORE_ASM_GENERIC_TYPES_H_

#ifndef __ASSEMBLY__

typedef __signed__ char __s8;
typedef __signed__ short __s16;
typedef __signed__ int __s32;

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;

#ifdef __GNUC__
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#else
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

#endif /* __ASSEMBLY__ */
#endif /* _LIGHTCORE_ASM_GENERIC_TYPES_H_ */
