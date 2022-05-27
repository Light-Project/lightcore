/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <types.h>
#include <stddef.h>
#include <proc.h>
#include <irqflags.h>
#include <bottom-half.h>
#include <asm/mmiowb.h>
#include <asm/barrier.h>

#if defined(CONFIG_SMP)
# include <asm/spinlock.h>
#else /* !CONFIG_SMP */

typedef struct {
    volatile uint8_t rawlock;
} arch_spinlock_t;

#endif /* CONFIG_SMP */

typedef struct spinlock {
    arch_spinlock_t rlock;
} spinlock_t;

#define SPIN_LOCK_STATIC(name) \
    {{}}

#define SPIN_LOCK_UNLOCK(name) \
    (spinlock_t) SPIN_LOCK_STATIC(name)

#define SPIN_LOCK(name) \
    spinlock_t name = SPIN_LOCK_UNLOCK(name)

static inline void spin_lock_init(spinlock_t *spinlock)
{
    *spinlock = (spinlock_t){};
}

#if defined(CONFIG_SMP)
# include <asm/spinlock.h>
#elif defined(CONFIG_DEBUG_SPINLOCK)

static inline void arch_spin_lock(arch_spinlock_t *lock)
{
    while (lock->rawlock)
        cpu_relax();
    lock->rawlock++;
    barrier();
}

static inline void arch_spin_unlock(arch_spinlock_t *lock)
{
    barrier();
    lock->rawlock--;
}

#else /* !CONFIG_DEBUG_SPINLOCK */

# define arch_spin_lock(lock)               do { barrier(); (void)(lock); } while (0)
# define arch_spin_unlock(lock)             do { barrier(); (void)(lock); } while (0)
# define arch_spin_trylock(lock)            ({ barrier(); (void)(lock); 1; })

#endif  /* CONFIG_SMP */

#if defined(CONFIG_SMP)

#else /* !CONFIG_SMP */

#define _raw_generic_lock(lock)                         \
do {                                                    \
    arch_spin_lock(lock);                               \
} while (0)

#define _raw_generic_unlock(lock)                       \
do {                                                    \
    arch_spin_unlock(lock);                             \
} while (0)

#define _raw_generic_lock_bh(lock)                      \
do {                                                    \
    bh_local_lock();                                    \
    _raw_generic_lock(lock);                            \
} while (0)

#define _raw_generic_unlock_bh(lock)                    \
do {                                                    \
    _raw_generic_unlock(lock);                          \
    bh_local_unlock();                                  \
} while (0)

#define _raw_generic_lock_irq(lock)                     \
do {                                                    \
    irq_local_disable();                                \
    _raw_generic_lock(lock);                            \
} while (0)

#define _raw_generic_unlock_irq(lock)                   \
do {                                                    \
    _raw_generic_unlock(lock);                          \
    irq_local_enable();                                 \
} while (0)

#define _raw_generic_lock_irqsave(lock, flags)          \
do {                                                    \
    *flags = irq_local_save();                          \
    _raw_generic_lock(lock);                            \
} while (0)

#define _raw_generic_unlock_irqrestore(lock, flags)     \
do {                                                    \
    _raw_generic_unlock(lock);                          \
    irq_local_restore(*flags);                          \
} while (0)

#define _raw_spin_lock(lock)                        _raw_generic_lock(lock)
#define _raw_spin_unlock(lock)                      _raw_generic_unlock(lock)
#define _raw_spin_lock_bh(lock)                     _raw_generic_lock_bh(lock)
#define _raw_spin_unlock_bh(lock)                   _raw_generic_unlock_bh(lock)
#define _raw_spin_lock_irq(lock)                    _raw_generic_lock_irq(lock)
#define _raw_spin_unlock_irq(lock)                  _raw_generic_unlock_irq(lock)
#define _raw_spin_lock_irqsave(lock, flags)         _raw_generic_lock_irqsave(lock, flags)
#define _raw_spin_unlock_irqrestore(lock, flags)    _raw_generic_unlock_irqrestore(lock, flags)
#define _raw_spin_trylock(lock)                     ({_raw_generic_lock(lock); true;})
#define _raw_spin_trylock_bh(lock)                  ({_raw_generic_lock_bh(lock); true;})

