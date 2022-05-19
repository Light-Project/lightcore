/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_LED_H_
#define _DRIVER_LED_H_

#include <device.h>
#include <lightcore/led.h>

enum led_default_state {
    LED_DEFSTATE_OFF    = 0,
    LED_DEFSTATE_ON     = 1,
    LED_DEFSTATE_KEEP   = 2,
};

/**
 * struct led_device - describe a led device.
 * @dev: points to the parent device of the led device.
 * @ops: operations method of led device.
 * @list: linked list for centralized management of led device.
 * @trigger_list: linked list for trigger management led device.
 * @trigger: the triggers currently used by led device.
 * @brightness: the brightness of the current led device.
 * @max_brightness: the maximum brightness of the led device.
 * @default_state: default state of led device.
 * @pdata: private data of led device.
 */
struct led_device {
    struct device *dev;
    struct led_ops *ops;
    struct list_head list;
    struct list_head trigger_list;
    struct led_trigger *trigger;
    enum led_brightness brightness;
    enum led_brightness max_brightness;
    enum led_default_state default_state;
    void *pdata;
};

/**
 * struct led_ops - describe the operations of a led.
 * @enable: enable led device interface.
 * @disable: disable led device interface.
 * @brightness_get: getting led brightness interface.
 * @brightness_set: setting led brightness interface.
 * @blink_get: getting led blank in milliseconds interface.
 * @blink_set: setting led blank in milliseconds interface.
 */
struct led_ops {
    state (*enable)(struct led_device *ldev);
    state (*disable)(struct led_device *ldev);
    state (*brightness_get)(struct led_device *ldev, enum led_brightness *bn);
    state (*brightness_set)(struct led_device *ldev, enum led_brightness bn);
    state (*blink_get)(struct led_device *ldev, unsigned int *on, unsigned int *off);
    state (*blink_set)(struct led_device *ldev, unsigned int on, unsigned int off);
};

/**
 * struct led_trigger - describe a led trigger.
 * @name: name of led trigger.
 * @list: list node for centralized management of led triggers.
 * @activate: add led device into this trigger.
 * @deactivate: remove led device form this trigger.
 */
struct led_trigger {
    const char *name;
    struct list_head list;
    state (*activate)(struct led_device *ldev);
    void (*deactivate)(struct led_device *ldev);
};

extern spinlock_t led_lock;
extern struct list_head led_list;

extern state led_enable(struct led_device *ldev);
extern state led_disable(struct led_device *ldev);
extern state led_brightness_get(struct led_device *ldev, enum led_brightness *bn);
extern state led_brightness_set(struct led_device *ldev, enum led_brightness bn);
extern state led_blink_get(struct led_device *ldev, unsigned int *on, unsigned int *off);
extern state led_blink_set(struct led_device *ldev, unsigned int on, unsigned int off);

extern state led_trigger_activate(struct led_device *ldev);
extern void led_trigger_deactivate(struct led_device *ldev);
extern state led_trigger_register(struct led_trigger *trigger);
extern void led_trigger_unregister(struct led_trigger *trigger);

extern enum led_default_state led_fwnode_default_state(struct fwnode *fwnode);
extern state led_register(struct led_device *ldev);
extern void led_unregister(struct led_device *ldev);

#endif  /* _DRIVER_LED_H_ */
