/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_ASM_GENERIC_POSIX_H_
#define _LIGHTCORE_ASM_GENERIC_POSIX_H_

#include <lightcore/asm/bitsperlong.h>

#ifndef __kernel_long_t
typedef signed long         __kernel_long_t;
typedef unsigned long       __kernel_ulong_t;
#endif

#ifndef __kernel_ino_t
typedef __kernel_ulong_t    __kernel_ino_t;
#endif

#ifndef __kernel_mode_t
typedef unsigned int        __kernel_mode_t;
typedef unsigned int        __kernel_umode_t;
#endif

#ifndef __kernel_pid_t
typedef signed int          __kernel_pid_t;
#endif

#ifndef __kernel_uid_t
typedef unsigned int        __kernel_uid_t;
typedef unsigned int        __kernel_gid_t;
#endif

#ifndef __kernel_off_t
typedef __kernel_long_t     __kernel_off_t;
typedef signed long long    __kernel_loff_t;
#endif

#ifndef __kernel_size_t
typedef __kernel_ulong_t    __kernel_size_t;
typedef __kernel_long_t	    __kernel_ssize_t;
typedef __kernel_long_t	    __kernel_ptrdiff_t;
#endif

typedef signed int          __kernel_timer_t;
typedef signed int          __kernel_clockid_t;
typedef signed char *       __kernel_caddr_t;
typedef __kernel_long_t     __kernel_clock_t;

#endif  /* _LIGHTCORE_ASM_GENERIC_POSIX_H_ */
