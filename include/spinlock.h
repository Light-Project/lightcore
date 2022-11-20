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

static __always_inline bool spin_trylock(spinlock_t *lock)
{
    return raw_spin_trylock(&lock->rlock);
}

static __always_inline bool spin_trylock_bh(spinlock_t *lock)
{
    return raw_spin_trylock_bh(&lock->rlock);
}

static __always_inline bool spin_trylock_irq(spinlock_t *lock)
{
    return raw_spin_trylock_irq(&lock->rlock);
}

static __always_inline bool spin_trylock_irqsave(spinlock_t *lock, irqflags_t *irqflags)
{
    return raw_spin_trylock_irqsave(&lock->rlock, irqflags);
}

static __always_inline bool spin_locked(spinlock_t *lock)
{
    return raw_spin_locked(&lock->rlock);
}

static __always_inline bool spin_contended(spinlock_t *lock)
{
    return raw_spin_contended(&lock->rlock);
}

static __always_inline void read_lock(rwlock_t *lock)
{
    raw_read_lock(&lock->rlock);
}

static __always_inline void read_unlock(rwlock_t *lock)
{
    raw_read_unlock(&lock->rlock);
}

static __always_inline void read_lock_bh(rwlock_t *lock)
{
    raw_read_lock_bh(&lock->rlock);
}

static __always_inline void read_unlock_bh(rwlock_t *lock)
{
    raw_read_unlock_bh(&lock->rlock);
}

static __always_inline void read_lock_irq(rwlock_t *lock)
{
    raw_read_lock_irq(&lock->rlock);
}

static __always_inline void read_unlock_irq(rwlock_t *lock)
{
    raw_read_unlock_irq(&lock->rlock);
}

static __always_inline void read_lock_irqsave(rwlock_t *lock, irqflags_t *irqflags)
{
    raw_read_lock_irqsave(&lock->rlock, irqflags);
}

static __always_inline void read_unlock_irqrestore(rwlock_t *lock, irqflags_t *irqflags)
{
    raw_read_unlock_irqrestore(&lock->rlock, irqflags);
}

static __always_inline bool read_trylock(rwlock_t *lock)
{
    return raw_read_trylock(&lock->rlock);
}

static __always_inline bool read_trylock_bh(rwlock_t *lock)
{
    return raw_read_trylock_bh(&lock->rlock);
}

static __always_inline bool read_trylock_irq(rwlock_t *lock)
{
    return raw_read_trylock_irq(&lock->rlock);
}

static __always_inline bool read_trylock_irqsave(rwlock_t *lock, irqflags_t *irqflags)
{
    return raw_read_trylock_irqsave(&lock->rlock, irqflags);
}

static __always_inline void write_lock(rwlock_t *lock)
{
    raw_write_lock(&lock->rlock);
}

static __always_inline void write_unlock(rwlock_t *lock)
{
    raw_write_unlock(&lock->rlock);
}

static __always_inline void write_lock_bh(rwlock_t *lock)
{
    raw_write_lock_bh(&lock->rlock);
}

static __always_inline void write_unlock_bh(rwlock_t *lock)
{
    raw_write_unlock_bh(&lock->rlock);
}

static __always_inline void write_lock_irq(rwlock_t *lock)
{
    raw_write_lock_irq(&lock->rlock);
}

static __always_inline void write_unlock_irq(rwlock_t *lock)
{
    raw_write_unlock_irq(&lock->rlock);
}

static __always_inline void write_lock_irqsave(rwlock_t *lock, irqflags_t *irqflags)
{
    raw_write_lock_irqsave(&lock->rlock, irqflags);
}

static __always_inline void write_unlock_irqrestore(rwlock_t *lock, irqflags_t *irqflags)
{
    raw_write_unlock_irqrestore(&lock->rlock, irqflags);
}

static __always_inline bool write_trylock(rwlock_t *lock)
{
    return raw_write_trylock(&lock->rlock);
}

static __always_inline bool write_trylock_bh(rwlock_t *lock)
{
    return raw_write_trylock_bh(&lock->rlock);
}

static __always_inline bool write_trylock_irq(rwlock_t *lock)
{
    return raw_write_trylock_irq(&lock->rlock);
}

static __always_inline bool write_trylock_irqsave(rwlock_t *lock, irqflags_t *irqflags)
{
    return raw_write_trylock_irqsave(&lock->rlock, irqflags);
}

#endif  /* _SPINLOCK_H_ */
