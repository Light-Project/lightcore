/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKEVENT_H_
#define _DRIVER_CLOCKEVENT_H_

#include <ktime.h>
#include <device.h>

#define CLOCK_RATING_UNSAFE     0
#define CLOCK_RATING_BASE       200
#define CLOCK_RATING_GOOD       400
#define CLOCK_RATING_DESIRED    800
#define CLOCK_RATING_PERFECT    1400

enum clockevent_state {
    CLKEVT_STATE_UNINIT     = 0,
    CLKEVT_STATE_SHUTDOWN   = 1,
    CLKEVT_STATE_PERIODIC   = 2,
    CLKEVT_STATE_ONESHOT    = 3,
    CLKEVT_STATE_STOP       = 4,
};

enum clockevent_capability {
    CLKEVT_CAP_PERIODIC     = BIT(0),
    CLKEVT_CAP_ONESHOT      = BIT(1),
    CLKEVT_CAP_PERCPU       = BIT(2),
};

/**
 *
 *
 */
struct clockevent_device {
    /* device base data */
    struct list_head list;
    struct device *device;
    struct clockevent_ops *ops;

    /* clockevent information */
    uint64_t delta_min;
    uint64_t delta_max;
    unsigned int rating;
    unsigned int mult;
    unsigned int shift;
    enum clockevent_capability capability;

    /* clockevent state */
    enum clockevent_state state;
    void (*event_handle)(struct clockevent_device *cdev);
};

/**
 * struct clockevent_ops - periodic running counter operations
 * @next_event: program the next event in oneshot mode
 * @stop_event: remove the next event in oneshot mode
 * @state_oneshot: switch state to oneshot
 * @state_periodic: switch state to periodic
 * @state_shutdown: switch state to shutdown
 * @state_resume: resume before shutdown
 */
struct clockevent_ops {
    state (*next_event)(struct clockevent_device *cdev, uint64_t delta);
    state (*stop_event)(struct clockevent_device *cdev);
    state (*state_oneshot)(struct clockevent_device *cdev);
    state (*state_periodic)(struct clockevent_device *cdev, uint64_t delta);
    state (*state_shutdown)(struct clockevent_device *cdev);
    state (*state_resume)(struct clockevent_device *cdev);
};

extern void clockevent_cloc_mult_shift(unsigned int *mult, unsigned int *shift, unsigned int from, uint64_t to, unsigned int maxsec);
extern state clockevent_switch_state(struct clockevent_device *cdev, enum clockevent_state cstate);
extern state clockevent_program_event(struct clockevent_device *cdev, ktime_t count);

extern void clockevent_config(struct clockevent_device *cdev, uint64_t freq, uint64_t delta_min, uint64_t delta_max);
extern state clockevent_config_register(struct clockevent_device *cdev, uint64_t freq, uint64_t delta_min, uint64_t delta_max);
extern state clockevent_register(struct clockevent_device *cdev);
extern void clockevent_unregister(struct clockevent_device *cdev);
extern void __init clockevent_init(void);

#endif /* _DRIVER_CLOCKEVENT_H_ */
