/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_CMPXCHG_H_
#define _ASM_GENERIC_CMPXCHG_H_

#include <types.h>
#include <irqflags.h>

#ifdef CONFIG_SMP
# error "Cannot use generic cmpxchg on SMP"
#endif

static inline unsigned long
generic_xchg_function(volatile void *ptr, unsigned long new, unsigned int size)
{
    unsigned long retval;
    irqflags_t flags;

    switch (size) {
        case 1:
            flags = irq_local_save();
            retval = *(volatile uint8_t *)ptr;
            *(volatile uint8_t *)ptr = new;
            irq_local_restore(flags);
            break;

        case 2:
            flags = irq_local_save();
            retval = *(volatile uint16_t *)ptr;
            *(volatile uint16_t *)ptr = new;
            irq_local_restore(flags);
            break;

        case 4:
            flags = irq_local_save();
            retval = *(volatile uint32_t *)ptr;
            *(volatile uint32_t *)ptr = new;
            irq_local_restore(flags);
            break;

#ifdef CONFIG_ARCH_64BIT
        case 8:
            flags = irq_local_save();
            retval = *(volatile uint64_t *)ptr;
            *(volatile uint64_t *)ptr = new;
            irq_local_restore(flags);
            break;
#endif
    }

    return retval;
}

static inline unsigned long
generic_cmpxchg_function(volatile void *ptr, unsigned long old, unsigned long new, unsigned int size)
{
    unsigned long retval;
    irqflags_t flags;

    switch (size) {
        case 1:
            flags = irq_local_save();
            retval = *(volatile uint8_t *)ptr;
            if (retval == old)
                *(volatile uint8_t *)ptr = new;
            irq_local_restore(flags);
            break;

        case 2:
            flags = irq_local_save();
            retval = *(volatile uint16_t *)ptr;
            if (retval == old)
                *(volatile uint16_t *)ptr = new;
            irq_local_restore(flags);
            break;

        case 4:
            flags = irq_local_save();
            retval = *(volatile uint32_t *)ptr;
            if (retval == old)
                *(volatile uint32_t *)ptr = new;
            irq_local_restore(flags);
            break;

#ifdef CONFIG_ARCH_64BIT
        case 8:
            flags = irq_local_save();
            retval = *(volatile uint64_t *)ptr;
            if (retval == old)
                *(volatile uint64_t *)ptr = new;
            irq_local_restore(flags);
            break;
#endif
    }

    return retval;
}

#define generic_xchg(ptr, new) ({                       \
    ((typeof(*(ptr))) generic_xchg_function(            \
        (ptr), (unsigned long)(new), sizeof(*(ptr)))    \
    );                                                  \
})

#define generic_cmpxchg(ptr, old, new) ({               \
    ((typeof(*(ptr))) generic_cmpxchg_function(         \
        (ptr), (unsigned long)(old),                    \
        (unsigned long)(new), sizeof(*(ptr)))           \
    );                                                  \
})

#ifndef arch_xchg
# define arch_xchg(ptr, new) generic_xchg(ptr, new)
#endif

#ifndef arch_cmpxchg
# define arch_cmpxchg(ptr, old, new) generic_cmpxchg(ptr, old, new)
#endif

#endif  /* _ASM_GENERIC_CMPXCHG_H_ */
