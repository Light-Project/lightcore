/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/power.h>
#include <asm/proc.h>
#include <kshell.h>

static void help(void)
{

}

static int bench_main(int argc, char *argv[])
{

}

static struct kshell_command bench_cmd = {
    .name = "bench",
    .desc = "BenchMark",
    .exec = bench_main,
};

static state bench_init(void)
{
    return kshell_register(&bench_cmd);
}
kshell_initcall(bench_init);
