/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_TYPE_H_
#define _SPINLOCK_TYPE_H_

typedef struct raw_spinlock{
    volatile uint8_t rawlock;
} raw_spinlock_t;

typedef struct spinlock{
    raw_spinlock_t rlock;
} spinlock_t;

#define SPIN_LOCK(name) \
    struct spinlock name

#endif  /* _KERNEL_SPINLOCK_TYPE_H_ */