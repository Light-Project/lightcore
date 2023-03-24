/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPIO_ICH_H_
#define _DRIVER_GPIO_ICH_H_

enum ich_gpio_register {
    ICH_GPIO_USE_SEL    = 0x00, /* RW: GPIO Use Select */
    ICH_GPIO_IO_SEL     = 0x04, /* RW: GPIO Input/Output Select */
    ICH_GPIO_GP_SEL     = 0x0c, /* RW: GPIO Level for Input or Output */
    ICH_GPIO_LUSE_SEL   = 0x10, /* RW: GPIO Use Select Override Low */
    ICH_GPIO_GPO_BLANK  = 0x18, /* RW: GPIO Blink Enable */
    ICH_GPIO_SER_BLANK  = 0x1c, /* RW: GP Serial Blink */
    ICH_GPIO_SB_CMDSTS  = 0x20, /* RW: GP Serial Blink Command Status */
    ICH_GPIO_SB_DATA    = 0x24, /* RW: GP Serial Blink Data */
    ICH_GPIO_GPI_INV    = 0x2c, /* RW: GPIO Signal Invert */
};

#define ICH_GPIO_INDEX_NUM          3
#define ICH_GPIO_INDEX_SIZE         0x30

#define ICH_GPIO_USE_SEL(index)     (ICH_GPIO_USE_SEL   + (index) * 0x30)
#define ICH_GPIO_IO_SEL(index)      (ICH_GPIO_IO_SEL    + (index) * 0x30)
#define ICH_GPIO_GP_SEL(index)      (ICH_GPIO_GP_SEL    + (index) * 0x30)
#define ICH_GPIO_LUSE_SEL(index)    (ICH_GPIO_LUSE_SEL  + (index) * 0x30)
#define ICH_GPIO_GPO_BLANK(index)   (ICH_GPIO_GPO_BLANK + (index) * 0x30)
#define ICH_GPIO_SER_BLANK(index)   (ICH_GPIO_SER_BLANK + (index) * 0x30)
#define ICH_GPIO_SB_CMDSTS(index)   (ICH_GPIO_SB_CMDSTS + (index) * 0x30)
#define ICH_GPIO_SB_DATA(index)     (ICH_GPIO_SB_DATA   + (index) * 0x30)
#define ICH_GPIO_GPI_INV(index)     (ICH_GPIO_GPI_INV   + (index) * 0x30)

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

#endif /* _DRIVER_GPIO_ICH_H_ */
