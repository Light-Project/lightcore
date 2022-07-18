/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_RAW_TYPES_H_
#define _SPINLOCK_RAW_TYPES_H_

#if defined(CONFIG_SMP)
# include <asm/spinlock-types.h>
#else
# include <spinlock-up-types.h>
#endif

typedef struct {
    arch_spinlock_t alock;
} raw_spinlock_t;

typedef struct {
    arch_rwlock_t alock;
} raw_rwlock_t;

#endif  /* _SPINLOCK_RAW_TYPES_H_ */
