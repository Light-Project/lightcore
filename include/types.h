/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef __ASSEMBLY__

#include <lightcore/types.h>

typedef __s8                    s8;
typedef __s16                   s16;
typedef __s32                   s32;
typedef __s64                   s64;
typedef __u8                    u8;
typedef __u16                   u16;
typedef __u32                   u32;
typedef __u64                   u64;

typedef __le16                  le16;
typedef __le32                  le32;
typedef __le64                  le64;
typedef __be16                  be16;
typedef __be32                  be32;
typedef __be64                  be64;

typedef s8                      int8_t;
typedef s16                     int16_t;
typedef s32                     int32_t;
typedef s64                     int64_t;
typedef u8                      uint8_t;
typedef u16                     uint16_t;
typedef u32                     uint32_t;
typedef u64                     uint64_t;

#if defined(CONFIG_ARCH_HAS_INT128) && defined(__SIZEOF_INT128__)
#define TYPE_HAS_INT128
typedef __int128                s128;
typedef unsigned __int128       u128;
typedef s128                    int128_t;
typedef u128                    uint128_t;
#endif

/* C language standard type */
typedef _Bool                   bool;
typedef __kernel_ssize_t        ssize_t;
typedef __kernel_size_t         size_t;

typedef int64_t                 intmax_t;
typedef uint64_t                uintmax_t;
typedef unsigned long           uintptr_t;
typedef __kernel_ptrdiff_t      intptr_t;
typedef __kernel_ptrdiff_t      ptrdiff_t;

/* Kernel unique type */
typedef size_t                  phys_addr_t;
typedef size_t                  dma_addr_t;
typedef size_t                  resource_size_t;
typedef size_t                  irqflags_t;
typedef size_t                  irqnr_t;

typedef uint32_t                gfp_t;
typedef uint32_t                gvm_t;
typedef uint32_t                fd_t;

typedef int64_t                 time_t;
typedef int64_t                 ktime_t;
typedef int64_t                 ttime_t;
typedef uint64_t                sector_t;
typedef uint64_t                blkcnt_t;

typedef ssize_t                 atomic_t;
typedef unsigned short          umode_t;

/* Kernel user share type */
typedef uint32_t                dev_t;
typedef __state                 state;

typedef __kernel_ino_t          ino_t;
typedef __kernel_mode_t         mode_t;
typedef __kernel_pid_t          pid_t;
typedef __kernel_uid_t          uid_t;
typedef __kernel_gid_t          gid_t;
typedef __kernel_off_t          off_t;
typedef __kernel_loff_t         loff_t;
typedef __kernel_timer_t        timer_t;
typedef __kernel_clockid_t      clockid_t;
typedef __kernel_caddr_t        caddr_t;
typedef __kernel_clock_t        clock_t;

typedef __kernel_key_t          key_t;
typedef __kernel_mqd_t          mqd_t;

#define BYTES_PER_CHAR          sizeof(char)
#define BYTES_PER_SHORT         sizeof(short)
#define BYTES_PER_INT           sizeof(int)
#define BYTES_PER_LONG          sizeof(long)
#define BYTES_PER_LONG_LONG     sizeof(long long)

#define BYTES_PER_U8            sizeof(uint8_t)
#define BYTES_PER_U16           sizeof(uint16_t)
#define BYTES_PER_U32           sizeof(uint32_t)
#define BYTES_PER_U64           sizeof(uint64_t)
#define BYTES_PER_UINTMAX       sizeof(uintmax_t)
#define BYTES_PER_UINTPTR       sizeof(uintptr_t)

#define CHAR_WIDTH              __SCHAR_WIDTH__
#define SHRT_WIDTH              __SHRT_WIDTH__
#define INT_WIDTH               __INT_WIDTH__
#define LONG_WIDTH              __LONG_WIDTH__
#define LLONG_WIDTH             __LONG_LONG_WIDTH__
#define INTMAX_WIDTH            __LONG_LONG_WIDTH__

#endif  /* __ASSEMBLY__ */
#endif  /* _TYPES_H_ */
