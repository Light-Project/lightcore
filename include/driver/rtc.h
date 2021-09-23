/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RTC_H_
#define _DRIVER_RTC_H_

#include <types.h>
#include <state.h>
#include <spinlock.h>
#include <device.h>
#include <lightcore/rtc.h>

struct rtc_device {
	struct device *device;
    struct rtc_ops *ops;
};

struct rtc_ops {
    state (*read_time)(struct rtc_device *, struct rtc_time *);
    state (*set_time)(struct rtc_device *, struct rtc_time *);
    state (*read_alarm)(struct rtc_device *, struct rtc_alarm *);
    state (*set_alarm)(struct rtc_device *, struct rtc_alarm *);
};

state rtc_register(struct rtc_device *);
state rtc_unregister(struct rtc_device *);

#endif
