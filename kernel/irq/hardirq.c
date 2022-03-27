/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <hardirq.h>
#include <bottom-half.h>

void hardirq_entry(void)
{
    hardirq_start();
}

void hardirq_exit(void)
{
    hardirq_end();
    softirq_handle();
}
