/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "timer"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <timer.h>
#include <cpu.h>
#include <kmalloc.h>
#include <softirq.h>
#include <ticktime.h>
#include <panic.h>
#include <export.h>

#define CLOCK_SHIFT         CONFIG_TIMER_SHIFT
#define CLOCK_DIV           (1UL << CLOCK_SHIFT)
#define CLOCK_MASK          (CLOCK_DIV - 1)

#define LEVEL_WIDTH         CONFIG_TIMER_LEVEL
#define LEVEL_SIZE          (1UL << LEVEL_WIDTH)
#define LEVEL_MASK          (LEVEL_SIZE - 1)

#define WHEEL_DEPTH         CONFIG_TIMER_DEPTH
#define WHEEL_LEN           (LEVEL_SIZE * WHEEL_DEPTH)

#define LEVEL_SHIFT(lvl)    ((lvl) * CLOCK_SHIFT)
#define LEVEL_GAIN(lvl)     (1UL << LEVEL_SHIFT(lvl))
#define LEVEL_OFFS(lvl)     (LEVEL_SIZE * (lvl))
#define LEVEL_START(lvl)    (LEVEL_SIZE << LEVEL_SHIFT((lvl) - 1))

#define LEVEL_TIMEOUT_CUT   (LEVEL_START(WHEEL_DEPTH))
#define LEVEL_TIMEOUT_MAX   (LEVEL_TIMEOUT_CUT - LEVEL_GAIN(WHEEL_DEPTH - 1))

struct timer_base {
    spinlock_t lock;
    unsigned int cpu;
    bool next_recalc;
    bool timers_pending;

    ttime_t current;
    ttime_t recent_timer;

    struct timer *running;
    DEFINE_BITMAP(pending_map, WHEEL_LEN);
    struct hlist_head wheels[WHEEL_LEN];
};

static void timer_softirq_handle(void *pdata);
static DEFINE_SOFTIRQ(timer_softirq, timer_softirq_handle, NULL, 0);
static DEFINE_PERCPU(struct timer_base, timer_bases);
static struct kcache *timer_cache;

static void base_enqueue(struct timer_base *base, struct timer *timer, unsigned int idx, ttime_t expiry)
{
    hlist_head_add(base->wheels + idx, &timer->list);
    bit_set(base->pending_map, idx);

    if (ttime_before(expiry, base->recent_timer)) {
        base->recent_timer = expiry;
        base->timers_pending = true;
        base->next_recalc = false;
    }
}

static void base_dequeue(struct timer *timer, bool clear)
{
    struct hlist_node *list = &timer->list;

    hlist_deluf(&timer->list);

    if (clear)
        list->pprev = NULL;
    list->next = POISON_HLIST2;
}

static bool base_try_dequeue(struct timer_base *base, struct timer *timer, bool clear)
{
    if (!timer_check_pending(timer))
        return false;

    base_dequeue(timer, clear);

    if (hlist_check_empty(base->wheels + timer->index)) {
        bit_clr(base->pending_map, timer->index);
        base->next_recalc = true;
    }

    return true;
}

static unsigned int clac_index(int depth, ttime_t expires, ttime_t *expiry)
{
    expires = (expires + LEVEL_GAIN(depth)) >> LEVEL_SHIFT(depth);
    *expiry = expires << LEVEL_SHIFT(depth);
    return LEVEL_OFFS(depth) + (expires & LEVEL_MASK);
}

static unsigned int clac_wheel(ttime_t current, ttime_t expires, ttime_t *expiry)
{
    ttime_t delta = expires - current;
    unsigned int count;

    if (expires < current) {
        *expiry = current;
        return current & LEVEL_MASK;
    }

    for (count = 0; count < WHEEL_DEPTH; ++count) {
        if (delta < LEVEL_START(count + 1))
            return clac_index(count, expires, expiry);
    }

    if (delta >= LEVEL_TIMEOUT_CUT)
        expires = current + LEVEL_TIMEOUT_MAX;

    return clac_index(WHEEL_DEPTH - 1, expires, expiry);
}

