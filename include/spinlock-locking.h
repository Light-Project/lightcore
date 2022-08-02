/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_LOCKING_H_
#define _SPINLOCK_LOCKING_H_

#include <irqflags.h>
#include <bottom-half.h>
#include <spinlock-arch.h>

static inline void _raw_spin_lock(raw_spinlock_t *lock)
{
    preempt_disable();
    __raw_spin_lock(lock);
}

static inline void _raw_spin_unlock(raw_spinlock_t *lock)
{
    __raw_spin_unlock(lock);
    preempt_enable();
}

static inline void _raw_spin_lock_bh(raw_spinlock_t *lock)
{
    bh_local_lock();
    __raw_spin_lock(lock);
}

static inline void _raw_spin_unlock_bh(raw_spinlock_t *lock)
{
    __raw_spin_unlock(lock);
    bh_local_unlock();
}

static inline void _raw_spin_lock_irq(raw_spinlock_t *lock)
{
    irq_local_disable();
    preempt_disable();
    __raw_spin_lock(lock);
}

static inline void _raw_spin_unlock_irq(raw_spinlock_t *lock)
{
    __raw_spin_unlock(lock);
    preempt_enable();
    irq_local_enable();
}

static inline void _raw_spin_lock_irqsave(raw_spinlock_t *lock, irqflags_t *flags)
{
    *flags = irq_local_save();
    preempt_disable();
    __raw_spin_lock(lock);
}

static inline void _raw_spin_unlock_irqrestore(raw_spinlock_t *lock, irqflags_t *flags)
{
    __raw_spin_unlock(lock);
    preempt_enable();
    irq_local_restore(*flags);
}

static inline bool _raw_spin_trylock(raw_spinlock_t *lock)
{
    preempt_disable();
    if (__raw_spin_trylock(lock))
        return true;
    preempt_enable();
    return false;
}

static inline bool _raw_spin_trylock_bh(raw_spinlock_t *lock)
{
    bh_local_disable();
    if (__raw_spin_trylock(lock))
        return true;
    bh_local_enable();
    return false;
}

static inline void _raw_read_lock(raw_rwlock_t *lock)
{
    preempt_disable();
    __raw_read_lock(lock);
}

static inline void _raw_read_unlock(raw_rwlock_t *lock)
{
    __raw_read_unlock(lock);
    preempt_enable();
}

static inline void _raw_read_lock_bh(raw_rwlock_t *lock)
{
    bh_local_disable();
    __raw_read_lock(lock);
}

static inline void _raw_read_unlock_bh(raw_rwlock_t *lock)
{
    __raw_read_unlock(lock);
    bh_local_enable();
}

static inline void _raw_read_lock_irq(raw_rwlock_t *lock)
{
    irq_local_disable();
    preempt_disable();
    __raw_read_lock(lock);
}

static inline void _raw_read_unlock_irq(raw_rwlock_t *lock)
{
    __raw_read_unlock(lock);
    preempt_enable();
    irq_local_disable();
}

static inline void _raw_read_lock_irqsave(raw_rwlock_t *lock, irqflags_t *flags)
{
    *flags = irq_local_save();
    preempt_disable();
    __raw_read_lock(lock);
}

static inline void _raw_read_unlock_irqrestore(raw_rwlock_t *lock, irqflags_t *flags)
{
    __raw_read_unlock(lock);
    preempt_enable();
    irq_local_restore(*flags);
}

static inline bool _raw_read_trylock(raw_rwlock_t *lock)
{
    preempt_disable();
    if (__raw_read_trylock(lock))
        return true;
    preempt_enable();
    return false;
}

static inline bool _raw_read_trylock_bh(raw_rwlock_t *lock)
{
    bh_local_disable();
    if (__raw_read_trylock(lock))
        return true;
    bh_local_enable();
    return false;
}

static inline void _raw_write_lock(raw_rwlock_t *lock)
{
    preempt_disable();
    __raw_write_lock(lock);
}

static inline void _raw_write_unlock(raw_rwlock_t *lock)
{
    __raw_write_unlock(lock);
    preempt_enable();
}

static inline void _raw_write_lock_bh(raw_rwlock_t *lock)
{
    bh_local_disable();
    __raw_write_lock(lock);
}

static inline void _raw_write_unlock_bh(raw_rwlock_t *lock)
{
    __raw_write_unlock(lock);
    bh_local_enable();
}

static inline void _raw_write_lock_irq(raw_rwlock_t *lock)
{
    irq_local_disable();
    preempt_disable();
    __raw_write_lock(lock);
}

static inline void _raw_write_unlock_irq(raw_rwlock_t *lock)
{
    __raw_write_unlock(lock);
    preempt_enable();
    irq_local_enable();
}

static inline void _raw_write_lock_irqsave(raw_rwlock_t *lock, irqflags_t *flags)
{
    *flags = irq_local_save();
    preempt_disable();
    __raw_write_lock(lock);
}

static inline void _raw_write_unlock_irqrestore(raw_rwlock_t *lock, irqflags_t *flags)
{
    __raw_write_unlock(lock);
    preempt_enable();
    irq_local_restore(*flags);
}

static inline bool _raw_write_trylock(raw_rwlock_t *lock)
{
    preempt_disable();
    if (__raw_write_trylock(lock))
        return true;
    preempt_enable();
    return false;
}

static inline bool _raw_write_trylock_bh(raw_rwlock_t *lock)
{
    bh_local_disable();
    if (__raw_write_trylock(lock))
        return true;
    bh_local_enable();
    return false;
}

#endif  /* _SPINLOCK_LOCKING_H_ */
