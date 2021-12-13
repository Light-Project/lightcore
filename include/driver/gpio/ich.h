/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPIO_ICH_H_
#define _DRIVER_GPIO_ICH_H_

enum intel_gpio_resources {
    INTEL_RES_GPIO      = 0,
    INTEL_RES_GPE0      = 1,
};

#define INTEL_GPIO_I3100    "intel,gpio-i3100"
#define INTEL_GPIO_V5       "intel,gpio-v5"
#define INTEL_GPIO_V6       "intel,gpio-v6"
#define INTEL_GPIO_V7       "intel,gpio-v7"
#define INTEL_GPIO_V9       "intel,gpio-v9"
#define INTEL_GPIO_V10CP    "intel,gpio-v10cp"
#define INTEL_GPIO_V10CS    "intel,gpio-v10cs"

#endif  /* _DRIVER_GPIO_ICH_H_ */
