/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CPUIDLE_H_
#define _DRIVER_CPUIDLE_H_

#include <kernel.h>
#include <list.h>
#include <cpu.h>
#include <mutex.h>
#include <percpu.h>

#define CPUIDLE_RATING_DUMMY    0
#define CPUIDLE_RATING_BASE     100
#define CPUIDLE_RATING_GOOD     200
#define CPUIDLE_RATING_DESIRED  300
#define CPUIDLE_RATING_PERFECT  400

struct cpuidle_device {
    struct list_head detected_list;
    struct cpuidle_driver *driver;

    unsigned int cpu;
    uint32_t enabled:1;
    uint32_t poll_allow_limit:1;

    unsigned int last_ladder;
    uint64_t last_residency;
    uint64_t poll_limit;
};

/**
 * struct cpuidle_driver - describe the driver of idle device.
 * @name: the name of device driver.
 * @states: base pointer of level state array.
 * @ladders: total number of idle level states.
 */
struct cpuidle_driver {
    const char *name;
    const struct cpuidle_state *states;
    unsigned int ladders;
};

/**
 * struct cpuidle_state - describe the state of a device idle level.
 * @name: name of the idle level of the device.
 * @desc: description of device idle level.
 * @target_residency: expected idle time of level (in ns).
 * @exit_latency: delay time required to exit this level (in ns).
 * @power_usage: expected power consumption at this level (in mW).
 * @enter: callback function entering this level.
 */
struct cpuidle_state {
    const char *name;
    const char *desc;

    uint64_t target_residency;
    uint64_t exit_latency;
    unsigned int power_usage;

    unsigned long flags;
    state (*enter)(struct cpuidle_device *cdev, unsigned int *index);
};

/**
 * struct cpuidle_governor - describe a cpuidle governor.
 * @name: the name of governor.
 * @rating: determine the priority to use this governor.
 * @activate: make devices active on this governor.
 * @deactivate: make devices deactivate on this governor.
 * @select: selects an appropriate idle level.
 * @reflect: reflect the level actually used.
 */
struct cpuidle_governor {
    struct list_head list;
    const char *name;
    unsigned int rating;

	state (*activate)(struct cpuidle_device *cdev);
	void (*deactivate)(struct cpuidle_device *cdev);
	unsigned int (*select)(struct cpuidle_device *cdev);
	void (*reflect)(struct cpuidle_device *cdev, unsigned int index);
};

extern struct mutex cpuidle_lock;
extern struct list_head cpuidle_detected_devices;
extern struct list_head cpuidle_governors;
extern struct cpuidle_governor *cpuidle_governor_curr;
DECLARE_PERCPU(struct cpuidle_device *, cpuidle_devices);

#ifndef CONFIG_CPUIDLE
static inline struct cpuidle_device *cpuidle_device_get(void) {return NULL;}
static inline unsigned int cpuidle_select(struct cpuidle_device *cdev) {return 0;}
static inline void cpuidle_reflect(struct cpuidle_device *cdev, unsigned int index) {}
static inline state cpuidle_enter(struct cpuidle_device *cdev, unsigned int *index) {return -ENODEV;}
static inline uint64_t cpuidle_poll_time(struct cpuidle_device *cdev) {return 0;}

static inline state cpuidle_device_activate(struct cpuidle_device *cdev) {return -ENODEV;}
static inline void cpuidle_device_deactivate(struct cpuidle_device *cdev) {}

static inline bool cpuidle_available(struct cpuidle_device *cdev) {return false;}
static inline state cpuidle_device_register(struct cpuidle_device *cdev) {return -ENODEV;}
static inline void cpuidle_device_unregister(struct cpuidle_device *cdev) {}
#else
static inline struct cpuidle_device *cpuidle_device_get(void)
{
    return thiscpu_ptr(cpuidle_devices);
}

extern unsigned int cpuidle_select(struct cpuidle_device *cdev);
extern void cpuidle_reflect(struct cpuidle_device *cdev, unsigned int index);
extern state cpuidle_enter(struct cpuidle_device *cdev, unsigned int *index);

extern state cpuidle_device_activate(struct cpuidle_device *cdev);
extern void cpuidle_device_deactivate(struct cpuidle_device *cdev);
extern uint64_t cpuidle_poll_time(struct cpuidle_device *cdev);

extern bool cpuidle_available(struct cpuidle_device *cdev);
extern state cpuidle_device_register(struct cpuidle_device *cdev);
extern void cpuidle_device_unregister(struct cpuidle_device *cdev);
#endif

#if !defined(CONFIG_CPUIDLE) || !defined(CONFIG_ARCH_HAS_CPU_RELAX)
static inline void cpuidle_poll_init(struct cpuidle_state *csta) {}
#else
extern void cpuidle_poll_init(struct cpuidle_state *csta);
#endif

extern struct cpuidle_governor *cpuidle_governor_find(const char *name);
extern void cpuidle_governor_switch(struct cpuidle_governor *governor);
extern state cpuidle_governor_register(struct cpuidle_governor *governor);
extern void cpuidle_governor_unregister(struct cpuidle_governor *governor);

#endif /* _DRIVER_CPUIDLE_H_ */
