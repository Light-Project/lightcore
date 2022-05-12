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

/**
 * struct rtc_device - describe a rtc device.
 * @dev: points to the parent device of the rtc device.
 * @ops: operations method of rtc device.
 * @info: informations of rtc device.
 * @list: linked list for centralized management of rtc device.
 * @time_offset: current time offset of the rtc device.
 */
struct rtc_device {
	struct device *dev;
    struct rtc_ops *ops;
    struct rtc_info *info;
    struct list_head list;
    time_t time_offset;
};

/**
 * struct rtc_info - describe the informations of a rtc.
 * @time_min: minimum time that the rtc device can describe.
 * @time_max: maximum time that the rtc device can describe.
 */
struct rtc_info {
    time_t time_min;
    time_t time_max;
};

/**
 * struct rtc_ops - describe the operations of a rtc.
 * @time_get: get the time of this rtc device.
 * @time_set: set the time for this rtc device.
 * @offset_get: get the time offset of this rtc device.
 * @offset_set: set the time offset for this rtc device.
 * @alarm_get: get the alarm time of this rtc device.
 * @alarm_set: set the alarm time for this rtc device.
 * @ioctl: ioctl callback function for this rtc device.
 */
struct rtc_ops {
    state (*time_get)(struct rtc_device *rdev, struct rtc_time *rtc);
    state (*time_set)(struct rtc_device *rdev, struct rtc_time *rtc);
    state (*offset_get)(struct rtc_device *rdev, time_t *offset);
    state (*offset_set)(struct rtc_device *rdev, time_t offset);
    state (*alarm_get)(struct rtc_device *rdev, struct rtc_alarm *rtc);
    state (*alarm_set)(struct rtc_device *rdev, struct rtc_alarm *rtc);
    state (*ioctl)(struct rtc_device *rdev, unsigned int, unsigned long);
};

extern spinlock_t rtc_lock;
extern struct list_head rtc_list;

extern state rtc_time_get(struct rtc_device *rdev, struct rtc_time *rtc);
extern state rtc_time_set(struct rtc_device *rdev, struct rtc_time *rtc);
extern state rtc_offset_get(struct rtc_device *rdev, time_t *offset);
extern state rtc_offset_set(struct rtc_device *rdev, time_t offset);
extern state rtc_alarm_get(struct rtc_device *rdev, struct rtc_alarm *rtc);
extern state rtc_alarm_set(struct rtc_device *rdev, struct rtc_alarm *rtc);

extern state rtc_register(struct rtc_device *rdev);
extern void rtc_unregister(struct rtc_device *rdev);

#endif  /* _DRIVER_RTC_H_ */
