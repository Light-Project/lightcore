/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPIO_H_
#define _DRIVER_GPIO_H_

#include <fwnode.h>
#include <driver/dt.h>
#include <asm/gpio.h>

enum gpio_direction {
    GPIO_DIRECTION_INPUT    = 0,
    GPIO_DIRECTION_DRAIN    = 1,
    GPIO_DIRECTION_SOURCE   = 2,
    GPIO_DIRECTION_PUSH     = 3,
};

enum gpio_pull {
    GPIO_PULL_FLOAT         = 0,
    GPIO_PULL_UP            = 1,
    GPIO_PULL_DOWN          = 2,
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
    __GPIOC_SOFT_DRAIN      = 1,
    __GPIOC_SOFT_SOURCE     = 2,
    __GPIOC_TRANSITORY      = 3,
    __GPIOC_IS_HOGGED       = 4,
};

#define GPIOC_ACTIVE_LOW        BIT(__GPIOC_ACTIVE_LOW)
#define GPIOC_SOFT_DRAIN        BIT(__GPIOC_SOFT_DRAIN)
#define GPIOC_SOFT_SOURCE       BIT(__GPIOC_SOFT_SOURCE)
#define GPIOC_TRANSITORY        BIT(__GPIOC_TRANSITORY)
#define GPIOC_IS_HOGGED         BIT(__GPIOC_IS_HOGGED)

enum gpio_lookup_flags {
    GPIOL_ACTIVE_LOW        = BIT(0),
    GPIOL_FLOAT_INPUT       = BIT(1),
    GPIOL_OPEN_DRAIN        = BIT(2),
    GPIOL_OPEN_SOURCE       = BIT(3),
    GPIOL_TRANSITORY        = BIT(4),
    GPIOL_PULL_UP           = BIT(5),
    GPIOL_PULL_DOWN         = BIT(6),
};

/**
 * enum gpio_request_flags - Optional flags to configure direction and output value.
 * @GPIOR_NCHG: Don't change anything.
 * @GPIOR_IN: Set lines to input mode.
 * @GPIOR_OUT_LOW: Set lines to output and drive them low.
 * @GPIOD_OUT_HIGH: Set lines to output and drive them high.
 * @GPIOR_OUT_OPEN_DRAIN_LOW: Set lines to open-drain output and drive them low.
 * @GPIOR_OUT_OPEN_DRAIN_HIGH: Set lines to open-drain output and drive them high.
 * @GPIOR_OUT_OPEN_SOURCE_LOW: Set lines to open-source output and drive them low.
 * @GPIOR_OUT_OPEN_SOURCE_HIGH: Set lines to open-source output and drive them high.
 */
enum gpio_request_flags {
    GPIOR_FLAG_DIR_SET      = BIT(0),
    GPIOR_FLAG_DIR_IN       = BIT(1),
    GPIOR_FLAG_DIR_VAL      = BIT(2),
    GPIOR_FLAG_OPEN_DRAIN   = BIT(3),
    GPIOR_FLAG_OPEN_SOURCE  = BIT(4),

    GPIOR_NCHG                  = 0,
    GPIOR_IN                    = GPIOR_FLAG_DIR_SET | GPIOR_FLAG_DIR_IN,
    GPIOR_OUT_LOW	            = GPIOR_FLAG_DIR_SET,
    GPIOR_OUT_HIGH	            = GPIOR_FLAG_DIR_SET | GPIOR_FLAG_DIR_VAL,
    GPIOR_OUT_OPEN_DRAIN_LOW    = GPIOR_FLAG_OPEN_DRAIN | GPIOR_OUT_LOW,
    GPIOR_OUT_OPEN_DRAIN_HIGH   = GPIOR_FLAG_OPEN_DRAIN | GPIOR_OUT_HIGH,
    GPIOR_OUT_OPEN_SOURCE_LOW   = GPIOR_FLAG_OPEN_SOURCE | GPIOR_OUT_LOW,
    GPIOR_OUT_OPEN_SOURCE_HIGH  = GPIOR_FLAG_OPEN_SOURCE | GPIOR_OUT_HIGH,
};

