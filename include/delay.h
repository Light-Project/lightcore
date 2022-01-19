/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DELAY_H_
#define _DELAY_H_

#include <math.h>
#include <asm/delay.h>

#ifndef MAX_UDELAY_MS
# define MAX_UDELAY_MS  5
#endif

#ifndef ndelay
#define ndelay ndelay
static inline void ndelay(unsigned long nsecs)
{
    udelay(DIV_ROUND_UP(nsecs, 1000));
}
#endif

#ifndef mdelay
#define mdelay(msecs) (                         \
    (__builtin_constant_p(n) && (n) <= MAX_UDELAY_MS) ?
        udelay((n) * 1000) : ({
        unsigned long __ms = (n);
        while (__ms--) udelay(1000);
}))
#endif

extern unsigned long loops_per_tick;
extern void msleep(unsigned int msecs);

#endif  /* _DELAY_H_ */
