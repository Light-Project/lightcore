#ifndef _ASM_CSKY_SPINLOCK_H_
#define _ASM_CSKY_SPINLOCK_H_

#include <system/spinlock_types.h>
#include <asm/barrier.h>

/*
 * Ticket-based spin-locking.
 */
static inline void arch_spin_lock(arch_spinlock_t *lock)
{
    arch_spinlock_t lockval;
    u32 ticket_next = 1 << TICKET_NEXT;
    u32 *p = &lock->lock;
    u32 tmp;

    asm volatile (
        "1: ldex.w      %0, (%2) \n"
        "   mov         %1, %0	 \n"
        "   add         %0, %3	 \n"
        "   stex.w      %0, (%2) \n"
        "   bez         %0, 1b   \n"
        : "=&r" (tmp), "=&r" (lockval)
        : "r"(p), "r"(ticket_next)
        : "cc");

    while (lockval.tickets.next != lockval.tickets.owner)
        lockval.tickets.owner = READ_ONCE(lock->tickets.owner);

    smp_mb();
}

static inline int arch_spin_trylock(arch_spinlock_t *lock)
{
    u32 tmp, contended, res;
    u32 ticket_next = 1 << TICKET_NEXT;
    u32 *p = &lock->lock;

    do {
        asm volatile (
        "   ldex.w      %0, (%3)    \n"
        "   movi        %2, 1       \n"
        "   rotli       %1, %0, 16  \n"
        "   cmpne       %1, %0      \n"
        "   bt          1f          \n"
        "   movi        %2, 0       \n"
        "   add         %0, %0, %4  \n"
        "   stex.w      %0, (%3)    \n"
        "1:                         \n"
        : "=&r" (res), "=&r" (tmp), "=&r" (contended)
        : "r"(p), "r"(ticket_next)
        : "cc");
    } while (!res);

    if (!contended)
        smp_mb();

    return !contended;
}
