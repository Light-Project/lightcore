#include <system/spinlock.h>
#include <asm/irq.h>

void raw_spin_lock(raw_spinlock_t *lock)
{
    
}

void raw_spin_unlock(raw_spinlock_t *lock)
{

}

void raw_spin_lock_bh(raw_spinlock_t *lock)
{

}

void raw_spin_unlock_bh(raw_spinlock_t *lock)
{

}

void raw_spin_lock_irq(raw_spinlock_t *lock)
{
    arch_irq_disable();

}

void raw_spin_unlock_irq(raw_spinlock_t *lock)
{

    arch_irq_enable();
}

void raw_spin_lock_irqsave(raw_spinlock_t *lock, irqflags_t *flags)
{

    *flags = arch_irq_save();
    
}

void raw_spin_unlock_irqrestore(raw_spinlock_t *lock, irqflags_t *flags)
{

    arch_irq_restore(*flags);
}

bool raw_spin_trylock(raw_spinlock_t *lock)
{

    return true;
}

bool raw_spin_trylock_bh(raw_spinlock_t *lock)
{


    return true;
}
bool raw_spin_trylock_irq(raw_spinlock_t *lock)
{

    return true;
}

bool raw_spin_trylock_irqsave(raw_spinlock_t *lock, irqflags_t *flags)
{

    return true;
}
