/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <spinlock.h>
#include <export.h>

#ifndef CONFIG_INLINE_SPINLOCK

noinline __lockfunc void raw_spin_lock(raw_spinlock_t *lock)
{
    _raw_spin_lock(lock);
}
EXPORT_SYMBOL(raw_spin_lock);

noinline __lockfunc void raw_spin_unlock(raw_spinlock_t *lock)
{
    _raw_spin_unlock(lock);
}
EXPORT_SYMBOL(raw_spin_unlock);

noinline __lockfunc void raw_spin_lock_bh(raw_spinlock_t *lock)
{
    _raw_spin_lock_bh(lock);
}
EXPORT_SYMBOL(raw_spin_lock_bh);

noinline __lockfunc void raw_spin_unlock_bh(raw_spinlock_t *lock)
{
    _raw_spin_unlock_bh(lock);
}
EXPORT_SYMBOL(raw_spin_unlock_bh);

noinline __lockfunc void raw_spin_lock_irq(raw_spinlock_t *lock)
{
    _raw_spin_lock_irq(lock);
}
EXPORT_SYMBOL(raw_spin_lock_irq);

noinline __lockfunc void raw_spin_unlock_irq(raw_spinlock_t *lock)
{
    _raw_spin_unlock_irq(lock);
}
EXPORT_SYMBOL(raw_spin_unlock_irq);

noinline __lockfunc void raw_spin_lock_irqsave(raw_spinlock_t *lock, irqflags_t *flags)
{
    _raw_spin_lock_irqsave(lock, flags);
}
EXPORT_SYMBOL(raw_spin_lock_irqsave);

noinline __lockfunc void raw_spin_unlock_irqrestore(raw_spinlock_t *lock, irqflags_t *flags)
{
    _raw_spin_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(raw_spin_unlock_irqrestore);

noinline __lockfunc bool raw_spin_trylock(raw_spinlock_t *lock)
{
    return _raw_spin_trylock(lock);
}
EXPORT_SYMBOL(raw_spin_trylock);

noinline __lockfunc bool raw_spin_trylock_bh(raw_spinlock_t *lock)
{
    return _raw_spin_trylock_bh(lock);
}
EXPORT_SYMBOL(raw_spin_trylock_bh);

noinline __lockfunc bool raw_spin_locked(raw_spinlock_t *lock)
{
    return _raw_spin_locked(lock);
}
EXPORT_SYMBOL(raw_spin_locked);

noinline __lockfunc bool raw_spin_contended(raw_spinlock_t *lock)
{
    return _raw_spin_contended(lock);
}
EXPORT_SYMBOL(raw_spin_contended);

noinline __lockfunc void raw_read_lock(raw_rwlock_t *lock)
{
    _raw_read_lock(lock);
}
EXPORT_SYMBOL(raw_read_lock);

noinline __lockfunc void raw_read_unlock(raw_rwlock_t *lock)
{
    _raw_read_unlock(lock);
}
EXPORT_SYMBOL(raw_read_unlock);

noinline __lockfunc void raw_read_lock_bh(raw_rwlock_t *lock)
{
    _raw_read_lock_bh(lock);
}
EXPORT_SYMBOL(raw_read_lock_bh);

noinline __lockfunc void raw_read_unlock_bh(raw_rwlock_t *lock)
{
    _raw_read_unlock_bh(lock);
}
EXPORT_SYMBOL(raw_read_unlock_bh);

noinline __lockfunc void raw_read_lock_irq(raw_rwlock_t *lock)
{
    _raw_read_lock_irq(lock);
}
EXPORT_SYMBOL(raw_read_lock_irq);

noinline __lockfunc void raw_read_unlock_irq(raw_rwlock_t *lock)
{
    _raw_read_unlock_irq(lock);
}
EXPORT_SYMBOL(raw_read_unlock_irq);

noinline __lockfunc void raw_read_lock_irqsave(raw_rwlock_t *lock, irqflags_t *flags)
{
    _raw_read_lock_irqsave(lock, flags);
}
EXPORT_SYMBOL(raw_read_lock_irqsave);

noinline __lockfunc void raw_read_unlock_irqrestore(raw_rwlock_t *lock, irqflags_t *flags)
{
    _raw_read_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(raw_read_unlock_irqrestore);

noinline __lockfunc bool raw_read_trylock(raw_rwlock_t *lock)
{
    return _raw_read_trylock(lock);
}
EXPORT_SYMBOL(raw_read_trylock);

noinline __lockfunc bool raw_read_trylock_bh(raw_rwlock_t *lock)
{
    return _raw_read_trylock_bh(lock);
}
EXPORT_SYMBOL(raw_read_trylock_bh);

noinline __lockfunc void raw_write_lock(raw_rwlock_t *lock)
{
    _raw_write_lock(lock);
}
EXPORT_SYMBOL(raw_write_lock);

noinline __lockfunc void raw_write_unlock(raw_rwlock_t *lock)
{
    _raw_write_unlock(lock);
}
EXPORT_SYMBOL(raw_write_unlock);

noinline __lockfunc void raw_write_lock_bh(raw_rwlock_t *lock)
{
    _raw_write_lock_bh(lock);
}
EXPORT_SYMBOL(raw_write_lock_bh);

noinline __lockfunc void raw_write_unlock_bh(raw_rwlock_t *lock)
{
    _raw_write_unlock_bh(lock);
}
EXPORT_SYMBOL(raw_write_unlock_bh);

noinline __lockfunc void raw_write_lock_irq(raw_rwlock_t *lock)
{
    _raw_write_lock_irq(lock);
}
EXPORT_SYMBOL(raw_write_lock_irq);

noinline __lockfunc void raw_write_unlock_irq(raw_rwlock_t *lock)
{
    _raw_write_unlock_irq(lock);
}
EXPORT_SYMBOL(raw_write_unlock_irq);

noinline __lockfunc void raw_write_lock_irqsave(raw_rwlock_t *lock, irqflags_t *flags)
{
    _raw_write_lock_irqsave(lock, flags);
}
EXPORT_SYMBOL(raw_write_lock_irqsave);

noinline __lockfunc void raw_write_unlock_irqrestore(raw_rwlock_t *lock, irqflags_t *flags)
{
    _raw_write_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(raw_write_unlock_irqrestore);

noinline __lockfunc bool raw_write_trylock(raw_rwlock_t *lock)
{
    return _raw_write_trylock(lock);
}
EXPORT_SYMBOL(raw_write_trylock);

noinline __lockfunc bool raw_write_trylock_bh(raw_rwlock_t *lock)
{
    return _raw_write_trylock_bh(lock);
}
EXPORT_SYMBOL(raw_write_trylock_bh);

#endif /* CONFIG_INLINE_SPINLOCK */
