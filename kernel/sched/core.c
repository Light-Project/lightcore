/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */


#include <string.h>
#include <initcall.h>
#include <sort.h>
#include <sched.h>
#include <printk.h>

static LIST_HEAD(sched_list);
