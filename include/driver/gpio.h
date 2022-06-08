/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPIO_H_
#define _DRIVER_GPIO_H_

#include <device.h>
#include <bitflags.h>
#include <driver/dt.h>
#include <dt-bindings/gpio.h>

enum gpio_direction {
    GPIO_DIRECTION_INPUT    = 0,
    GPIO_DIRECTION_DRAIN    = 1,
    GPIO_DIRECTION_PUSH     = 2,
};

enum gpio_pull {
    GPIO_PULL_NONE          = 0,
    GPIO_PULL_LOW           = 1,
    GPIO_PULL_HIGH          = 2,
};

enum gpio_drive {
    GPIO_DRIVER_WEAKEST     = 0,
    GPIO_DRIVER_WEAKER      = 1,
    GPIO_DRIVER_WEAK        = 2,
    GPIO_DRIVER_STRONG      = 3,
    GPIO_DRIVER_STRONGER    = 4,
    GPIO_DRIVER_STRONGEST   = 5,
};

enum gpio_speed {
    GPIO_SPEED_SLOWEST      = 0,
    GPIO_SPEED_SLOWER       = 1,
    GPIO_SPEED_SLOW         = 2,
    GPIO_SPEED_FAST         = 3,
    GPIO_SPEED_FASTER       = 4,
    GPIO_SPEED_FASTEST      = 5,
};

enum gpio_channel_flags {
    __GPIOC_ACTIVE_LOW      = 0,
    __GPIOC_TRANSITORY      = 1,
    __GPIOC_IS_HOGGED       = 2,
};

#define GPIOC_ACTIVE_LOW    BIT(__GPIOC_ACTIVE_LOW)
#define GPIOC_TRANSITORY    BIT(__GPIOC_TRANSITORY)
#define GPIOC_IS_HOGGED     BIT(__GPIOC_IS_HOGGED)

/**
 * struct gpio_device - describe a gpio channel.
 * @gdev: the parent gpio device for the channel.
 * @list: the channel is inserted on the @channel of the parent gpio device.
 * @index: channel number.
 */
struct gpio_channel {
    struct gpio_device *gdev;
    struct list_head list;
    unsigned long index;
    unsigned long flags;
};

GENERIC_STRUCT_BITOPS(gpioc, struct gpio_channel, flags)
GENERIC_STRUCT_FLAG(gpioc, struct gpio_channel, flags, active_low, __GPIOC_ACTIVE_LOW)
GENERIC_STRUCT_FLAG(gpioc, struct gpio_channel, flags, transitory, __GPIOC_TRANSITORY)
GENERIC_STRUCT_FLAG(gpioc, struct gpio_channel, flags, is_hogged, __GPIOC_IS_HOGGED)

/**
 * struct gpio_device - describe a gpio device.
 * @lock: channels lock of the gpio device.
 * @dev: points to the parent device of the gpio device.
 * @list: linked list for centralized management of gpio device.
 * @ops: operations method of gpio device.
 * @dt_node: device tree node pointing to the gpio device.
 * @channel: linked list of allocated channels for the gpio device.
 * @channel_nr: number of channels owned by the gpio device.
 */
struct gpio_device {
    spinlock_t lock;
    struct device *dev;
    struct list_head list;
    struct gpio_ops *ops;
    struct dt_node *dt_node;
    struct list_head channel;
    unsigned int channel_nr;
};

/**
 * struct gpio_ops - describe the operations of a gpio device.
 * @request: request a port channel from gpio controller.
 * @release: release a port channel to gpio controller.
 * @value_get: get the level state of one channel of gpio device.
 * @value_set: set the level state of one channel of gpio device.
 * @direction_get: get the direction of one channel of gpio device.
 * @direction_set: set the direction of one channel of gpio device.
 * @pull_get: get push pull of one channel of gpio device.
 * @pull_set: set push pull of one channel of gpio device.
 * @drive_get: get the driving ability of one channel of gpio device.
 * @drive_set: set the driving ability of one channel of gpio device.
 * @speed_get: get the driving speed of one channel of gpio device.
 * @speed_set: set the driving speed of one channel of gpio device.
 */
