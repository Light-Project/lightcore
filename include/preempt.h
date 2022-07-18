/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PREEMPT_H_
#define _PREEMPT_H_

#include <macro.h>
#include <bits.h>
#include <percpu.h>

DECLARE_PERCPU(unsigned long, preempt_counts);

enum preempt_level {
    PREEMPT_SOFTIRQ,
    PREEMPT_HARDIRQ,
    PREEMPT_NMI,
};

#define PREEMPT_BITS        8
#define SOFTIRQ_BITS        8
#define HARDIRQ_BITS        4
#define NMI_BITS            4

#define PREEMPT_SHIFT       0
#define SOFTIRQ_SHIFT       (PREEMPT_SHIFT + PREEMPT_BITS)
#define HARDIRQ_SHIFT       (SOFTIRQ_SHIFT + SOFTIRQ_BITS)
#define NMI_SHIFT           (HARDIRQ_SHIFT + HARDIRQ_BITS)

#define PREEMPT_OFFSET      BIT(PREEMPT_SHIFT)
#define SOFTIRQ_OFFSET      BIT(SOFTIRQ_SHIFT)
#define SOFTIRQ_DISABLE     BIT(SOFTIRQ_SHIFT + 1)
#define SOFTIRQ_LOCK        BIT(SOFTIRQ_SHIFT + 2)
#define HARDIRQ_OFFSET      BIT(HARDIRQ_SHIFT)
#define NMI_OFFSET          BIT(NMI_SHIFT)

#define PREEMPT_MASK        BIT_SHIFT(PREEMPT_SHIFT, BIT_LOW_MASK(PREEMPT_BITS))
#define SOFTIRQ_MASK        BIT_SHIFT(SOFTIRQ_SHIFT, BIT_LOW_MASK(SOFTIRQ_BITS))
#define HARDIRQ_MASK        BIT_SHIFT(HARDIRQ_SHIFT, BIT_LOW_MASK(HARDIRQ_BITS))
#define NMI_MASK            BIT_SHIFT(NMI_SHIFT, BIT_LOW_MASK(NMI_BITS))

#define softirq_count()     (preempt_count() & SOFTIRQ_MASK)
#define hardirq_count()     (preempt_count() & HARDIRQ_MASK)
#define nmi_count()         (preempt_count() & NMI_MASK)
#define irq_count()         (softirq_count() | hardirq_count() | nmi_count())

#define in_softirq()        (!!softirq_count())
#define in_hardirq()        (!!hardirq_count())
#define in_nmi()            (!!nmi_count())
#define in_irq()            (!!irq_count())

#ifndef preempt_count
static inline unsigned long preempt_count(void)
{
    return thiscpu(preempt_counts);
}
#endif

#ifndef preempt_count_ptr
static inline unsigned long *preempt_count_ptr(void)
{
    return thiscpu_ptr(&preempt_counts);
}
#endif

#ifndef raw_preempt_count_set
static inline void raw_preempt_count_set(unsigned long val)
{
    *preempt_count_ptr() = val;
}
#endif

#ifndef raw_preempt_count_add
static inline void raw_preempt_count_add(unsigned long val)
{
    *preempt_count_ptr() += val;
}
#endif

#ifndef raw_preempt_count_sub
static inline void raw_preempt_count_sub(unsigned long val)
{
    *preempt_count_ptr() -= val;
}
#endif

static inline enum preempt_level preempt_count_level(void)
{
    unsigned long count = preempt_count();
    enum preempt_level type = 0;

    type += VAL_BOOL(count & (SOFTIRQ_MASK));
    type += VAL_BOOL(count & (SOFTIRQ_MASK | HARDIRQ_MASK));
    type += VAL_BOOL(count & (SOFTIRQ_MASK | HARDIRQ_MASK | NMI_MASK));

    return type;
}

extern void preempt_count_add(unsigned long val);
extern void preempt_count_sub(unsigned long val);
#define preempt_count_inc() preempt_count_add(1)
#define preempt_count_dec() preempt_count_sub(1)

#ifdef CONFIG_PREEMPTION
#define preempt_disable() do {  \
    preempt_count_inc();        \
    barrier();                  \
} while (0)

#define preempt_enable() do {   \
    barrier();                  \
    preempt_count_dec();        \
} while (0)
#else
#define preempt_disable()
#define preempt_enable()
#endif  /* CONFIG_PREEMPTION */

#endif  /* _PREEMPT_H_ */
