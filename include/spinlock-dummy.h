/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_DUMMY_H_
#define _SPINLOCK_DUMMY_H_

#include <irqflags.h>
#include <bottom-half.h>
#include <spinlock-raw-types.h>

#define _raw_generic_lock(lock)                     \
do {                                                \
    preempt_disable();                              \
    (void)(lock);                                   \
} while (0)

#define _raw_generic_unlock(lock)                   \
do {                                                \
    preempt_enable();                               \
    (void)(lock);                                   \
} while (0)

#define _raw_generic_lock_bh(lock)                  \
do {                                                \
    bh_local_lock();                                \
    _raw_generic_lock(lock);                        \
} while (0)

#define _raw_generic_unlock_bh(lock)                \
do {                                                \
    _raw_generic_unlock(lock);                      \
    bh_local_unlock();                              \
} while (0)

#define _raw_generic_lock_irq(lock)                 \
do {                                                \
    irq_local_disable();                            \
    _raw_generic_lock(lock);                        \
} while (0)

#define _raw_generic_unlock_irq(lock)               \
do {                                                \
    _raw_generic_unlock(lock);                      \
    irq_local_enable();                             \
} while (0)

#define _raw_generic_lock_irqsave(lock, flags)      \
do {                                                \
    *flags = irq_local_save();                      \
    _raw_generic_lock(lock);                        \
} while (0)

#define _raw_generic_unlock_irqrestore(lock, flags) \
do {                                                \
    _raw_generic_unlock(lock);                      \
    irq_local_restore(*flags);                      \
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
#define _raw_write_trylock(lock)                    ({_raw_generic_lock(lock); true;})
#define _raw_write_trylock_bh(lock)                 ({_raw_generic_lock_bh(lock); true;})

#endif  /* _SPINLOCK_DUMMY_H_ */
