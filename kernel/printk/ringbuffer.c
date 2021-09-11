/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define ringbuffer_len  0x4000

static char buffer[ringbuffer_len];
struct kfifo ringbuffer = {
    .buffer = buffer,
    .size = ringbuffer_len,
};