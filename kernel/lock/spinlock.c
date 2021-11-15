/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <spinlock.h>
#include <export.h>
#include <asm/irq.h>

void raw_spin_lock(raw_spinlock_t *lock)
{

}
EXPORT_SYMBOL(raw_spin_lock);

void raw_spin_unlock(raw_spinlock_t *lock)
{

}
EXPORT_SYMBOL(raw_spin_unlock);

void raw_spin_lock_bh(raw_spinlock_t *lock)
{

}
EXPORT_SYMBOL(raw_spin_lock_bh);

void raw_spin_unlock_bh(raw_spinlock_t *lock)
{

}
EXPORT_SYMBOL(raw_spin_unlock_bh);

void raw_spin_lock_irq(raw_spinlock_t *lock)
{
    cpu_irq_disable();
}
EXPORT_SYMBOL(raw_spin_lock_irq);

void raw_spin_unlock_irq(raw_spinlock_t *lock)
{
    cpu_irq_enable();
}
EXPORT_SYMBOL(raw_spin_unlock_irq);

void raw_spin_lock_irqsave(raw_spinlock_t *lock, irqflags_t *flags)
{
    *flags = cpu_irq_save();
}
EXPORT_SYMBOL(raw_spin_lock_irqsave);

void raw_spin_unlock_irqrestore(raw_spinlock_t *lock, irqflags_t *flags)
{
    cpu_irq_restore(*flags);
}
EXPORT_SYMBOL(raw_spin_unlock_irqrestore);

bool raw_spin_trylock(raw_spinlock_t *lock)
{
    return true;
}
EXPORT_SYMBOL(raw_spin_trylock);

bool raw_spin_trylock_bh(raw_spinlock_t *lock)
{
    return true;
}
EXPORT_SYMBOL(raw_spin_trylock_bh);

bool raw_spin_trylock_irq(raw_spinlock_t *lock)
{
    return true;
}
EXPORT_SYMBOL(raw_spin_trylock_irq);

bool raw_spin_trylock_irqsave(raw_spinlock_t *lock, irqflags_t *flags)
{
    return true;
}
EXPORT_SYMBOL(raw_spin_trylock_irqsave);
