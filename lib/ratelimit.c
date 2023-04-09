/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "ratelimit"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ratelimit.h>
#include <string.h>
#include <printk.h>
#include <export.h>

bool ratelimit(struct ratelimit *limit, const char *prompt)
{
    irqflags_t irqflags;
    bool retval;

    if (!limit->interval)
        return true;

    if (!spin_trylock_irqsave(&limit->lock, &irqflags))
        return false;

    if (unlikely(!limit->begin))
        limit->begin = ticktime;

    if (ttime_before(ticktime, ttime_add(limit->begin, limit->interval))) {
		if (unlikely(limit->missed) && !ratelimit_test_msgquiet(limit)) {
            pr_warn("%s: %d times suppressed\n", prompt, limit->missed);
            limit->missed = 0;
        }
        limit->begin = ticktime;
        limit->passed = 0;
    }

    if (limit->burst > limit->passed) {
        limit->passed++;
        retval = true;
    } else {
        limit->missed++;
        retval = false;
    }

    spin_unlock_irqrestore(&limit->lock, &irqflags);

    return retval;
}
EXPORT_SYMBOL(ratelimit);

void ratelimit_exit(struct ratelimit *limit, const char *prompt)
{
    if (ratelimit_test_msgquiet(limit))
        return;

    if (unlikely(limit->missed)) {
        pr_warn("%s: %d times suppressed\n", prompt, limit->missed);
        limit->missed = 0;
    }
}
EXPORT_SYMBOL(ratelimit_exit);

void ratelimit_init(struct ratelimit *limit, ttime_t interval, unsigned int burst)
{
    memset(limit, 0, sizeof(*limit));
	spin_lock_init(&limit->lock);
    limit->interval = interval;
    limit->burst = burst;
}
EXPORT_SYMBOL(ratelimit_init);
