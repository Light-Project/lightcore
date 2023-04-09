/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#define PROGRAM_NAME local
#define PROGRAM_DESC "setting local variables"
#define PROGRAM_FUNC kshell_local
#define PROGRAM_VARS struct kshell_stack *local = list_first_entry(&ctx->local, struct kshell_stack, list)
#define PROGRAM_CHILD &local->env
#include "env.c"
