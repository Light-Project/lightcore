/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <spinlock-raw.h>
#include <spinlock-types.h>

static __always_inline void spin_lock(spinlock_t *lock)
{
    raw_spin_lock(&lock->rlock);
}

static __always_inline void spin_unlock(spinlock_t *lock)
{
    raw_spin_unlock(&lock->rlock);
}

static __always_inline void spin_lock_bh(spinlock_t *lock)
{
    raw_spin_lock_bh(&lock->rlock);
}

static __always_inline void spin_unlock_bh(spinlock_t *lock)
{
    raw_spin_unlock_bh(&lock->rlock);
}

static __always_inline void spin_lock_irq(spinlock_t *lock)
{
    raw_spin_lock_irq(&lock->rlock);
}

static __always_inline void spin_unlock_irq(spinlock_t *lock)
{
    raw_spin_unlock_irq(&lock->rlock);
}

static __always_inline void spin_lock_irqsave(spinlock_t *lock, irqflags_t *irqflags)
{
    raw_spin_lock_irqsave(&lock->rlock, irqflags);
}

static __always_inline void spin_unlock_irqrestore(spinlock_t *lock, irqflags_t *irqflags)
{
    raw_spin_unlock_irqrestore(&lock->rlock, irqflags);
}

static __always_inline int spin_trylock(spinlock_t *lock)
{
    return raw_spin_trylock(&lock->rlock);
}

static __always_inline int spin_trylock_bh(spinlock_t *lock)
{
    return raw_spin_trylock_bh(&lock->rlock);
}

static __always_inline int spin_trylock_irq(spinlock_t *lock)
{
    return raw_spin_trylock_irq(&lock->rlock);
}

static __always_inline int spin_trylock_irqsave(spinlock_t *lock, irqflags_t *irqflags)
{
    return raw_spin_trylock_irqsave(&lock->rlock, irqflags);
}

#endif	/* _SPINLOCK_H_ */
