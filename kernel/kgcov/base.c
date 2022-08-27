/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kgcov.h>

MUTEX_LOCK(kgcov_lock);
bool kgcov_event_enable;
