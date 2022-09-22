/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

static inline void
timekeeping_wall_mono_set(struct timekeeper *tk, struct timespec *ts)
{
    struct timespec tmp;

    tk->timespec_wall = *ts;
    timespec_set_normalized(&tmp, -ts->tv_sec, -ts->tv_nsec);
    tk->offset_real = timespec_to_ktime(&tmp);
}
