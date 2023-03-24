/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BUZZER_H_
#define _DRIVER_BUZZER_H_

#include <device.h>
#include <timer.h>
#include <lightcore/buzzer.h>

/**
 * struct buzzer_device - describe a buzzer device.
 * @dev: points to the parent device of the buzzer device.
 * @ops: operations method of buzzer device.
 * @list: linked list for centralized management of buzzer device.
 * @beep: timer for software implementation of beep operation.
 * @pdata: private data of buzzer device.
 */
struct buzzer_device {
    struct device *dev;
    struct buzzer_ops *ops;
    struct list_head list;
    struct timer beep;
    void *pdata;
};

/**
 * struct buzzer_ops - describe the operations of a buzzer.
 * @start: open speaker gate interface.
 * @stop: close speaker gate interface.
 * @freq_get: getting beep frequency interface.
 * @freq_set: setting beep frequency interface.
 * @beep: hardware timing and frequency beep interface.
 * @panic: emergency alarm in kernel panic interface.
 */
struct buzzer_ops {
    state (*start)(struct buzzer_device *bdev);
    state (*stop)(struct buzzer_device *bdev);
    state (*freq_get)(struct buzzer_device *bdev, enum buzzer_frequency *freq);
    state (*freq_set)(struct buzzer_device *bdev, enum buzzer_frequency freq);
    state (*beep)(struct buzzer_device *bdev, enum buzzer_frequency freq, int64_t msec);
#ifdef CONFIG_BUZZER_PANIC
    void (*panic)(struct buzzer_device *bdev);
#endif
};

#ifndef CONFIG_BUZZER

#define default_buzzer NULL

static inline state buzzer_start(struct buzzer_device *bdev)
{
    return -ENOERR;
}

static inline state buzzer_stop(struct buzzer_device *bdev)
{
    return -ENOERR;
}

static inline state buzzer_freq_get(struct buzzer_device *bdev, enum buzzer_frequency *freq)
{
    return -ENOERR;
}

static inline state buzzer_freq_set(struct buzzer_device *bdev, enum buzzer_frequency freq)
{
    return -ENOERR;
}

static inline state buzzer_freq_set(struct buzzer_device *bdev, enum buzzer_frequency freq)
{
    return -ENOERR;
}

static inline state buzzer_beep(struct buzzer_device *bdev, enum buzzer_frequency freq, int64_t msec)
{
    return -ENOERR;
}

#else /* CONFIG_BUZZER */

extern spinlock_t buzzer_lock;
extern struct list_head buzzer_list;
extern struct buzzer_device *default_buzzer;

extern state buzzer_start(struct buzzer_device *bdev);
extern state buzzer_stop(struct buzzer_device *bdev);
extern state buzzer_freq_get(struct buzzer_device *bdev, enum buzzer_frequency *freq);
extern state buzzer_freq_set(struct buzzer_device *bdev, enum buzzer_frequency freq);
extern state buzzer_beep(struct buzzer_device *bdev, enum buzzer_frequency freq, int64_t msec);

extern state buzzer_register(struct buzzer_device *bdev);
extern void buzzer_unregister(struct buzzer_device *bdev);

#endif /* CONFIG_BUZZER */
#endif /* _DRIVER_BUZZER_H_ */
