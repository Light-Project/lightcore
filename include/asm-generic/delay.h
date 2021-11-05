/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_DELAY_H_
#define _ASM_GENERIC_DELAY_H_

extern unsigned long tick_loops;

extern void bad_ndelay(void);
extern void bad_udelay(void);
extern void bad_mdelay(void);

extern void cpu_const_delay(unsigned long loops);
extern void cpu_ndelay(unsigned long nsecs);
extern void cpu_udelay(unsigned long usecs);

/* 5 is 2^32 / 1000000000 (rounded up) */
#define ndelay(nsecs) ({                            \
    if (__builtin_constant_p(nsecs)) {              \
        if ((nsecs) / 20000 >= 1)                   \
            bad_ndelay();                           \
        else                                        \
            cpu_const_delay((nsecs) * 5UL);         \
    } else {                                        \
        cpu_ndelay(nsecs);                          \
    }                                               \
})

/* 4295 is 2^32 / 1000000 (rounded up) */
#define udelay(usecs) ({                            \
    if (__builtin_constant_p(usecs)) {              \
        if ((usecs) / 20000 >= 1)                   \
            bad_udelay();                           \
        else                                        \
            cpu_const_delay((usecs) * 4295UL);      \
    } else {                                        \
        cpu_udelay(usecs);                          \
    }                                               \
})

/* 4294967 is 2^32 / 1000 (rounded up) */
#define mdelay(msecs) ({                            \
    unsigned long ms = (msecs);                     \
    if (__builtin_constant_p(ms) && ms < 20) {      \
        udelay(ms * 1000UL);                        \
    } else {                                        \
        while (ms--)                                \
            udelay(1000);                           \
    }                                               \
})

#endif /* _ASM_GENERIC_DELAY_H_ */
