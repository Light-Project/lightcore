/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_SPINLOCK_H_
#define _ASM_GENERIC_SPINLOCK_H_

#include <asm-generic/spinlock-types.h>
#include <atomic.h>

static inline bool arch_spin_lock(arch_spinlock_t *lock)
{

}

static inline bool arch_spin_unlock(arch_spinlock_t *lock)
{

}

static inline bool arch_spin_trylock(arch_spinlock_t *lock)
{

}

#endif /* _ASM_GENERIC_SPINLOCK_H_ */
