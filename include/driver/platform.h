/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_PLATFORM_H_
#define _DEVICE_PLATFORM_H_

#include <device.h>
#include <resource.h>
#include <devtable.h>

extern struct bus_type platform_bus;

struct platform_device {
    struct device dev;      /* Generic device */
    const char *name;       /* Match witch driver */

    int         id;
    bool        id_auto;
    uint64_t    platform_dma_mask;

    struct acpi_node *acpi_node;
    struct dt_node *dt_node;

    /* I/O resource */
    struct resource *resource;
    unsigned int resources_nr;
};

#define device_to_platform(devp)	\
    container_of((devp), struct platform_device, dev)

static inline void *platform_get_devdata(const struct platform_device *pdev)
{
    return device_get_pdata(&pdev->dev);
}

static inline void platform_set_devdata(struct platform_device *pdev, void *data)
{
    return device_set_pdata(&pdev->dev, data);
}

struct platform_driver {
    struct driver driver;
    const struct dt_device_id       *dt_table;
    const struct acpi_device_id     *acpi_table;
    const struct platform_device_id *platform_table;

    state (*probe)(struct platform_device *pdev, const void *pdata);
    void (*remove)(struct platform_device *pdev);
    void (*shutdown)(struct platform_device *pdev);
    state (*suspend)(struct platform_device *pdev, pm_message_t state);
    state (*resume)(struct platform_device *pdev);
};

#define driver_to_platform_driver(drv) \
    container_of(drv, struct platform_driver, driver)

#ifdef CONFIG_DT
const struct dt_device_id *platform_dt_match(struct platform_driver *pdrv, struct platform_device *pdev);
#endif

#ifdef CONFIG_ACPI
const struct acpi_device_id *platform_acpi_match(struct platform_driver *pdrv, struct platform_device *pdev);
#endif

extern resource_size_t platform_resource_start(struct platform_device *pdev, unsigned int index);
extern resource_size_t platform_resource_size(struct platform_device *pdev, unsigned int index);
extern resource_size_t platform_resource_end(struct platform_device *pdev, unsigned int index);

extern enum resource_type platform_resource_type(struct platform_device *pdev, unsigned int index);
extern struct resource *platform_get_resource(struct platform_device *pdev, unsigned int index, enum resource_type type);
extern struct resource *platform_name_resource(struct platform_device *pdev, const char *name);

extern struct platform_device *platform_device_alloc(const char *name, int id);
extern state platform_device_register(struct platform_device *);
extern void platform_device_unregister(struct platform_device *);
extern state platform_driver_register(struct platform_driver *);
extern void platform_driver_unregister(struct platform_driver *);

extern void __init platform_dt_init(void);
extern void __init platform_bus_init(void);

#endif  /* _DEVICE_PLATFORM_H_ */
