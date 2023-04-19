/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_RAW_H_
#define _SPINLOCK_RAW_H_

#if defined(CONFIG_SMP) || defined(CONFIG_DEBUG_SPINLOCK)
# include <spinlock-locking.h>
#else
# include <spinlock-dummy.h>
#endif
#include <irqflags.h>
#include <proc.h>

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
# define raw_spin_check_lock(lock)                  _raw_spin_check_lock(lock)

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

#else /* !CONFIG_INLINE_SPINLOCK */

extern void raw_spin_lock                           (raw_spinlock_t *lock);
extern void raw_spin_unlock                         (raw_spinlock_t *lock);
extern void raw_spin_lock_bh                        (raw_spinlock_t *lock);
extern void raw_spin_unlock_bh                      (raw_spinlock_t *lock);
extern void raw_spin_lock_irq                       (raw_spinlock_t *lock);
extern void raw_spin_unlock_irq                     (raw_spinlock_t *lock);
extern void raw_spin_lock_irqsave                   (raw_spinlock_t *lock, irqflags_t *flags);
extern void raw_spin_unlock_irqrestore              (raw_spinlock_t *lock, irqflags_t *flags);
extern bool raw_spin_trylock                        (raw_spinlock_t *lock);
extern bool raw_spin_trylock_bh                     (raw_spinlock_t *lock);
extern bool raw_spin_locked                         (raw_spinlock_t *lock);
extern bool raw_spin_contended                      (raw_spinlock_t *lock);

extern void raw_read_lock                           (raw_rwlock_t *lock);
extern void raw_read_unlock                         (raw_rwlock_t *lock);
extern void raw_read_lock_bh                        (raw_rwlock_t *lock);
extern void raw_read_unlock_bh                      (raw_rwlock_t *lock);
extern void raw_read_lock_irq                       (raw_rwlock_t *lock);
extern void raw_read_unlock_irq                     (raw_rwlock_t *lock);
extern void raw_read_lock_irqsave                   (raw_rwlock_t *lock, irqflags_t *flags);
extern void raw_read_unlock_irqrestore              (raw_rwlock_t *lock, irqflags_t *flags);
extern bool raw_read_trylock                        (raw_rwlock_t *lock);
extern bool raw_read_trylock_bh                     (raw_rwlock_t *lock);

extern void raw_write_lock                          (raw_rwlock_t *lock);
extern void raw_write_unlock                        (raw_rwlock_t *lock);
extern void raw_write_lock_bh                       (raw_rwlock_t *lock);
extern void raw_write_unlock_bh                     (raw_rwlock_t *lock);
extern void raw_write_lock_irq                      (raw_rwlock_t *lock);
extern void raw_write_unlock_irq                    (raw_rwlock_t *lock);
extern void raw_write_lock_irqsave                  (raw_rwlock_t *lock, irqflags_t *flags);
extern void raw_write_unlock_irqrestore             (raw_rwlock_t *lock, irqflags_t *flags);
extern bool raw_write_trylock                       (raw_rwlock_t *lock);
extern bool raw_write_trylock_bh                    (raw_rwlock_t *lock);

#endif /* CONFIG_INLINE_SPINLOCK */

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

#define raw_read_trylock_irq(lock) ({                   \
    irq_local_disable();                                \
    raw_read_trylock(lock) ?                            \
    1 : ({ irq_local_enable(); 0; });                   \
})

#define raw_read_trylock_irqsave(lock, irqflags) ({     \
    *irqflags = irq_local_save();                       \
    raw_read_trylock(lock) ?                            \
    1 : ({ irq_local_restore(*irqflags); 0; });         \
})

#define raw_write_trylock_irq(lock) ({                  \
    irq_local_disable();                                \
    raw_write_trylock(lock) ?                           \
    1 : ({ irq_local_enable(); 0; });                   \
})

#define raw_write_trylock_irqsave(lock, irqflags) ({    \
    *irqflags = irq_local_save();                       \
    raw_write_trylock(lock) ?                           \
    1 : ({ irq_local_restore(*irqflags); 0; });         \
})

#endif /* _SPINLOCK_RAW_H_ */