/**
 * struct gpio_device - describe a gpio channel.
 * @list: the channel is inserted on the @channel of the parent gpio device.
 * @gdev: the parent gpio device for the channel.
 * @index: channel number.
 */
struct gpio_channel {
    struct list_head list;
    struct gpio_device *gdev;
    const char *label;

    unsigned long flags;
    unsigned int index;
    unsigned int using;
};

GENERIC_STRUCT_BITOPS(gpioc, struct gpio_channel, flags, false)
GENERIC_STRUCT_FLAG(gpioc, struct gpio_channel, flags, active_low, __GPIOC_ACTIVE_LOW)
GENERIC_STRUCT_FLAG(gpioc, struct gpio_channel, flags, soft_drain, __GPIOC_SOFT_DRAIN)
GENERIC_STRUCT_FLAG(gpioc, struct gpio_channel, flags, soft_source, __GPIOC_SOFT_SOURCE)
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
 * @gpio_num: number of channels owned by the gpio device.
 */
struct gpio_device {
    spinlock_t lock;
    struct device *dev;
    struct list_head list;
    struct dt_node *dt_node;

    struct gpio_ops *ops;
    struct list_head channel;

    unsigned int gpio_base;
    unsigned int gpio_num;
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
    state (*multiple_get)(struct gpio_device *gdev, unsigned long *mask, unsigned long *value);
    state (*multiple_set)(struct gpio_device *gdev, unsigned long *mask, unsigned long *value);
    state (*direction_get)(struct gpio_device *gdev, unsigned int port, enum gpio_direction *dir);
    state (*direction_set)(struct gpio_device *gdev, unsigned int port, enum gpio_direction dir);
    state (*pull_get)(struct gpio_device *gdev, unsigned int port, enum gpio_pull *pull);
    state (*pull_set)(struct gpio_device *gdev, unsigned int port, enum gpio_pull pull);
    state (*drive_get)(struct gpio_device *gdev, unsigned int port, enum gpio_drive *drive);
    state (*drive_set)(struct gpio_device *gdev, unsigned int port, enum gpio_drive drive);
    state (*speed_get)(struct gpio_device *gdev, unsigned int port, enum gpio_speed *speed);
    state (*speed_set)(struct gpio_device *gdev, unsigned int port, enum gpio_speed speed);
#ifdef CONFIG_GPIO_DT
    unsigned int dt_gpio_cells;
    state (*dt_xlate)(struct gpio_device *gdev, const struct dt_phandle_args *args, uint32_t *index, uint32_t *flags);
#endif
};

struct gpio_dt_quirk {
    struct list_head list;
    struct gpio_channel *(*lookup)(struct dt_node *node, const char *prop, int index, uint32_t *flags);
};

extern spinlock_t gpio_lock;
extern struct list_head gpio_list;
extern const char *const gpio_suffixes[2];

/* gpio operations */
extern state gpio_raw_request(struct gpio_device *gdev, unsigned int port);
extern state gpio_raw_release(struct gpio_device *gdev, unsigned int port);
extern state gpio_raw_dt_xlate(struct gpio_device *gdev, const struct dt_phandle_args *args, uint32_t *index, uint32_t *flags);

extern state gpio_raw_value_get(struct gpio_device *gdev, unsigned int port, bool *value);
extern state gpio_raw_value_set(struct gpio_device *gdev, unsigned int port, bool value);
extern state gpio_raw_direction_get(struct gpio_device *gdev, unsigned int port, enum gpio_direction *dir);
extern state gpio_raw_direction_set(struct gpio_device *gdev, unsigned int port, enum gpio_direction dir);
extern state gpio_raw_pull_get(struct gpio_device *gdev, unsigned int port, enum gpio_pull *pull);
extern state gpio_raw_pull_set(struct gpio_device *gdev, unsigned int port, enum gpio_pull pull);
extern state gpio_raw_drive_get(struct gpio_device *gdev, unsigned int port, enum gpio_drive *drive);
extern state gpio_raw_drive_set(struct gpio_device *gdev, unsigned int port, enum gpio_drive drive);
extern state gpio_raw_speed_get(struct gpio_device *gdev, unsigned int port, enum gpio_speed *speed);
extern state gpio_raw_speed_set(struct gpio_device *gdev, unsigned int port, enum gpio_speed speed);

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

