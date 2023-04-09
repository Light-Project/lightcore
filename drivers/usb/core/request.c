/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <driver/usb.h>

int usb_request_submit(struct usb_request *request)
{

    return usb_host_request_submit(request);
}
