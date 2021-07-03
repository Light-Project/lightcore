#ifndef _SYSTEM_SPINLOCK_TYPE_H_
#define _SYSTEM_SPINLOCK_TYPE_H_

#ifndef __ASSEMBLY__

typedef struct raw_spinlock{
    volatile uint8_t rawlock;
} raw_spinlock_t;

typedef struct spinlock{
    raw_spinlock_t rlock;
} spinlock_t;

#endif /* __ASSEMBLY__ */
#endif