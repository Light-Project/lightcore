/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_BASE_H_
#define _DEVICE_BASE_H_

#define bus_for_each_device(device, bus) \
        list_for_each_entry(device, &bus->devices_list, bus_list_device)

#define bus_for_each_driver(driver, bus) \
        list_for_each_entry(driver, &bus->drivers_list, bus_list_driver)

/* Initialization functions */
void bus_init(void);
void platform_bus_init(void);

/* Bus highlevel API */
state bus_device_match(struct device *dev);
state bus_device_probe(struct device *dev);
state bus_device_add(struct device *drv);
void bus_device_remove(struct device *drv);

state bus_driver_add(struct driver *drv);
void bus_driver_remove(struct driver *dev);


/* Platform device tree interactive */
void platform_dt_init(void);

#endif