#define _raw_read_lock(lock)                        _raw_generic_lock(lock)
#define _raw_read_unlock(lock)                      _raw_generic_unlock(lock)
#define _raw_read_lock_bh(lock)                     _raw_generic_lock_bh(lock)
#define _raw_read_unlock_bh(lock)                   _raw_generic_unlock_bh(lock)
#define _raw_read_lock_irq(lock)                    _raw_generic_lock_irq(lock)
#define _raw_read_unlock_irq(lock)                  _raw_generic_unlock_irq(lock)
#define _raw_read_lock_irqsave(lock, flags)         _raw_generic_lock_irqsave(lock, flags)
#define _raw_read_unlock_irqrestore(lock, flags)    _raw_generic_unlock_irqrestore(lock, flags)
#define _raw_read_trylock(lock)                     ({_raw_generic_lock(lock); true;})
#define _raw_read_trylock_bh(lock)                  ({_raw_generic_lock_bh(lock); true;})

#define _raw_write_lock(lock)                       _raw_generic_lock(lock)
#define _raw_write_unlock(lock)                     _raw_generic_unlock(lock)
#define _raw_write_lock_bh(lock)                    _raw_generic_lock_bh(lock)
#define _raw_write_unlock_bh(lock)                  _raw_generic_unlock_bh(lock)
#define _raw_write_lock_irq(lock)                   _raw_generic_lock_irq(lock)
#define _raw_write_unlock_irq(lock)                 _raw_generic_unlock_irq(lock)
#define _raw_write_lock_irqsave(lock, flags)        _raw_generic_lock_irqsave(lock, flags)
#define _raw_write_unlock_irqrestore(lock, flags)   _raw_generic_unlock_irqrestore(lock, flags)
#define _raw_write_trylock(lock)                    ({_raw_generic_lock(lock), true})
#define _raw_write_trylock_bh(lock)                 ({_raw_generic_lock_bh(lock), true})

#endif  /* CONFIG_SMP */

#ifdef CONFIG_INLINE_SPINLOCK

# define raw_spin_lock(lock)                        _raw_spin_lock(lock)
# define raw_spin_unlock(lock)                      _raw_spin_unlock(lock)
# define raw_spin_lock_bh(lock)                     _raw_spin_lock_bh(lock)
# define raw_spin_unlock_bh(lock)                   _raw_spin_unlock_bh(lock)
# define raw_spin_lock_irq(lock)                    _raw_spin_lock_irq(lock)
# define raw_spin_unlock_irq(lock)                  _raw_spin_unlock_irq(lock)
# define raw_spin_lock_irqsave(lock, flags)         _raw_spin_lock_irqsave(lock, flags)
# define raw_spin_unlock_irqrestore(lock, flags)    _raw_spin_unlock_irqrestore(lock, flags)
# define raw_spin_trylock(lock)                     _raw_spin_trylock(lock)
# define raw_spin_trylock_bh(lock)                  _raw_spin_trylock_bh(lock)

# define raw_read_lock(lock)                        _raw_read_lock(lock)
# define raw_read_unlock(lock)                      _raw_read_unlock(lock)
# define raw_read_lock_bh(lock)                     _raw_read_lock_bh(lock)
# define raw_read_unlock_bh(lock)                   _raw_read_unlock_bh(lock)
# define raw_read_lock_irq(lock)                    _raw_read_lock_irq(lock)
# define raw_read_unlock_irq(lock)                  _raw_read_unlock_irq(lock)
# define raw_read_lock_irqsave(lock, flags)         _raw_read_lock_irqsave(lock, flags)
# define raw_read_unlock_irqrestore(lock, flags)    _raw_read_unlock_irqrestore(lock, flags)
# define raw_read_trylock(lock)                     _raw_read_trylock(lock)

