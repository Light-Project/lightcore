/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_SPINLOCK_TYPES_H_
#define _ASM_GENERIC_SPINLOCK_TYPES_H_

#include <types.h>
#include <atomic.h>

typedef atomic_t arch_spinlock_t;
typedef atomic_t arch_rwlock_t;

#define ARCH_SPINLOCK_INIT 0
#define ARCH_RWLOCK_INIT 0

#endif  /* _ASM_GENERIC_SPINLOCK_TYPES_H_ */
