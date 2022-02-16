/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_DELAY_H_
#define _ASM_GENERIC_DELAY_H_

extern void proc_bad_ndelay(void);
extern void proc_bad_udelay(void);
extern void proc_bad_mdelay(void);

/* Designed by instruction set */
extern void arch_proc_do_loop(unsigned long loops);
extern void arch_proc_const_delay(unsigned long loops);
extern void arch_proc_ndelay(unsigned long nsecs);
extern void arch_proc_udelay(unsigned long usecs);

/* 5 is 2^32 / 1000000000 (rounded up) */
#define proc_ndelay(nsecs) ({                           \
    if (__builtin_constant_p(nsecs)) {                  \
        if ((nsecs) / 20000 >= 1)                       \
            proc_bad_ndelay();                          \
        else                                            \
            arch_proc_const_delay((nsecs) * 5UL);       \
    } else {                                            \
        arch_proc_ndelay(nsecs);                        \
    }                                                   \
})

/* 4295 is 2^32 / 1000000 (rounded up) */
#define proc_udelay(usecs) ({                           \
    if (__builtin_constant_p(usecs)) {                  \
        if ((usecs) / 20000 >= 1)                       \
            proc_bad_udelay();                          \
        else                                            \
            arch_proc_const_delay((usecs) * 4295UL);    \
    } else {                                            \
        arch_proc_udelay(usecs);                        \
    }                                                   \
})

/* 4294967 is 2^32 / 1000 (rounded up) */
#define proc_mdelay(msecs) ({                           \
    unsigned long ms = (msecs);                         \
    if (__builtin_constant_p(ms) && ms < 20) {          \
        proc_udelay(ms * 1000UL);                       \
    } else {                                            \
        while (ms--)                                    \
            proc_udelay(1000);                          \
    }                                                   \
})

#endif /* _ASM_GENERIC_DELAY_H_ */
