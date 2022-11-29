/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PARALLEL_H_
#define _DRIVER_PARALLEL_H_

#include <device.h>
#include <devtable.h>
#include <lightcore/parallel.h>

enum parallel_flags {
    PARALLEL_NONE,
};

/**
 * struct parallel_device - describe a parallel device.
 * @dev: kernel infrastructure device architecture.
 * @name: name used to match the platform_table.
 * @dt_node: device tree node of the device.
 * @host: parallel controller used with the device.
 */
struct parallel_device {
    struct device dev;
    const char *name;
    struct dt_node *dt_node;
    struct parallel_host *host;
};

#define device_to_parallel_device(ptr) \
    container_of(ptr, struct parallel_device, dev)

static inline void *parallel_get_devdata(const struct parallel_device *pdev)
{
    return device_get_pdata(&pdev->dev);
}

static inline void parallel_set_devdata(struct parallel_device *pdev, void *data)
{
    return device_set_pdata(&pdev->dev, data);
}

/**
 * struct parallel_driver - describe a parallel driver.
 * @driver: kernel infrastructure driver architecture.
 * @platform_table: list of parallel devices supported by this driver.
 * @dt_table: list of parallel devices supported by this driver.
 * @probe: binds this driver from the parallel device.
 * @remove: unbinds this driver from the parallel device.
 * @shutdown: shutdown parallel device.
 */
struct parallel_driver {
    struct driver driver;
    const struct platform_device_id *platform_table;
    const struct dt_device_id *dt_table;

    state (*probe)(struct parallel_device *pdev, const void *pdata);
    void (*remove)(struct parallel_device *pdev);
    void (*shutdown)(struct parallel_device *pdev);
};

#define driver_to_parallel_driver(ptr) \
    container_of(ptr, struct parallel_driver, driver)

/**
 * struct parallel_host - describe a parallel host.
 * @dev: points to the parent device of the parallel host.
 * @dt_node: points to the parent device tree of the parallel host.
 * @list: linked list for centralized management of parallel host.
 * @ops: operations method of parallel host.
 */
struct parallel_host {
    struct device *dev;
    struct list_head list;
    struct parallel_ops *ops;
};

/**
 * struct parallel_ops - describe the operations of a parallel host.
 * @data_read: legacy mode read data interface.
 * @data_write: legacy mode write data interface.
 * @ctrl_read: legacy mode read control interface.
 * @ctrl_write: legacy mode write control interface.
 */
struct parallel_ops {
    state (*data_read)(struct parallel_host *host, uint8_t *data);
    state (*data_write)(struct parallel_host *host, uint8_t data);
    state (*ctrl_read)(struct parallel_host *host, uint8_t *data);
    state (*ctrl_write)(struct parallel_host *host, uint8_t data);
    state (*status_read)(struct parallel_host *host, uint8_t *data);

    state (*epp_data_read)(struct parallel_host *host, const void *buff, unsigned int len, enum parallel_flags flags);
    state (*epp_data_write)(struct parallel_host *host, void *buff, unsigned int len, enum parallel_flags flags);
    state (*epp_addr_read)(struct parallel_host *host, const void *buff, unsigned int len, enum parallel_flags flags);
    state (*epp_addr_write)(struct parallel_host *host, void *buff, unsigned int len, enum parallel_flags flags);

    state (*ecp_data_read)(struct parallel_host *host, const void *buff, unsigned int len, enum parallel_flags flags);
    state (*ecp_data_write)(struct parallel_host *host, void *buff, unsigned int len, enum parallel_flags flags);
    state (*ecp_addr_write)(struct parallel_host *host, void *buff, unsigned int len, enum parallel_flags flags);
};

extern struct bus_type parallel_bus;

/* parallel device matching */
extern void dt_populate_parallel(struct parallel_host *host);

/* parallel operations */
extern state parallel_host_data_read(struct parallel_host *host, uint8_t *data);
extern state parallel_host_data_write(struct parallel_host *host, uint8_t data);
extern state parallel_host_ctrl_read(struct parallel_host *host, uint8_t *data);
extern state parallel_host_ctrl_write(struct parallel_host *host, uint8_t data);
extern state parallel_host_status_read(struct parallel_host *host, uint8_t *data);

extern state parallel_data_read(struct parallel_device *pdev, uint8_t *data);
extern state parallel_data_write(struct parallel_device *pdev, uint8_t data);
extern state parallel_ctrl_read(struct parallel_device *host, uint8_t *data);
extern state parallel_ctrl_write(struct parallel_device *host, uint8_t data);
extern state parallel_status_read(struct parallel_device *host, uint8_t *data);

/* parallel registration */
extern struct parallel_device *parallel_device_alloc(struct parallel_host *host, const char *name);
extern void parallel_device_free(struct parallel_device *pdev);
extern state parallel_device_register(struct parallel_device *pdev);
extern void parallel_device_unregister(struct parallel_device *pdev);
extern state parallel_driver_register(struct parallel_driver *pdrv);
extern void parallel_driver_unregister(struct parallel_driver *pdrv);
extern state parallel_host_register(struct parallel_host *host);
extern void parallel_host_unregister(struct parallel_host *host);

/* convenience logging macros */
#define parallel_emerg(sdev, fmt, ...)  dev_emerg(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define parallel_alert(sdev, fmt, ...)  dev_alert(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define parallel_crit(sdev, fmt, ...)   dev_crit(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define parallel_err(sdev, fmt, ...)    dev_err(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define parallel_warn(sdev, fmt, ...)   dev_warn(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define parallel_notice(sdev, fmt, ...) dev_notice(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define parallel_info(sdev, fmt, ...)   dev_info(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define parallel_debug(sdev, fmt, ...)  dev_debug(&(sdev)->dev, fmt, ##__VA_ARGS__)
BUS_DEVRES_GENERIC(parallel, struct parallel_device, dev)

#endif  /* _DRIVER_PARALLEL_H_ */

