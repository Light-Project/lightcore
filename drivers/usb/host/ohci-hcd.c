/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ohci"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <init/initcall.h>
#include <kernel/irq.h>
#include <driver/usb.h>
#include <driver/usb/ohci.h>
#include <printk.h>