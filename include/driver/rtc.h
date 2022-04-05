/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RTC_H_
#define _DRIVER_RTC_H_

#include <types.h>
#include <device.h>
#include <librtc.h>
#include <driver/chardev.h>

#define RTC_TIMESTAMP_0000  -62167219200ULL /* 0000-01-01 00:00:00 */
#define RTC_TIMESTAMP_1900  -2208988800LL   /* 1900-01-01 00:00:00 */
#define RTC_TIMESTAMP_2000  946684800LL     /* 2000-01-01 00:00:00 */
#define RTC_TIMESTAMP_2063  2966371199LL    /* 2063-12-31 23:59:59 */
#define RTC_TIMESTAMP_2079  3471292799LL    /* 2079-12-31 23:59:59 */
#define RTC_TIMESTAMP_2099  4102444799LL    /* 2099-12-31 23:59:59 */
#define RTC_TIMESTAMP_2199  7258118399LL    /* 2199-12-31 23:59:59 */
#define RTC_TIMESTAMP_9999  253402300799LL  /* 9999-12-31 23:59:59 */

struct rtc_device {
	struct device *dev;
    struct rtc_info *info;
    struct rtc_ops *ops;
    time_t time_offset;
};

struct rtc_info {
    time_t time_min;
    time_t time_max;
};

struct rtc_ops {
    state (*time_get)(struct rtc_device *, struct rtc_time *);
    state (*time_set)(struct rtc_device *, struct rtc_time *);
    state (*offset_get)(struct rtc_device *, time_t *offset);
    state (*offset_set)(struct rtc_device *, time_t offset);
    state (*alarm_get)(struct rtc_device *, struct rtc_alarm *);
    state (*alarm_set)(struct rtc_device *, struct rtc_alarm *);
    state (*ioctl)(struct rtc_device *, unsigned int, unsigned long);
};

/* RTC operation */
extern state rtc_time_get(struct rtc_device *, struct rtc_time *);
extern state rtc_time_set(struct rtc_device *, struct rtc_time *);
extern state rtc_offset_get(struct rtc_device *, time_t *offset);
extern state rtc_offset_set(struct rtc_device *, time_t offset);
extern state rtc_alarm_get(struct rtc_device *, struct rtc_alarm *);
extern state rtc_alarm_set(struct rtc_device *, struct rtc_alarm *);

/* RTC core */
extern state rtc_register(struct rtc_device *);
extern void rtc_unregister(struct rtc_device *);

#endif
