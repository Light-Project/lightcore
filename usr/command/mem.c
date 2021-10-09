/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>

int memdump_main(int argc, char *argv[])
{
    return 0;
}

static const struct kshell_command reset_cmd = {
    .entry = memdump_main,
    .name = "memdump",
    .help = "[sl] - Reboot the system."
};

static state mem_init(void)
{
    return kshell_register(&reset_cmd);
}
kshell_initcall(mem_init);