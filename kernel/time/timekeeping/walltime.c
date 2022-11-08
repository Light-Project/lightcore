/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

static inline void
timekeeping_wall_mono_set(struct timekeeper *tk, struct timespec *ts)
{
    struct timespec tmp;

    tk->timespec_real = *ts;
    timespec_set_normalized(&tmp, -ts->tv_sec, -ts->tv_nsec);
    tk->offset_real = timespec_to_ktime(&tmp);
    tk->offset_tai = ktime_add(tk->offset_real, tk->tai_offset);
}

static inline void
timekeeping_sleep_update(struct timekeeper *tk, ktime_t delta)
{
    tk->offset_boot += delta;
	tk->timespec_boot = ktime_to_timespec(tk->offset_boot);
}
