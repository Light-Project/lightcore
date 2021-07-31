#ifndef _KERNEL_SPINLOCK_UP_H_
#define _KERNEL_SPINLOCK_UP_H_

#define arch_spin_is_locked(lock)           ((void)(lock), 0)
/* for sched/core.c and kernel_lock.c: */
#define arch_spin_lock(lock)                do { barrier(); (void)(lock); } while (0)
#define arch_spin_lock_flags(lock, flags)   do { barrier(); (void)(lock); } while (0)
#define arch_spin_unlock(lock)              do { barrier(); (void)(lock); } while (0)
#define arch_spin_trylock(lock)	            ({ barrier(); (void)(lock); 1; })

#endif