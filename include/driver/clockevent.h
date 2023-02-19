/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKEVENT_H_
#define _DRIVER_CLOCKEVENT_H_

#include <ktime.h>
#include <device.h>

#define CLOCK_RATING_UNSAFE     0
#define CLOCK_RATING_BASE       100
#define CLOCK_RATING_GOOD       200
#define CLOCK_RATING_DESIRED    300
#define CLOCK_RATING_PERFECT    400

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
    CLKEVT_CAP_KTIME        = BIT(2),
    CLKEVT_CAP_PERCPU       = BIT(3),
};

/**
 * clockevent_device - describes clockevent device
 * @delta_min: maximum delta value (ns)
 * @delta_max: minimum delta value (ns)
 * @rating: clockevent accuracy rating
 * @mult: nsec to cycles multiplier
 * @shift: nsec to cycles divisor
 * @capability: clockevent capability
 * @next_event: time of next event
 * @state: clockevent current state
 * @event_handle: clockevent interrupt handle
 */
struct clockevent_device {
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
    ktime_t next_event;
    enum clockevent_state state;
    void (*event_handle)(struct clockevent_device *cdev);
};

/**
 * clockevent_ops - periodic running counter operations
 * @next_ktime: program the next event using ktime in oneshot mode
 * @next_event: program the next event using delta in oneshot mode
 * @stop_event: remove the next event in oneshot mode
 * @state_oneshot: switch state to oneshot
 * @state_periodic: switch state to periodic
 * @state_shutdown: switch state to shutdown
 * @state_resume: resume before shutdown
 */
struct clockevent_ops {
    state (*next_ktime)(struct clockevent_device *cdev, ktime_t time);
    state (*next_event)(struct clockevent_device *cdev, uint64_t delta);
    state (*stop_event)(struct clockevent_device *cdev);
    state (*state_oneshot)(struct clockevent_device *cdev);
    state (*state_periodic)(struct clockevent_device *cdev, uint64_t delta);
    state (*state_shutdown)(struct clockevent_device *cdev);
    state (*state_resume)(struct clockevent_device *cdev);
};

extern state clockevent_switch_state(struct clockevent_device *cdev, enum clockevent_state cstate);
extern state clockevent_program_event(struct clockevent_device *cdev, ktime_t stamp);
extern void clockevent_config(struct clockevent_device *cdev, uint64_t freq, uint64_t delta_min, uint64_t delta_max);
extern state clockevent_config_register(struct clockevent_device *cdev, uint64_t freq, uint64_t delta_min, uint64_t delta_max);
extern state clockevent_register(struct clockevent_device *cdev);
extern void clockevent_unregister(struct clockevent_device *cdev);
extern void __init clockevent_init(void);

#endif /* _DRIVER_CLOCKEVENT_H_ */
