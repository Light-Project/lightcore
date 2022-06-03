/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <logo.h>
#include <printk.h>

void __init terminal_logo(void)
{
    pr_early("                                                    \n");
    pr_early("----------------------------------------------------\n");
    pr_early(" _     _       _     _    ____                      \n");
    pr_early("| |   (_) __ _| |__ | |_ / ___|___  _ __ ___        \n");
    pr_early("| |   | |/ _` | '_ \\| __| |   / _ \\| '__/ _ \\    \n");
    pr_early("| |___| | (_| | | | | |_| |__| (_) | | |  __/       \n");
    pr_early("|_____|_|\\__, |_| |_|\\__|\\____\\___/|_|  \\___|  \n");
    pr_early("         |___/                                      \n");
}
