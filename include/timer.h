/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIMER_H_
#define _TIMER_H_

#include <hlist.h>
#include <ticktime.h>

typedef void (*timer_entry_t)(void *pdata);

enum timer_flags {
    __TIMER_IRQ_SAFE        = 0,
    __TIMER_PERIODIC        = 1,
};

#define TIMER_IRQ_SAFE      BIT(__TIMER_IRQ_SAFE)
#define TIMER_PERIODIC      BIT(__TIMER_PERIODIC)

/**
 * struct timer - describe a timer node.
 * @list: insert into the list node of the time wheel.
 * @delta: time increment of timer node.
 * @expires: expiration time of timer node.
 * @flags: characteristics flags of nodes.
 * @entry: entry function of timer interrupt.
 * @pdata: parameters of the entry function.
 */
struct timer {
    struct hlist_node list;
    ttime_t delta;
    ttime_t expires;
    unsigned int index;
    unsigned long flags;
    timer_entry_t entry;
    void *pdata;
};

#define TIMER_INITIALIZER(_entry, _pdata, _delta, _flags) \
    (struct timer) { .entry = (_entry), .pdata = (_pdata), .delta = (_delta), .flags = (_flags) }

#define DEFINE_TIMER(name, entry, pdata, delta, flags) \
    struct timer name = TIMER_INITIALIZER(entry, pdata, delta, flags)

#define TIMER_EXPIRE_DELTA_MAX ((1ULL << 62) - 1)

GENERIC_STRUCT_BITOPS(timer, struct timer, flags)
#define timer_flags_set(ptr, bit)   generic_timer_flags_set(ptr, bit)
#define timer_flags_clr(ptr, bit)   generic_timer_flags_clr(ptr, bit)
#define timer_flags_test(ptr, bit)  generic_timer_flags_test(ptr, bit)

#define timer_set_irq_safe(ptr)     timer_flags_set(ptr, __TIMER_IRQ_SAFE)
#define timer_clr_irq_safe(ptr)     timer_flags_clr(ptr, __TIMER_IRQ_SAFE)
#define timer_test_irq_safe(ptr)    timer_flags_test(ptr, __TIMER_IRQ_SAFE)

#define timer_set_periodic(ptr)     timer_flags_set(ptr, __TIMER_PERIODIC)
#define timer_clr_periodic(ptr)     timer_flags_clr(ptr, __TIMER_PERIODIC)
#define timer_test_periodic(ptr)    timer_flags_test(ptr, __TIMER_PERIODIC)

/**
 * timer_check_pending - Check whether the timer pending.
 * @timer: the timer in question.
 * @return: 1 if the timer is pending, 0 if not.
 */
static inline bool timer_check_pending(struct timer *timer)
{
    return !hlist_check_unhashed(&timer->list);
}

extern state timer_pending(struct timer *timer);
extern state timer_clear(struct timer *timer);
extern state timer_reduce(struct timer *timer, ttime_t delta);
extern state timer_modified(struct timer *timer, ttime_t delta);
extern void timer_update(void);

extern struct timer *timer_create(timer_entry_t entry, void *pdata, ttime_t delta, enum timer_flags flags);
extern void timer_destroy(struct timer *timer);
extern void __init timer_init(void);

#endif  /* _TIMER_H_ */
