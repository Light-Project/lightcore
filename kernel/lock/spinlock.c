/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <spinlock.h>
#include <export.h>

#ifndef CONFIG_INLINE_SPINLOCK

void raw_spin_lock(arch_spinlock_t *lock)
{
    _raw_spin_lock(lock);
}
EXPORT_SYMBOL(raw_spin_lock);

void raw_spin_unlock(arch_spinlock_t *lock)
{
    _raw_spin_unlock(lock);
}
EXPORT_SYMBOL(raw_spin_unlock);

void raw_spin_lock_bh(arch_spinlock_t *lock)
{
    _raw_spin_lock_bh(lock);
}
EXPORT_SYMBOL(raw_spin_lock_bh);

void raw_spin_unlock_bh(arch_spinlock_t *lock)
{
    _raw_spin_unlock_bh(lock);
}
EXPORT_SYMBOL(raw_spin_unlock_bh);

void raw_spin_lock_irq(arch_spinlock_t *lock)
{
    _raw_spin_lock_irq(lock);
}
EXPORT_SYMBOL(raw_spin_lock_irq);

void raw_spin_unlock_irq(arch_spinlock_t *lock)
{
    _raw_spin_unlock_irq(lock);
}
EXPORT_SYMBOL(raw_spin_unlock_irq);

void raw_spin_lock_irqsave(arch_spinlock_t *lock, irqflags_t *flags)
{
    _raw_spin_lock_irqsave(lock, flags);
}
EXPORT_SYMBOL(raw_spin_lock_irqsave);

void raw_spin_unlock_irqrestore(arch_spinlock_t *lock, irqflags_t *flags)
{
    _raw_spin_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(raw_spin_unlock_irqrestore);

bool raw_spin_trylock(arch_spinlock_t *lock)
{
    return _raw_spin_trylock(lock);
}
EXPORT_SYMBOL(raw_spin_trylock);

bool raw_spin_trylock_bh(arch_spinlock_t *lock)
{
    return _raw_spin_trylock_bh(lock);
}
EXPORT_SYMBOL(raw_spin_trylock_bh);

#endif  /* CONFIG_INLINE_SPINLOCK */
