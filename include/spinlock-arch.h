/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_ARCH_H_
#define _SPINLOCK_ARCH_H_

#ifdef CONFIG_SMP
# include <asm/spinlock.h>
#else
# include <spinlock-up.h>
#endif
#include <spinlock-raw-types.h>

static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
    arch_spin_lock(&lock->alock);
}

static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
    arch_spin_unlock(&lock->alock);
}

static inline bool __raw_spin_trylock(raw_spinlock_t *lock)
{
    return arch_spin_trylock(&lock->alock);
}

static inline bool __raw_spin_locked(raw_spinlock_t *lock)
{
    return arch_spin_locked(&lock->alock);
}

static inline bool __raw_spin_contended(raw_spinlock_t *lock)
{
    return arch_spin_contended(&lock->alock);
}

static inline void __raw_read_lock(raw_rwlock_t *lock)
{
    arch_read_lock(&lock->alock);
}

static inline void __raw_read_unlock(raw_rwlock_t *lock)
{
    arch_read_unlock(&lock->alock);
}

static inline bool __raw_read_trylock(raw_rwlock_t *lock)
{
    return arch_read_trylock(&lock->alock);
}

static inline void __raw_write_lock(raw_rwlock_t *lock)
{
    arch_write_lock(&lock->alock);
}

static inline void __raw_write_unlock(raw_rwlock_t *lock)
{
    arch_write_unlock(&lock->alock);
}

static inline bool __raw_write_trylock(raw_rwlock_t *lock)
{
    return arch_write_trylock(&lock->alock);
}

#endif  /* _SPINLOCK_ARCH_H_ */
