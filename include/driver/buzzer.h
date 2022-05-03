/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BUZZER_H_
#define _DRIVER_BUZZER_H_

#include <device.h>
#include <timer.h>

enum buzzer_frequency {
    BUZZER_FREQ_C0      = 16,
    BUZZER_FREQ_CS0     = 17,
    BUZZER_FREQ_D0      = 18,
    BUZZER_FREQ_DS0     = 20,
    BUZZER_FREQ_E0      = 21,
    BUZZER_FREQ_F0      = 22,
    BUZZER_FREQ_FS0     = 23,
    BUZZER_FREQ_G0      = 25,
    BUZZER_FREQ_GS0     = 26,
    BUZZER_FREQ_A0      = 28,
    BUZZER_FREQ_AS0     = 29,
    BUZZER_FREQ_B0      = 31,

    BUZZER_FREQ_C1      = 33,
    BUZZER_FREQ_CS1     = 35,
    BUZZER_FREQ_D1      = 37,
    BUZZER_FREQ_DS1     = 39,
    BUZZER_FREQ_E1      = 41,
    BUZZER_FREQ_F1      = 44,
    BUZZER_FREQ_FS1     = 46,
    BUZZER_FREQ_G1      = 49,
    BUZZER_FREQ_GS1     = 52,
    BUZZER_FREQ_A1      = 55,
    BUZZER_FREQ_AS1     = 58,
    BUZZER_FREQ_B1      = 62,

    BUZZER_FREQ_C2      = 65,
    BUZZER_FREQ_CS2     = 69,
    BUZZER_FREQ_D2      = 73,
    BUZZER_FREQ_DS2     = 78,
    BUZZER_FREQ_E2      = 82,
    BUZZER_FREQ_F2      = 87,
    BUZZER_FREQ_FS2     = 93,
    BUZZER_FREQ_G2      = 98,
    BUZZER_FREQ_GS2     = 104,
    BUZZER_FREQ_A2      = 110,
    BUZZER_FREQ_AS2     = 117,
    BUZZER_FREQ_B2      = 123,

    BUZZER_FREQ_C3      = 131,
    BUZZER_FREQ_CS3     = 139,
    BUZZER_FREQ_D3      = 147,
    BUZZER_FREQ_DS3     = 156,
    BUZZER_FREQ_E3      = 165,
    BUZZER_FREQ_F3      = 175,
    BUZZER_FREQ_FS3     = 185,
    BUZZER_FREQ_G3      = 196,
    BUZZER_FREQ_GS3     = 208,
    BUZZER_FREQ_A3      = 220,
    BUZZER_FREQ_AS3     = 233,
    BUZZER_FREQ_B3      = 247,

    BUZZER_FREQ_C4      = 262,
    BUZZER_FREQ_CS4     = 277,
    BUZZER_FREQ_D4      = 294,
    BUZZER_FREQ_DS4     = 311,
    BUZZER_FREQ_E4      = 330,
    BUZZER_FREQ_F4      = 349,
    BUZZER_FREQ_FS4     = 370,
    BUZZER_FREQ_G4      = 392,
    BUZZER_FREQ_GS4     = 415,
    BUZZER_FREQ_A4      = 440,
    BUZZER_FREQ_AS4     = 466,
    BUZZER_FREQ_B4      = 494,

    BUZZER_FREQ_C5      = 523,
    BUZZER_FREQ_CS5     = 554,
    BUZZER_FREQ_D5      = 587,
    BUZZER_FREQ_DS5     = 622,
    BUZZER_FREQ_E5      = 659,
    BUZZER_FREQ_F5      = 698,
    BUZZER_FREQ_FS5     = 740,
    BUZZER_FREQ_G5      = 784,
    BUZZER_FREQ_GS5     = 831,
    BUZZER_FREQ_A5      = 880,
    BUZZER_FREQ_AS5     = 932,
    BUZZER_FREQ_B5      = 988,

    BUZZER_FREQ_C6      = 1047,
    BUZZER_FREQ_CS6     = 1109,
    BUZZER_FREQ_D6      = 1175,
    BUZZER_FREQ_DS6     = 1245,
    BUZZER_FREQ_E6      = 1319,
    BUZZER_FREQ_F6      = 1397,
    BUZZER_FREQ_FS6     = 1480,
    BUZZER_FREQ_G6      = 1568,
    BUZZER_FREQ_GS6     = 1661,
    BUZZER_FREQ_A6      = 1760,
    BUZZER_FREQ_AS6     = 1865,
    BUZZER_FREQ_B6      = 1976,

    BUZZER_FREQ_C7      = 2093,
    BUZZER_FREQ_CS7     = 2217,
    BUZZER_FREQ_D7      = 2349,
    BUZZER_FREQ_DS7     = 2489,
    BUZZER_FREQ_E7      = 2637,
    BUZZER_FREQ_F7      = 2794,
    BUZZER_FREQ_FS7     = 2960,
    BUZZER_FREQ_G7      = 3136,
    BUZZER_FREQ_GS7     = 3322,
    BUZZER_FREQ_A7      = 3520,
    BUZZER_FREQ_AS7     = 3729,
    BUZZER_FREQ_B7      = 3951,

    BUZZER_FREQ_C8      = 4186,
    BUZZER_FREQ_CS8     = 4435,
    BUZZER_FREQ_D8      = 4699,
    BUZZER_FREQ_DS8     = 4978,
};

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

#endif  /* CONFIG_BUZZER */
#endif  /* _DRIVER_BUZZER_H_ */
