/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_CMPXCHG_H_
#define _ASM_GENERIC_CMPXCHG_H_

#include <asm/irq.h>

#define xchg(ptr, new) ({                                       \
    irqflags_t flags = cpu_irq_save();                          \
    typeof(*(ptr)) __new = (new);                               \
    typeof(*(ptr)) tmp;                                         \
    switch (sizeof(*(ptr))) {                                   \
        case 1:                                                 \
            tmp = *(uint8_t *)(ptr);                            \
            *(uint8_t *)(ptr) = (uint8_t)__new;                 \
            break;                                              \
        case 2:                                                 \
            tmp = *(uint16_t *)(ptr);                           \
            *(uint16_t *)(ptr) = (uint16_t)__new;               \
            break;                                              \
        case 4:                                                 \
            tmp = *(uint32_t *)(ptr);                           \
            *(uint32_t *)(ptr) = (uint32_t)__new;               \
            break;                                              \
        case 8: default:                                        \
            tmp = *(uint64_t *)(ptr);                           \
            *(uint64_t *)(ptr) = (uint64_t)__new;               \
            break;                                              \
    }                                                           \
    cpu_irq_restore(flags);                                     \
    tmp;                                                        \
})

#define cmpxchg(ptr, old, new) ({                               \
    irqflags_t flags = cpu_irq_save();                          \
    typeof(*(ptr)) __new = (new);                               \
    typeof(*(ptr)) __old = (old);                               \
    typeof(*(ptr)) tmp;                                         \
    switch (sizeof(*(ptr))) {                                   \
        case 1:                                                 \
            tmp = *(uint8_t *)(ptr);                            \
            if (tmp == __old)                                   \
                *(uint8_t *)(ptr) = (uint8_t)__new;             \
            break;                                              \
        case 2:                                                 \
            tmp = *(uint16_t *)(ptr);                           \
            if (tmp == __old)                                   \
                *(uint16_t *)(ptr) = (uint16_t)__new;           \
            break;                                              \
        case 4:                                                 \
            tmp = *(uint32_t *)(ptr);                           \
            if (tmp == __old)                                   \
                *(uint32_t *)(ptr) = (uint32_t)__new;           \
            break;                                              \
        case 8: default:                                        \
            tmp = *(uint64_t *)(ptr);                           \
            if (tmp == __old)                                   \
                *(uint64_t *)(ptr) = (uint64_t)__new;           \
            break;                                              \
    }                                                           \
    cpu_irq_restore(flags);                                     \
    tmp;                                                        \
})

#endif  /* _ASM_GENERIC_CMPXCHG_H_ */
