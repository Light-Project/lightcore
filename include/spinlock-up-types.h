/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_DUMMY_TYPES_H_
#define _SPINLOCK_DUMMY_TYPES_H_

typedef struct {
#ifdef CONFIG_DEBUG_SPINLOCK
    volatile unsigned int cnt;
#endif
    /* Nothing */
} arch_spinlock_t;

typedef struct {
#ifdef CONFIG_DEBUG_SPINLOCK
    volatile unsigned int cnt;
#endif
    /* Nothing */
} arch_rwlock_t;

#define ARCH_SPIN_LOCK_STATIC   {}
#define ARCH_RW_LOCK_STATIC     {}
#define ARCH_SPIN_LOCK_UNLOCK   (arch_spinlock_t) ARCH_SPIN_LOCK_STATIC
#define ARCH_RW_LOCK_UNLOCK     (arch_rwlock_t) ARCH_RW_LOCK_STATIC

#endif  /* _SPINLOCK_DUMMY_TYPES_H_ */
