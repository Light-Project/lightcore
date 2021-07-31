#ifndef _DRIVER_RTC_H_ 
#define _DRIVER_RTC_H_

#include <types.h>
#include <state.h>
#include <kernel/spinlock.h>
#include <device.h>
#include <lightcore/rtc.h>

struct rtc_ops {
    state (*read_time)(struct device *, struct rtc_time *);
    state (*set_time)(struct device *, struct rtc_time *);
    state (*read_alarm)(struct device *, struct rtc_alarm *);
    state (*set_alarm)(struct device *, struct rtc_alarm *);
};

struct rtc_device {
	struct device dev;
    spinlock_t lock;
    struct rtc_ops *ops;
};

state rtc_register(struct rtc_device *);
state rtc_unregister(struct rtc_device *);

#endif