# define raw_write_lock(lock)                       _raw_write_lock(lock)
# define raw_write_unlock(lock)                     _raw_write_unlock(lock)
# define raw_write_lock_bh(lock)                    _raw_write_lock_bh(lock)
# define raw_write_unlock_bh(lock)                  _raw_write_unlock_bh(lock)
# define raw_write_lock_irq(lock)                   _raw_write_lock_irq(lock)
# define raw_write_unlock_irq(lock)                 _raw_write_unlock_irq(lock)
# define raw_write_lock_irqsave(lock, flags)        _raw_write_lock_irqsave(lock, flags)
# define raw_write_unlock_irqrestore(lock, flags)   _raw_write_unlock_irqrestore(lock, flags)
# define raw_write_trylock(lock)                    _raw_write_trylock(lock)

#else   /* !CONFIG_INLINE_SPINLOCK */

extern void raw_spin_lock                           (arch_spinlock_t *lock);
extern void raw_spin_unlock                         (arch_spinlock_t *lock);
extern void raw_spin_lock_bh                        (arch_spinlock_t *lock);
extern void raw_spin_unlock_bh                      (arch_spinlock_t *lock);
extern void raw_spin_lock_irq                       (arch_spinlock_t *lock);
extern void raw_spin_unlock_irq                     (arch_spinlock_t *lock);
extern void raw_spin_lock_irqsave                   (arch_spinlock_t *lock, irqflags_t *flags);
extern void raw_spin_unlock_irqrestore              (arch_spinlock_t *lock, irqflags_t *flags);
extern bool raw_spin_trylock                        (arch_spinlock_t *lock);
extern bool raw_spin_trylock_bh                     (arch_spinlock_t *lock);

extern void raw_read_lock                           (arch_spinlock_t *lock);
extern void raw_read_unlock                         (arch_spinlock_t *lock);
extern void raw_read_lock_bh                        (arch_spinlock_t *lock);
extern void raw_read_unlock_bh                      (arch_spinlock_t *lock);
extern void raw_read_lock_irq                       (arch_spinlock_t *lock);
extern void raw_read_unlock_irq                     (arch_spinlock_t *lock);
extern void raw_read_lock_irqsave                   (arch_spinlock_t *lock, irqflags_t *flags);
extern void raw_read_unlock_irqrestore              (arch_spinlock_t *lock, irqflags_t *flags);
extern bool raw_read_trylock                        (arch_spinlock_t *lock);
extern bool raw_read_trylock_bh                     (arch_spinlock_t *lock);

extern void raw_write_lock                          (arch_spinlock_t *lock);
extern void raw_write_unlock                        (arch_spinlock_t *lock);
extern void raw_write_lock_bh                       (arch_spinlock_t *lock);
extern void raw_write_unlock_bh                     (arch_spinlock_t *lock);
extern void raw_write_lock_irq                      (arch_spinlock_t *lock);
extern void raw_write_unlock_irq                    (arch_spinlock_t *lock);
extern void raw_write_lock_irqsave                  (arch_spinlock_t *lock, irqflags_t *flags);
extern void raw_write_unlock_irqrestore             (arch_spinlock_t *lock, irqflags_t *flags);
extern bool raw_write_trylock                       (arch_spinlock_t *lock);
extern bool raw_write_trylock_bh                    (arch_spinlock_t *lock);

#endif  /* CONFIG_INLINE_SPINLOCK */

#define raw_spin_trylock_irq(lock) ({                   \
	irq_local_disable();                                \
	raw_spin_trylock(lock) ?                            \
	1 : ({ irq_local_enable(); 0; });                   \
})

#define raw_spin_trylock_irqsave(lock, irqflags) ({     \
    *irqflags = irq_local_save();                       \
	raw_spin_trylock(lock) ?                            \
	1 : ({ irq_local_restore(*irqflags); 0; });         \
})

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
