/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DELAY_H_
#define _DELAY_H_

#include <math.h>
#include <static-call.h>
#include <asm/delay.h>

struct clocksource_device;

#ifndef MAX_UDELAY_MS
# define MAX_UDELAY_MS  5
#endif

#ifndef proc_ndelay
#define proc_ndelay proc_ndelay
static inline void ndelay(unsigned long nsecs)
{
    udelay(DIV_ROUND_UP(nsecs, 1000));
}
#endif

#ifndef proc_mdelay
#define proc_mdelay(msecs) (                            \
    (__builtin_constant_p(n) && (n) <= MAX_UDELAY_MS) ? \
            udelay((n) * 1000)                          \
        : ({                                            \
            unsigned long __ms = (n);                   \
            while (__ms--)                              \
                udelay(1000);                           \
        })                                              \
)
#endif

extern unsigned long loops_per_tick;

#ifndef CONFIG_HIRES_TIMER_DELAY
# define ndelay(nsec) proc_ndelay(nsec)
# define udelay(usec) proc_udelay(usec)
# define mdelay(msec) proc_mdelay(msec)
#else
extern void static_call_proc_ndelay(unsigned long nsec);
extern void static_call_proc_udelay(unsigned long usec);
extern void static_call_proc_mdelay(unsigned long msec);

extern void timer_ndelay(unsigned long nsec);
extern void timer_udelay(unsigned long usec);
extern void timer_mdelay(unsigned long msec);
extern void delay_change(struct clocksource_device *cdev);

DECLARE_STATIC_CALL(ndelay, static_call_proc_ndelay);
DECLARE_STATIC_CALL(udelay, static_call_proc_udelay);
DECLARE_STATIC_CALL(mdelay, static_call_proc_mdelay);

# define ndelay(nsec) static_call_cond(ndelay)(nsec)
# define udelay(usec) static_call_cond(udelay)(usec)
# define mdelay(msec) static_call_cond(mdelay)(msec)
#endif  /* CONFIG_HIRES_TIMER_DELAY */

extern void msleep(unsigned int msecs);

#endif  /* _DELAY_H_ */
