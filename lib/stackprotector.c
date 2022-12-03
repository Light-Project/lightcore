/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <panic.h>
#include <prandom.h>
#include <stackprotector.h>
#include <export.h>

unsigned long random_canary(void)
{
	return prandom_value() & CANARY_MASK;
}

void __weak __visible __stack_chk_fail(void)
{
    panic("stack-protector: Kernel stack is corrupted in: %p", (void *)_RET_IP_);
}
EXPORT_SYMBOL(__stack_chk_fail);