static void timer_insert(struct timer_base *base, struct timer *timer)
{
    unsigned int index;
    ttime_t expiry;

    timer->expires = ticktime + timer->delta;
    index = clac_wheel(base->current, timer->expires, &expiry);
    base_enqueue(base, timer, index, expiry);
}

static void timer_forward(struct timer_base *base)
{
    ttime_t ticknow = ticktime;

    if (ttime_before_equal(ticknow, base->current))
        return;

    if (ttime_before(ticknow, base->recent_timer))
        base->current = ticknow;
    else {
        if (WARN_ON(ttime_before(base->recent_timer, base->current)))
            return;
        base->current = base->recent_timer;
    }
}

static inline void forward_insert(struct timer_base *base, struct timer *timer)
{
    timer_forward(base);
    timer_insert(base, timer);
}

static void expire_timers(struct timer_base *base, struct hlist_head *head)
{
    while (!hlist_check_empty(head)) {
        struct timer *timer;
        void (*entry)(void *pdata);

        timer = hlist_first_entry(head, struct timer, list);
        base_dequeue(timer, true);

        if (timer_test_periodic(timer))
            forward_insert(base, timer);

        base->running = timer;
        entry = timer->entry;

        if (timer_test_irq_safe(timer)) {
            spin_unlock(&base->lock);
            entry(timer->pdata);
            spin_lock(&base->lock);
        } else {
            spin_unlock_irq(&base->lock);
            entry(timer->pdata);
            spin_lock_irq(&base->lock);
        }
    }
}

static unsigned int collect_timers(struct timer_base *base, struct hlist_head *heads)
{
    ttime_t curr = base->current = base->recent_timer;
    unsigned int count, index, levels = 0;
    struct hlist_head *vector;

    for (count = 0; count < WHEEL_DEPTH; ++count) {
        index = (curr & LEVEL_MASK) + count * LEVEL_SIZE;

        if (bit_test_clr(base->pending_map, index)) {
            vector = base->wheels + index;
            hlist_move_list(vector, heads++);
            levels++;
        }

        if (curr & CLOCK_MASK)
            break;

        curr >>= CLOCK_SHIFT;
    }

    return levels;
}

static __always_inline int
next_pending_bucket(struct timer_base *base, unsigned int index, ttime_t clk)
{
    unsigned int pos, start = index + clk;
    unsigned int end = index + LEVEL_SIZE;

    pos = find_next_bit(base->pending_map, end, start);
    if (pos < end)
        return pos - start;

    pos = find_next_bit(base->pending_map, start, index);
    return pos < start ? pos + LEVEL_SIZE - start : -1;
}

static ttime_t next_recent_timer(struct timer_base *base)
{
    unsigned int depth, index = 0;
    ttime_t curr, next;

    curr = base->current;
    next = curr + TIMER_EXPIRE_DELTA_MAX;

    for (depth = 0; depth < WHEEL_DEPTH; ++depth, index += LEVEL_SIZE) {
        int pos = next_pending_bucket(base, index, curr & LEVEL_MASK);
        int level_clk = curr & CLOCK_MASK;

        if (pos >= 0) {
            ttime_t tmp = curr + (unsigned long)pos;

            tmp <<= LEVEL_SHIFT(depth);
            if (ttime_before(tmp, next))
                next = tmp;

            if (pos <= ((CLOCK_DIV - level_clk) & CLOCK_MASK))
                break;
        }

        curr >>= CLOCK_SHIFT;
        curr += !!level_clk;
    }

    base->next_recalc = false;
    base->timers_pending = !(next == base->current + TIMER_EXPIRE_DELTA_MAX);

    return next;
}

