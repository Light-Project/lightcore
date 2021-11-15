/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MTD_SPINOR_
#define _DRIVER_MTD_SPINOR_

#include <state.h>
#include <bits.h>
#include <driver/mtd.h>

enum norflash_commands {
    /* Status command */
    NORFLASH_STATUS_READ1       = 0x05,
    NORFLASH_STATUS_READ2       = 0x35,
    NORFLASH_STATUS_WRITE       = 0x01,
    NORFLASH_JEDEC_ID           = 0x9f,

    /* Read command */
    NORFLASH_READ               = 0x03,
    NORFLASH_FAST_READ          = 0x0b,
    NORFLASH_READ_DUAL_OUT      = 0x3b,
    NORFLASH_READ_DUAL_IO       = 0xbb,
    NORFLASH_READ_QUAD_OUT      = 0x6b,
    NORFLASH_READ_QUAD_IO       = 0xeb,
    NORFLASH_READ_OCTAL_OUT     = 0x8b,
    NORFLASH_READ_OCTAL_IO      = 0xcb,

    /* Write command */
    NORFLASH_WRITE              = 0x02,
    NORFLASH_WRITE_QUAD_OUT     = 0x32,
    NORFLASH_WRITE_QUAD_IO      = 0x38,
    NORFLASH_WRITE_OCTAL_OUT    = 0x82,
    NORFLASH_WRITE_OCTAL_IO     = 0xc2,

    /* Erase command */
    NORFLASH_ERASE_SECTOR       = 0x20,
    NORFLASH_ERASE_BLOCK_32K    = 0x52,
    NORFLASH_ERASE_BLOCK_64K    = 0xd8,
    NORFLASH_ERASE_CHIP         = 0xc7,

    NORFLASH_WRITE_DISABLE      = 0x04,
    NORFLASH_WRITE_ENABLE       = 0x06,

    NORFLASH_POWERDOWN          = 0xb9,
};

#define NORFLASH_STR_BUSY       BIT(0)  /* Write or erase in progress */
#define NORFLASH_STR_WEL        BIT(1)  /* Write enable latch */
#define NORFLASH_STR_BP0        BIT(2)  /* Block protect BIT0 */
#define NORFLASH_STR_BP1        BIT(3)  /* Block protect BIT1 */
#define NORFLASH_STR_BP2        BIT(4)  /* Block protect BIT2 */
#define NORFLASH_STR_TB         BIT(5)  /* Top/Bottom write protect */
#define NORFLASH_STR_SEC        BIT(6)  /* Sector write protect */
#define NORFLASH_STR_SRP0       BIT(7)  /* Status write protect */

struct spinor_device {
    struct device *parent;
    struct mtd_device mtd;
    struct spinor_ops *ops;

    uint8_t addr_width;
    uint8_t read_op;
    uint8_t write_op;
    uint8_t erase_op;
};

struct spinor_ops {
    state (*reg_read)(struct spinor_device *, uint8_t op, void *buf, uint8_t len);
    state (*reg_write)(struct spinor_device *, uint8_t op, void *buf, uint8_t len);
    state (*read)(struct spinor_device *, loff_t pos, void *buf, uint64_t len);
    state (*write)(struct spinor_device *, loff_t pos, void *buf, uint64_t len);
    state (*erase)(struct spinor_device *, loff_t pos, uint64_t len);
};

struct spinor_type {
    const char *name;
    uint32_t jedec;
    uint32_t extid;

    uint32_t sector_size;
    uint16_t sector_nr;
    uint16_t page_size;

    struct list_head list;
};

extern void spinor_type_register(struct spinor_type *stype);
extern void spinor_type_unregister(struct spinor_type *stype);

extern state spinor_register(struct spinor_device *);
extern void spinor_unregister(struct spinor_device *);

#endif  /* _DRIVER_MTD_NORFLASH_ */