struct gpio_ops {
    state (*request)(struct gpio_device *gdev, unsigned int port);
    state (*release)(struct gpio_device *gdev, unsigned int port);
    state (*value_get)(struct gpio_device *gdev, unsigned int port, bool *value);
    state (*value_set)(struct gpio_device *gdev, unsigned int port, bool value);
    state (*direction_get)(struct gpio_device *gdev, unsigned int port, enum gpio_direction *dir);
    state (*direction_set)(struct gpio_device *gdev, unsigned int port, enum gpio_direction dir);
    state (*pull_get)(struct gpio_device *gdev, unsigned int port, enum gpio_pull *pull);
    state (*pull_set)(struct gpio_device *gdev, unsigned int port, enum gpio_pull pull);
    state (*drive_get)(struct gpio_device *gdev, unsigned int port, enum gpio_drive *drive);
    state (*drive_set)(struct gpio_device *gdev, unsigned int port, enum gpio_drive drive);
    state (*speed_get)(struct gpio_device *gdev, unsigned int port, enum gpio_speed *speed);
    state (*speed_set)(struct gpio_device *gdev, unsigned int port, enum gpio_speed speed);
};

extern struct list_head gpio_list;
extern spinlock_t gpio_lock;

/* gpio operations */
extern state gpio_raw_request(struct gpio_device *gdev, unsigned int port);
extern state gpio_raw_release(struct gpio_device *gdev, unsigned int port);
extern state gpio_raw_value_get(struct gpio_device *gdev, unsigned int index, bool *value);
extern state gpio_raw_value_set(struct gpio_device *gdev, unsigned int index, bool value);
extern state gpio_raw_direction_get(struct gpio_device *gdev, unsigned int index, enum gpio_direction *dir);
extern state gpio_raw_direction_set(struct gpio_device *gdev, unsigned int index, enum gpio_direction dir);
extern state gpio_raw_pull_get(struct gpio_device *gdev, unsigned int index, enum gpio_pull *pull);
extern state gpio_raw_pull_set(struct gpio_device *gdev, unsigned int index, enum gpio_pull pull);
extern state gpio_raw_drive_get(struct gpio_device *gdev, unsigned int index, enum gpio_drive *drive);
extern state gpio_raw_drive_set(struct gpio_device *gdev, unsigned int index, enum gpio_drive drive);
extern state gpio_raw_speed_get(struct gpio_device *gdev, unsigned int index, enum gpio_speed *speed);
extern state gpio_raw_speed_set(struct gpio_device *gdev, unsigned int index, enum gpio_speed speed);

extern state gpioc_raw_value_get(struct gpio_channel *channel, bool *value);
extern state gpioc_raw_value_set(struct gpio_channel *channel, bool value);
extern state gpioc_raw_direction_get(struct gpio_channel *channel, enum gpio_direction *dir);
extern state gpioc_raw_direction_set(struct gpio_channel *channel, enum gpio_direction dir);
extern state gpioc_raw_pull_get(struct gpio_channel *channel, enum gpio_pull *pull);
extern state gpioc_raw_pull_set(struct gpio_channel *channel, enum gpio_pull pull);
extern state gpioc_raw_drive_get(struct gpio_channel *channel, enum gpio_drive *drive);
extern state gpioc_raw_drive_set(struct gpio_channel *channel, enum gpio_drive drive);
extern state gpioc_raw_speed_get(struct gpio_channel *channel, enum gpio_speed *speed);
extern state gpioc_raw_speed_set(struct gpio_channel *channel, enum gpio_speed speed);

/* gpio device tree linker */
#ifdef CONFIG_GPIO_DT
extern struct gpio_channel *dt_gpio_channel(struct dt_node *node, int index);
extern struct gpio_channel *dt_gpio_channel_name(struct dt_node *node, const char *name);
#endif

/* gpio registration */
extern struct gpio_device *gpio_device_find(bool (*match)(struct gpio_device *gdev, void *pdata), void *pdata);
extern struct gpio_channel *gpio_channel_get(struct gpio_device *gdev, unsigned int index);
extern void gpio_channel_release(struct gpio_channel *channel);
extern state gpio_register(struct gpio_device *gdev);
extern void gpio_unregister(struct gpio_device *gdev);

#endif /* _FRAMEWORK_GPIO_GPIO_H_ */
