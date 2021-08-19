/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_PLATFORM_H_
#define _DEVICE_PLATFORM_H_

#include <device.h>
#include <resource.h>
#include <mod_devicetable.h>

extern struct bus_type platform_bus;

struct platform_device {
    const char *name;               /* Match witch driver */
    struct device device;           /* Generic device */

    int         id;
    bool        id_auto;
    uint64_t    platform_dma_mask;

    /* I/O resource */
    struct resource *resource;
    unsigned int resources_nr;

    struct acpi_node *acpi_node;
    struct dt_node *dt_node;
};

#define device_to_platform_device(dev)	\
    (container_of((dev), struct platform_device, device))

static inline void *platform_get_devdata(const struct platform_device *pdev)
{
    return dev_get_devdata((const struct device *)&pdev->device);
}

static inline void platform_set_devdata(struct platform_device *pdev, void *data)
{
    return dev_set_devdata((struct device *)&pdev->device, data);
}

struct platform_driver {
    struct driver driver;
    const struct dt_device_id       *dt_table;
    const struct acpi_device_id     *acpi_table;
    const struct platform_device_id *platform_table;

    state (*probe)(struct platform_device *pdev);
    state (*remove)(struct platform_device *pdev);
    state (*shutdown)(struct platform_device *pdev);
    state (*suspend)(struct platform_device *pdev, pm_message_t state);
    state (*resume)(struct platform_device *pdev);
};

#define driver_to_platform_driver(drv)	\
    (container_of((drv), struct platform_driver, driver))

static inline resource_size_t
platform_resource_start(struct platform_device *pdev, unsigned int index)
{
    if (index > pdev->resources_nr)
        return 0;
    return pdev->resource[index].start;
}

static inline resource_size_t
platform_resource_end(struct platform_device *pdev, unsigned int index)
{
    if (index > pdev->resources_nr)
        return 0;
    return pdev->resource[index].end;
}

static inline resource_size_t
platform_resource_size(struct platform_device *pdev, unsigned int index)
{
    return platform_resource_end(pdev, index) -
           platform_resource_start(pdev, index);
}

static inline enum res_type
platform_resource_type(struct platform_device *pdev, unsigned int index)
{
    if (index > pdev->resources_nr)
        return RESOURCE_NONE;
    return pdev->resource[index].type;
}

#if defined(CONFIG_DT)
const struct dt_device_id *platform_dt_match(struct platform_driver *pdrv, struct platform_device *pdev);
#endif
#if defined(CONFIG_ACPI)
const struct dt_device_id *platform_acpi_match(struct platform_driver *pdrv, struct platform_device *pdev);
#endif

struct platform_device *platform_device_alloc(const char *name, int id);
struct resource *platform_name_resource(struct platform_device *pdev, const char *name);

extern state platform_device_register(struct platform_device *);
extern void platform_device_unregister(struct platform_device *);
extern state platform_driver_register(struct platform_driver *);
extern void platform_driver_unregister(struct platform_driver *);

#endif  /* _DEVICE_PLATFORM_H_ */
