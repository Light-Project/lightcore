/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_UP_H_
#define _SPINLOCK_UP_H_

#include <proc.h>
#include <spinlock-up-types.h>

#ifdef CONFIG_DEBUG_SPINLOCK

static inline bool arch_spin_is_lock(arch_spinlock_t *lock)
{
    return lock->cnt == 0;
}

static inline void arch_spin_lock(arch_spinlock_t *lock)
{
    lock->cnt = 0;
    barrier();
}

static inline void arch_spin_unlock(arch_spinlock_t *lock)
{
    barrier();
    lock->cnt = 1;
}

static inline bool arch_spin_trylock(arch_spinlock_t *lock)
{
    char ocnt = lock->cnt;

    lock->cnt = 0;
    barrier();

    return ocnt > 0;
}

#else

#define arch_spin_is_lock(lock)     ((void)(lock), 0)
#define arch_spin_lock(lock)        do { barrier(); (void)(lock); } while (0)
#define arch_spin_unlock(lock)      do { barrier(); (void)(lock); } while (0)
#define arch_spin_trylock(lock)     ({ barrier(); (void)(lock); 1; })

#endif  /* CONFIG_DEBUG_SPINLOCK */

#define arch_read_lock(lock)        do { barrier(); (void)(lock); } while (0)
#define arch_write_lock(lock)       do { barrier(); (void)(lock); } while (0)
#define arch_read_trylock(lock)     ({ barrier(); (void)(lock); 1; })
#define arch_write_trylock(lock)    ({ barrier(); (void)(lock); 1; })
#define arch_read_unlock(lock)      do { barrier(); (void)(lock); } while (0)
#define arch_write_unlock(lock)     do { barrier(); (void)(lock); } while (0)

#endif  /* _SPINLOCK_UP_H_ */