extern state gpioc_value_set(struct gpio_channel *channel, bool value);
extern state gpioc_value_get(struct gpio_channel *channel, bool *value);

extern state gpioc_lookup_configure(struct gpio_channel *channel, enum gpio_lookup_flags flags);
extern state gpioc_request_configure(struct gpio_channel *channel, enum gpio_request_flags flags);
extern state gpioc_configure(struct gpio_channel *channel, enum gpio_lookup_flags lflags, enum gpio_request_flags rflags);

/* gpio device tree linker */
#ifdef CONFIG_GPIO_DT
extern struct gpio_channel *gpio_dt_channel_prop(struct dt_node *node, const char *pname, int index, uint32_t *flags);
extern struct gpio_channel *gpio_dt_channel(struct dt_node *node, const char *label, int index, enum gpio_lookup_flags *flags);
extern state gpio_dt_quirk_register(struct gpio_dt_quirk *quirk);
extern void gpio_dt_quirk_unregister(struct gpio_dt_quirk *quirk);
#endif /* CONFIG_GPIO_DT */

/* gpio acpi linker */
#ifdef CONFIG_GPIO_ACPI
extern struct gpio_channel *gpio_acpi_channel(struct acpi_node *node, const char *label, int index, enum gpio_lookup_flags *flags);
#endif /* CONFIG_GPIO_DT */

/* gpio device fwnode */
extern struct gpio_channel *gpio_fwnode_channel_index(struct fwnode *node, const char *label, int index, enum gpio_request_flags flags);
extern struct gpio_channel *gpio_fwnode_channel(struct fwnode *node, const char *label, enum gpio_request_flags flags);

/* gpio registration */
extern struct gpio_device *gpio_device_find(bool (*match)(struct gpio_device *gdev, void *pdata), void *pdata);
extern struct gpio_channel *gpio_channel_get(struct gpio_device *gdev, unsigned int index);
extern void gpio_channel_put(struct gpio_channel *channel);
extern struct gpio_channel *gpio_channel_number(unsigned int index);
extern state gpio_register(struct gpio_device *gdev);
extern void gpio_unregister(struct gpio_device *gdev);

#define gpio_emerg(gdev, fmt, ...)      dev_emerg((gdev)->dev, fmt, ##__VA_ARGS__)
#define gpio_alert(gdev, fmt, ...)      dev_alert((gdev)->dev, fmt, ##__VA_ARGS__)
#define gpio_crit(gdev, fmt, ...)       dev_crit((gdev)->dev, fmt, ##__VA_ARGS__)
#define gpio_err(gdev, fmt, ...)        dev_err((gdev)->dev, fmt, ##__VA_ARGS__)
#define gpio_warn(gdev, fmt, ...)       dev_warn((gdev)->dev, fmt, ##__VA_ARGS__)
#define gpio_notice(gdev, fmt, ...)     dev_notice((gdev)->dev, fmt, ##__VA_ARGS__)
#define gpio_info(gdev, fmt, ...)       dev_info((gdev)->dev, fmt, ##__VA_ARGS__)
#define gpio_debug(gdev, fmt, ...)      dev_debug((gdev)->dev, fmt, ##__VA_ARGS__)

#define gpioc_emerg(chan, fmt, ...)     gpio_emerg((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)
#define gpioc_alert(chan, fmt, ...)     gpio_alert((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)
#define gpioc_crit(chan, fmt, ...)      gpio_crit((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)
#define gpioc_err(chan, fmt, ...)       gpio_err((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)
#define gpioc_warn(chan, fmt, ...)      gpio_warn((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)
#define gpioc_notice(chan, fmt, ...)    gpio_notice((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)
#define gpioc_info(chan, fmt, ...)      gpio_info((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)
#define gpioc_debug(chan, fmt, ...)     gpio_debug((chan)->gdev, "gpio-%u (%s): " fmt, (chan)->index, (chan)->label ?: "?", ##__VA_ARGS__)

#endif /* _FRAMEWORK_GPIO_GPIO_H_ */