static void base_do_timers(struct timer_base *base)
{
    struct hlist_head heads[WHEEL_DEPTH];

    if (ttime_before(ticktime, base->recent_timer))
        return;

    spin_lock_irq(&base->lock);
    while (ttime_after_equal(ticktime, base->current) &&
           ttime_after_equal(ticktime, base->recent_timer)) {
        unsigned int levels;

        levels = collect_timers(base, heads);
        WARN_ON(!levels && !base->next_recalc && base->timers_pending);

        base->current++;
        base->recent_timer = next_recent_timer(base);

        while (levels--)
            expire_timers(base, heads + levels);
    }
    spin_unlock_irq(&base->lock);
}

static void timer_softirq_handle(void *pdata)
{
    struct timer_base *base = thiscpu_ptr(&timer_bases);
    base_do_timers(base);
}

void timer_update(void)
{
    struct timer_base *base = thiscpu_ptr(&timer_bases);
    if (ttime_after_equal(ticktime, base->recent_timer))
        softirq_pending(&timer_softirq);
}

/**
 * timer_pending - pending a timer
 * @irq: the timer to pending
 */
state timer_pending(struct timer *timer)
{
    struct timer_base *base = thiscpu_ptr(&timer_bases);
    irqflags_t irqflags;

    if (!timer || timer_check_pending(timer))
        return -EINVAL;

    spin_lock_irqsave(&base->lock, &irqflags);
    forward_insert(base, timer);
    spin_unlock_irqrestore(&base->lock, &irqflags);

    return -ENOERR;
}
EXPORT_SYMBOL(timer_pending);

/**
 * timer_clear - clear a timer
 * @irq: the timer to clear
 */
state timer_clear(struct timer *timer)
{
    struct timer_base *base = thiscpu_ptr(&timer_bases);
    irqflags_t irqflags;
    bool success;

    if (!timer || !timer_check_pending(timer))
        return -EINVAL;

    spin_lock_irqsave(&base->lock, &irqflags);
    success = base_try_dequeue(base, timer, true);
    spin_unlock_irqrestore(&base->lock, &irqflags);

    return success ? -ENOERR : -EBUSY;
}
EXPORT_SYMBOL(timer_clear);

static state timer_increase(struct timer *timer, uint64_t expires)
{
    if (timer->entry)
        return -EINVAL;

    return -ENOERR;
}

state timer_reduce(struct timer *timer, ttime_t delta)
{
    return timer_increase(timer, delta);
}
EXPORT_SYMBOL(timer_reduce);

state timer_modified(struct timer *timer, ttime_t delta)
{
    return timer_increase(timer, delta);
}
EXPORT_SYMBOL(timer_modified);

/**
 * timer_create - Create a timer node
 * @entry: the timer handle entry
 * @pdata: handle entry pdata
 * @delta: timer expires delta
 * @flags: timer flags
 */
struct timer *timer_create(timer_entry_t entry, void *pdata, ttime_t delta, enum timer_flags flags)
{
    struct timer *timer;

    timer = kcache_zalloc(timer_cache, GFP_KERNEL);
    if (unlikely(!timer))
        return NULL;

    timer->entry = entry;
    timer->pdata = pdata;
    timer->delta = delta;
    timer->flags = flags;

    return timer;
}
EXPORT_SYMBOL(timer_create);

/**
 * softirq_destroy - Destroy a timer node
 * @irq: the softirq to destroy
 */
void timer_destroy(struct timer *timer)
{
    timer_clear(timer);
    kcache_free(timer_cache, timer);
}
EXPORT_SYMBOL(timer_destroy);

static __init void timer_init_cpu(unsigned int cpu)
{
    struct timer_base *base = percpu_ptr(cpu, &timer_bases);

    base->cpu = cpu;
    base->current = ticktime;
    base->recent_timer = base->current + TIMER_EXPIRE_DELTA_MAX;
}

static __init void timer_init_cpus(void)
{
    unsigned int cpu;

    cpu_for_each_possible(cpu)
        timer_init_cpu(cpu);
}

void __init timer_init(void)
{
    timer_cache = KCACHE_CREATE(struct timer, KCACHE_PANIC);
    softirq_regsiter(&timer_softirq);
    timer_init_cpus();
}
