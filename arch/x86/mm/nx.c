/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "NX protection"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <initcall.h>
#include <printk.h>
#include <asm/regs.h>

static bool noexec_disable;

void __init arch_report_nx(void)
{
    if (!cpuid_support(X86_CPUID_NX))
        pr_info("CPU does not support\n");
    else if (noexec_disable)
        pr_info("disabled by kernel command\n");
    else
        pr_info("active\n");
}

void __init arch_setup_nx(void)
{

}

static __init state noexec_cmdinit(char *arg)
{
    if (strcmp("enable", arg))
        noexec_disable = false;
    else
        noexec_disable = true;
    return -EINVAL;
}
bootarg_initcall("noexec", noexec_cmdinit);
