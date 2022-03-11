/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef __ASSEMBLY__

#include <lightcore/types.h>

typedef __s8            s8;
typedef __s16           s16;
typedef __s32           s32;
typedef __s64           s64;
typedef __u8            u8;
typedef __u16           u16;
typedef __u32           u32;
typedef __u64           u64;

typedef __le16          le16;
typedef __le32          le32;
typedef __le64          le64;
typedef __be16          be16;
typedef __be32          be32;
typedef __be64          be64;

typedef s8              int8_t;
typedef s16             int16_t;
typedef s32             int32_t;
typedef s64             int64_t;
typedef u8              uint8_t;
typedef u16             uint16_t;
typedef u32             uint32_t;
typedef u64             uint64_t;

#if defined(CONFIG_ARCH_HAS_INT128) && defined(__SIZEOF_INT128__)
#define TYPE_HAS_INT128
typedef __int128            s128;
typedef unsigned __int128   u128;

typedef s128            int128_t;
typedef u128            uint128_t;
#endif

typedef __ssize_t       ssize_t;
typedef __size_t        size_t;
typedef _Bool           bool;

typedef __state         state;

/* Limits of integral types. */
typedef int64_t         intmax_t;
typedef uint64_t        uintmax_t;
typedef ssize_t         intptr_t;
typedef ssize_t         ptrdiff_t;
typedef size_t          uintptr_t;

typedef size_t          phys_addr_t;
typedef size_t          dma_addr_t;
typedef size_t          resource_size_t;

typedef size_t          irqflags_t;
typedef size_t          irqnr_t;

typedef ssize_t         atomic_t;
typedef uint64_t        sector_t;
typedef uint64_t        blkcnt_t;

typedef uint32_t        gfp_t;
typedef uint32_t        gvm_t;

typedef uint32_t        dev_t;
typedef uint16_t        umode_t;
typedef int64_t         time_t;
typedef int64_t         ktime_t;

typedef __key_t         key_t;
typedef __mqd_t         mqd_t;

typedef __mode_t        mode_t;
typedef __pid_t         pid_t;
typedef __uid_t         uid_t;
typedef __gid_t         gid_t;

typedef __off_t         off_t;
typedef __loff_t        loff_t;

#define BYTES_PER_SHORT     sizeof(short)
#define BYTES_PER_INT       sizeof(int)
#define BYTES_PER_LONG      sizeof(long)
#define BYTES_PER_LONG_LONG sizeof(long long)

#endif  /* __ASSEMBLY__ */
#endif  /* _TYPES_H_ */
