/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define PROGRAM_NAME symbol
#define PROGRAM_DESC "setting symbol variables"
#define PROGRAM_FUNC kshell_symbol
#define PROGRAM_VARS struct kshell_stack *symbol = list_first_entry(&ctx->symbol, struct kshell_stack, list)
#define PROGRAM_CHILD &symbol->env
#include "env.c"
