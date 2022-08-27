/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KGCOV_H_
#define _KGCOV_H_

#include <types.h>
#include <stddef.h>
#include <mutex.h>

#if BITS_PER_LONG >= 64
typedef long kgcov_t;
#else
typedef long long kgcov_t;
#endif

extern struct mutex kgcov_lock;
extern bool kgcov_event_enable;

#ifdef CONFIG_CC_IS_GCC
# include <kgcov-gcc.h>
#endif /* CONFIG_CC_IS_GCC */

#endif /* _KGCOV_H_ */
