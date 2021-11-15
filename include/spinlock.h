/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <types.h>
#include <stddef.h>
#include <spinlock_type.h>
#include <asm/mmiowb.h>
#include <asm/irq.h>

#define spin_lock_init(lock)

#ifdef CONFIG_SMP
#include <asm/spinlock.h>
#else
#define arch_spin_lock(lock)                do { barrier(); (void)(lock); } while (0)
#define arch_spin_lock_flags(lock, flags)   do { barrier(); (void)(lock); } while (0)
#define arch_spin_unlock(lock)              do { barrier(); (void)(lock); } while (0)
#define arch_spin_trylock(lock)	            ({ barrier(); (void)(lock); 1; })
#endif

extern void raw_spin_lock(raw_spinlock_t *lock);
extern void raw_spin_unlock(raw_spinlock_t *lock);
extern void raw_spin_lock_bh(raw_spinlock_t *lock);
extern void raw_spin_unlock_bh(raw_spinlock_t *lock);
extern void raw_spin_lock_irq(raw_spinlock_t *lock);
extern void raw_spin_unlock_irq(raw_spinlock_t *lock);
extern void raw_spin_lock_irqsave(raw_spinlock_t *lock, irqflags_t *flags);
extern void raw_spin_unlock_irqrestore(raw_spinlock_t *lock, irqflags_t *flags);
extern void raw_spin_lock_irqsave_nested(raw_spinlock_t *lock);

extern bool raw_spin_trylock(raw_spinlock_t *lock);
extern bool raw_spin_trylock_bh(raw_spinlock_t *lock);
extern bool raw_spin_trylock_irq(raw_spinlock_t *lock);
extern bool raw_spin_trylock_irqsave(raw_spinlock_t *lock, irqflags_t *flags);

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

#endif	/* _SPINLOCK_H_ */
