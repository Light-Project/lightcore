/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ohci"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <memory.h>
#include <initcall.h>
#include <irq.h>
#include <driver/usb.h>
#include <driver/usb/ohci.h>
#include <printk.h>
