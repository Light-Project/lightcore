/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>

int mem_main(int argc, char *argv[])
{
    return 0;
}

static const struct kshell_command reset_cmd = {
    .name = "memdump",
    .desc = "Displays the memory contents",
    .exec = mem_main,
};

static state mem_init(void)
{
    return kshell_register(&reset_cmd);
}
kshell_initcall(mem_init);